#include <gtest/gtest.h>
#include <sstream>
#include <list>
#include <tuple>
#ifndef __PRETTY_FUNCTION__
#include "pretty.h"
#endif
#include "myallocator.h"
#include "mycontainers.h"


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

using namespace std::literals::string_literals;
using otus_hw3::std_11_pool_allocator;
using otus_hw3::std_11_pool_dealloc_allocator;
using otus_hw3::MyList;

TEST(test_std11, test_std_vec_resize)
{
    auto v = std::vector<int, std_11_pool_allocator<int>>{};
    v.reserve(5);

    try
    {
        for (int i = 0; i < 6; ++i)
        {
            std::cout << "vector size = " << v.size() << std::endl;
            v.emplace_back(i);
        }
    }
    catch(std::bad_alloc& excp)
    {
        std::cerr << __PRETTY_FUNCTION__ << ", exception: " <<  excp.what() << std::endl;
    }

    EXPECT_EQ( v.size(), 5 );
    EXPECT_GE( v.capacity(), 5 );
}

TEST(test_std11, test_std_map_add)
{
    using Alloc_t = std_11_pool_allocator<std::pair<const int, double>, 10, 0>;
    auto m = std::map<int, double, std::less<int>, Alloc_t>{};
    
    for (int i = 0; i < 2; ++i)
    {
        m[i] = static_cast<decltype(m)::mapped_type>(i);
    }
    EXPECT_EQ( m.size(), 2 );
}


TEST(test_std11, test_std_map_resize)
{
    using Alloc_t = std_11_pool_allocator<std::pair<const int, double>, 10, 1>;
    auto m = std::map<int, double, std::less<int>, Alloc_t>{};
    constexpr const int max_items = decltype(m)::allocator_type::size_;   
    
    try
    {
        for (int i = 0; i <= max_items ; ++i)
        {
            m[i] = static_cast<typename decltype(m)::mapped_type>(i);
        }    
    }
    catch(std::bad_alloc& excp)
    {
        std::cerr << __PRETTY_FUNCTION__ << ", exception: " <<  excp.what() << std::endl;
    }
    EXPECT_EQ( m.size(), max_items ); 
}


TEST(test_std11, test_mylist_add)
{
    auto L = MyList<int, std_11_pool_allocator<int>>{};
    
    for (int i = 0; i < 2; ++i)
    {
        L.push_back(i);
    }
    EXPECT_EQ( L.size(), 2 );
}


TEST(test_std11, test_mylist_oversize)
{
    auto L = MyList<int, std_11_pool_allocator<int, 10, 1>>{};
    constexpr const int max_items = decltype(L)::allocator_type::size_;   
    try
    {
        for (int i = 0; i <= max_items; ++i)
        {
            L.push_back(i);
        }
        }
    catch(std::bad_alloc& excp)
    {
        std::cerr << __PRETTY_FUNCTION__ << ", exception: " <<  excp.what() << std::endl;
    }
    EXPECT_EQ( L.size(), max_items );
}

TEST(test_std11, test_stdlist_move)
{
    using Alloc_t = std_11_pool_allocator<int, 10, 2>;
    std::list<int, Alloc_t> L;
    L.push_back(3);

    std::list<int, Alloc_t> L2;
    L2 = std::move(L);

    EXPECT_GT( L2.size(), L.size());
}

TEST(test_std11, test_stdvec_move)
{
    using Alloc_t = std_11_pool_allocator<int, 100, 3>;
    // why we need operators == & !=
    std::vector<int, Alloc_t> v0(10, 37);
    auto v1 = std::vector<int, Alloc_t>{5};
    v1 = std::move(v0); // if operator ==() = false for 2 allocators,
    // and propagate_on_container_move_assignment = std::false_type, then move is copy
    EXPECT_EQ( v0.size(), 0);

    auto v11 = std::move(v1);
    EXPECT_EQ( v1.size(), 0);

    std::cout << "BEFORE v2" << std::endl;

    using Alloc2_t = std_11_pool_allocator<int, 10, 3>;
    std::vector<int, Alloc_t> v2(std::move(v11));
    EXPECT_GT( v2.size(), v1.size());
    std::cout << "At end" << std::endl;
}

TEST(test_std11, test_fact)
{
    auto fact = [](int N) -> int {
        int F = 1;
        for(; N > 1 ; --N) F = F * N;
        return F;  
    };

    EXPECT_EQ( fact(0), 1);
    EXPECT_EQ( fact(1), 1);
    EXPECT_EQ( fact(2), 2);
    EXPECT_EQ( fact(3), 6);
    EXPECT_EQ( fact(4), 24);
    EXPECT_EQ( fact(5), 120);
    EXPECT_EQ( fact(6), 720);

}

TEST(test_std11, test_std_map_dealloc)
{
    struct Val_t{ 
        std::string nm_; double val_; 
        ~Val_t() noexcept 
        { 
            std::cout << __PRETTY_FUNCTION__ << ": " << std::hex << this << std::endl; 
        
        }
        Val_t() : val_{} {} 
        Val_t(const std::string nm, const double v) : nm_(nm), val_{v} {} 
    };
    using Alloc_t = std_11_pool_dealloc_allocator<std::pair<const int, Val_t>, 10, 3>;
    auto m = std::map<int, Val_t, std::less<int>, Alloc_t>{};
    constexpr const int max_items = decltype(m)::allocator_type::size_;   
    
    try
    {
        for (int i = 0; i < max_items ; ++i)
        {
            m[i] = decltype(m)::mapped_type(""s, static_cast<const double>(i));
        }
        
        for (int i = 0; i < max_items ; i += 2)
        {
            m.erase(i);
        }
    }
    catch(std::bad_alloc& excp)
    {
        std::cerr << __PRETTY_FUNCTION__ << ", exception: " <<  excp.what() << std::endl;
    }
    EXPECT_EQ( m.size(), max_items/2 ); 
}

TEST(test_std11, test_std_vec_fragmented)
{
    struct Val_t{ 
        std::string nm_; double val_; 
        ~Val_t() noexcept 
        { 
            std::cout << __PRETTY_FUNCTION__ << ": " << std::hex << this << std::endl; 
        
        }
        Val_t() : val_{} {} 
        Val_t(const std::string nm, const double v) : nm_(nm), val_{v} {} 
    };

    using Alloc_t = std_11_pool_dealloc_allocator<Val_t, 10, 3>;
    auto v = std::vector<Val_t, Alloc_t>{};
    constexpr const int max_items = decltype(v)::allocator_type::size_;   
    
    try
    {
        v.reserve(max_items/2);
        for (int i = 0; i < max_items ; ++i)
        {
            v.emplace_back( decltype(v)::value_type(""s, static_cast<const double>(i)) );
        }

        EXPECT_EQ( v.size(), max_items/2 ); 
        
        for (int i = 0; i < max_items ; i += 2)
        {
            v.erase(v.begin());
        }
    }
    catch(std::bad_alloc& excp)
    {
        std::cerr << __PRETTY_FUNCTION__ << ", exception: " <<  excp.what() << std::endl;
    }
    EXPECT_EQ( v.size(), max_items/2); 
}


