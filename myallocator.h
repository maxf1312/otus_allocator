#pragma once
#ifndef __MYALLOCATOR_H__
#define __MYALLOCATOR_H__

#ifndef __PRETTY_FUNCTION__
#include "pretty.h"
#endif

#include <memory>
#include <utility>
#include <bitset>

#ifdef USE_DBG_TRACE
#ifndef USE_PRETTY
#define DBG_TRACE( func, trace_statements )\
		std::cout << func << trace_statements << std::endl;
#else
#define DBG_TRACE( func, trace_statements )\
		std::cout << __PRETTY_FUNCTION__ << trace_statements << std::endl;
#endif
#else
#define DBG_TRACE( func, trace_statements )   
#endif // USE_DBG_TRACE

namespace otus_hw3{

// stateless allocator
template <class T>
struct std_03_allocator
{
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;

    std_03_allocator() noexcept {}
    template <class U>
    std_03_allocator(const std_03_allocator<U> &) noexcept {}

    T *allocate(size_t n)
    {
#ifndef USE_PRETTY
		std::cout << "allocate: [n = " << std::dec << n << "]" << std::endl;
#else
		std::cout << __PRETTY_FUNCTION__ << "[n = " << std::dec << n << "]" << std::endl;
#endif
        return static_cast<T *>(::operator new(n * sizeof(T)));
    }
    void deallocate(T *p, size_t n)
    {
#ifndef USE_PRETTY
		std::cout << "deallocate: [p = " << std::hex << p << ", n = " << std::dec << n << "]" << std::endl;
#else
		std::cout << __PRETTY_FUNCTION__ << "[p = " << std::hex << p << ", n = " << std::dec  << n << "]" << std::endl;
#endif
    ::operator delete(p);
    }

    template <class Up, class... Args>
    void construct(Up *p, Args &&...args)
    {
#ifndef USE_PRETTY
		std::cout << "construct at " << std::hex << p << std::endl;
#else
		std::cout << __PRETTY_FUNCTION__ << std::hex << p << std::endl;
#endif
        ::new ((void *)p) Up(std::forward<Args>(args)...);
    }

    void destroy(pointer p)
    {
#ifndef USE_PRETTY
		std::cout << "destroy at " << std::hex << p << std::endl;
#else
		std::cout << __PRETTY_FUNCTION__ << std::hex << p << std::endl;
#endif
        p->~T();
    }

    template <class U>
    struct rebind
    {
        typedef std_03_allocator<U> other;
    };
};

template <class T, class U>
constexpr bool operator==(const std_03_allocator<T> &a1, const std_03_allocator<U> &a2) noexcept
{
    std::ignore = a1;
    std::ignore = a2;
    return true;
}

template <class T, class U>
constexpr bool operator!=(const std_03_allocator<T> &a1, const std_03_allocator<U> &a2) noexcept
{
    std::ignore = a1;
    std::ignore = a2;
    return false;
}

// statefull Cxx11 allocator of fixed pool
template <class T, const size_t POOL_SIZE = 10, const int tag = 0>
struct std_11_pool_allocator
{
    using Alloc_t =  std_11_pool_allocator<T, POOL_SIZE, tag>;  
    //typedef T value_type;

    using value_type = T; 
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&; 
    using const_reference = T const&; 
    using size_type	= std::size_t;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::true_type;  

    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type; // UB if std::false_type and a1 != a2;


    static constexpr const size_t size_ = POOL_SIZE;
    static constexpr const size_t mem_size_ = size_ * sizeof(T);
    static uint8_t pool_[mem_size_];
    static size_t  pos_;

    std_11_pool_allocator() noexcept {}
    ~std_11_pool_allocator() {}

    template <class U>
    std_11_pool_allocator(const std_11_pool_allocator<U, POOL_SIZE, tag> &) noexcept {}

    std_11_pool_allocator select_on_container_copy_construction() const
    {
        DBG_TRACE("std_11_pool_allocator::select_on_container_copy_construction()", "" )
        return std_11_pool_allocator<T, POOL_SIZE, tag>{};
    }
   
    constexpr size_type max_size() const noexcept
    {
        //std::numeric_limits<size_type>::max() / sizeof(value_type);        
        return size_;
    }

    template <class U>
    struct rebind
    {
        typedef std_11_pool_allocator<U, POOL_SIZE, tag> other;
    };

    T *allocate(size_t n)
    {
        DBG_TRACE("allocate", ": [n = " << std::dec << n << "]" )
        if( pos_ + n > size_ )
            throw std::bad_alloc();
        
        size_t current_offset = pos_;
        pos_ += n;
        return reinterpret_cast<T*>(pool_) + current_offset;
    }

    void deallocate(T *p, size_t n)
    {
        std::ignore = p;
        std::ignore = n;
        DBG_TRACE("deallocate", ": [p = " << std::hex << p << ", n = " << std::dec << n << "]" )
    }

    template <class Up, class... Args>
    void construct(Up *p, Args &&...args)
    {
        DBG_TRACE("construct at", std::hex << p )
        ::new (p) Up(std::forward<Args>(args)...);
    }

    void destroy(pointer p)
    {
        DBG_TRACE("destroy at", std::hex << p )
        p->~T();
    }
};

template <class T, const size_t POOL_SIZE, const int tag>
size_t std_11_pool_allocator<T, POOL_SIZE, tag>::pos_ = 0;

template <class T, const size_t POOL_SIZE, const int tag>
uint8_t std_11_pool_allocator<T, POOL_SIZE, tag>::pool_[std_11_pool_allocator<T, POOL_SIZE, tag>::mem_size_] = {0};


template <class T, class U, const size_t POOL_SIZE_T, const int tag_T, const size_t POOL_SIZE_U, const int tag_U>
constexpr bool operator==(const std_11_pool_allocator<T,POOL_SIZE_T,tag_T> &a1, const std_11_pool_allocator<U,POOL_SIZE_U,tag_U> &a2) noexcept
{
    return a1.pool_ == a2.pool_;
}

template <class T, class U, const size_t POOL_SIZE_T, const int tag_T, const size_t POOL_SIZE_U, const int tag_U>
constexpr bool operator!=(const std_11_pool_allocator<T,POOL_SIZE_T,tag_T> &a1, const std_11_pool_allocator<U,POOL_SIZE_U,tag_U> &a2) noexcept
{
    return a1.pool_ != a2.pool_;
}

template<typename T, typename Alloc>
struct Deleter
{
    Alloc alloc_;
    Deleter(Alloc const& alloc) : alloc_(alloc){} 
    Deleter(Alloc&& alloc) : alloc_(alloc){} 
    void operator()(T* p)
    {
        if(p)
        {
            alloc_.destroy(p);
            alloc_.deallocate(p, 1);
        }
    }
};

template<typename T, typename Alloc>
struct Deleter<T[], Alloc>
{
    Alloc alloc_;
    size_t n_;
    bool constructed_;
    Deleter(Alloc const& alloc, size_t N, bool constructed) : alloc_(alloc), n_(N), constructed_(constructed){} 
    Deleter(Alloc&& alloc, size_t N, bool constructed) : alloc_(alloc), n_(N), constructed_(constructed){} 
    void operator()(T* p)
    {
        if(p)
        {
            if( constructed_ )
                for(size_t k = n_; k > 0; )            
                    alloc_.destroy(p + --k);
            alloc_.deallocate(p, n_);
        }
    }
};


// statefull Cxx11 allocator of fixed pool with deallocate
template <class T, const size_t POOL_SIZE = 10, const int tag = 0>
struct std_11_pool_dealloc_allocator
{
    using Alloc_t =  std_11_pool_dealloc_allocator<T, POOL_SIZE, tag>;  

    using value_type = T; 
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&; 
    using const_reference = T const&; 
    using size_type	= std::size_t;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::true_type;  

    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type; // UB if std::false_type and a1 != a2;


    static constexpr const size_t size_ = POOL_SIZE;
    static constexpr const size_t mem_size_ = size_ * sizeof(T);
    static uint8_t pool_[mem_size_];
    using used_bitmap_t = std::bitset<size_>;
    static used_bitmap_t map_of_blocks_;

    std_11_pool_dealloc_allocator() noexcept {}
    ~std_11_pool_dealloc_allocator() {}

    template <class U>
    std_11_pool_dealloc_allocator(const std_11_pool_dealloc_allocator<U, POOL_SIZE, tag> &) noexcept {}

    std_11_pool_dealloc_allocator select_on_container_copy_construction() const
    {
        DBG_TRACE("std_11_pool_dealloc_allocator::select_on_container_copy_construction()", "" )
        return std_11_pool_dealloc_allocator<T, POOL_SIZE, tag>{};
    }
   
    constexpr size_type max_size() const noexcept
    {
        //std::numeric_limits<size_type>::max() / sizeof(value_type);        
        return size_;
    }

    template <class U>
    struct rebind
    {
        typedef std_11_pool_dealloc_allocator<U, POOL_SIZE, tag> other;
    };

    T *allocate(size_t n)
    {
        DBG_TRACE("allocate", ": [n = " << std::dec << n << "]" )
        size_t current_offset = find_free_pos(n);
        while(n--) 
            map_of_blocks_.set(current_offset + n, true);
        DBG_TRACE( "allocate", *this )
        return reinterpret_cast<T*>(pool_) + current_offset;
    }

    void deallocate(T *p, size_t n)
    {
        DBG_TRACE("deallocate", ": [p = " << std::hex << p << ", n = " << std::dec << n << "]" )

        uint8_t* p_mem = reinterpret_cast<uint8_t*>(p);
        std::ptrdiff_t offset = std::distance(pool_, p_mem);
        if( offset % sizeof(T) )
            throw std::runtime_error("p is incorrect address of memory block");
        offset /= sizeof(T);
        while(n--) map_of_blocks_.reset(offset + n);
        DBG_TRACE( "deallocate", *this )
    }

    template <class Up, class... Args>
    void construct(Up *p, Args &&...args)
    {
        DBG_TRACE("construct at", std::hex << p )
        ::new (p) Up(std::forward<Args>(args)...);
    }

    void destroy(pointer p)
    {
        DBG_TRACE("destroy at", std::hex << p )
        p->~T();
    }

    size_t find_free_pos(size_t n)
    {
        bool  i_beg_init{};
        size_t i_beg, zero_count{};
        for(size_t i = 0; i < map_of_blocks_.size() && zero_count < n; ++i)
        {
            if ( map_of_blocks_.test(i) )
            {
                zero_count = 0;
                i_beg_init = false;
                continue;
            }

            if (!i_beg_init)
            {
                i_beg_init = true;
                i_beg = i;
            }
            ++zero_count;
        }

        if (zero_count < n)
            throw std::bad_alloc();
        return i_beg;        
    }

    std::ostream& out_block_map(std::ostream& os) const
    {
        for(size_t i = 0; i < map_of_blocks_.size(); ++i)
            os << (map_of_blocks_.test(i) ? "1" : "0");
        os << std::endl;
        return os;
    }
};

template <class T, const size_t POOL_SIZE, const int tag>
uint8_t std_11_pool_dealloc_allocator<T, POOL_SIZE, tag>::pool_[std_11_pool_dealloc_allocator<T, POOL_SIZE, tag>::mem_size_] = {0};

template <class T, const size_t POOL_SIZE, const int tag>
typename std_11_pool_dealloc_allocator<T, POOL_SIZE, tag>::used_bitmap_t std_11_pool_dealloc_allocator<T, POOL_SIZE, tag>::map_of_blocks_{};

template <class T, class U, const size_t POOL_SIZE_T, const int tag_T, const size_t POOL_SIZE_U, const int tag_U>
constexpr bool operator==(const std_11_pool_dealloc_allocator<T,POOL_SIZE_T,tag_T> &a1, const std_11_pool_dealloc_allocator<U,POOL_SIZE_U,tag_U> &a2) noexcept
{
    return a1.pool_ == a2.pool_;
}

template <class T, class U, const size_t POOL_SIZE_T, const int tag_T, const size_t POOL_SIZE_U, const int tag_U>
constexpr bool operator!=(const std_11_pool_dealloc_allocator<T,POOL_SIZE_T,tag_T> &a1, const std_11_pool_dealloc_allocator<U,POOL_SIZE_U,tag_U> &a2) noexcept
{
    return a1.pool_ != a2.pool_;
}

template <class T, const size_t POOL_SIZE_T, const int tag_T>
constexpr std::ostream& operator<<(std::ostream& os, const std_11_pool_dealloc_allocator<T,POOL_SIZE_T,tag_T> &a) noexcept
{
    return a.out_block_map(os);
}


} // otus_hw3

#endif //__MYALLOCATOR_H__
