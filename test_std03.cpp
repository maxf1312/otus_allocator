#include <gtest/gtest.h>
#include <sstream>
#include <map>
#include "myallocator.h"
#include "mycontainers.h"

using otus_hw3::std_03_allocator;
using otus_hw3::MyList;


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(test_std03, test_std_vec_resize)
{
    auto v = std::vector<int, std_03_allocator<int>>{};
    v.reserve(5);
    for (int i = 0; i < 6; ++i)
    {
        std::cout << "vector size = " << v.size() << std::endl;
        v.push_back(i);
    }
    EXPECT_EQ( v.size(), 6 );
    EXPECT_GE( v.capacity(), 6 );
}

TEST(test_std03, test_std_map_resize)
{
    auto m = std::map<int, double, std::less<int>, 
                      std_03_allocator<
                        std::pair<const int, double>>>{};
    
    for (int i = 0; i < 2; ++i)
    {
        m[i] = static_cast<decltype(m)::mapped_type>(i);
    }
    EXPECT_EQ( m.size(), 2 );
}


TEST(test_std03, test_mylist_add)
{
    auto L = MyList<int, std_03_allocator<int>>{};
    
    for (int i = 0; i < 2; ++i)
    {
        L.push_back(i);
    }
    EXPECT_EQ( L.size(), 2 );
}

TEST(test_std03, test_alloca_stateless)
{
    // why 03 allocators stateless
    std_03_allocator<int> a1;
    std_03_allocator<int> a2;

    std::vector<int, std_03_allocator<int>> v1(10, a1);
    std::vector<int, std_03_allocator<int>> v2(10, a2);

    v1.swap(v2); // no swap support for allocators ???

    EXPECT_EQ( v1.size(), v2.size() );
    EXPECT_EQ( v1.get_allocator(), v2.get_allocator() );
}

TEST(test_std03, test_alloca_move)
{
    using otus_hw3::Deleter;

    // why we need copy constructor
    std::allocator<int> stdAl;
    std::unique_ptr<int> smartPtr(stdAl.allocate(10));

    std::vector<int> vect(10, stdAl);

    std_03_allocator<int> al;
    std_03_allocator<float> al2(al);

    std::unique_ptr<int, Deleter<int[], std_03_allocator<int>> > sp1(al.allocate(10), Deleter<int[], std_03_allocator<int>>(al, 10, false)); 

    std::vector<int, decltype(al)> vect2(10, al);

    EXPECT_EQ( vect2.size(), 10 );
    EXPECT_EQ( vect2.get_allocator(), al2 );
}


