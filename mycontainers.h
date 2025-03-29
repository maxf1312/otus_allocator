#pragma once
#ifndef __MYCONTAINERS_H__
#define __MYCONTAINERS_H__

#ifndef __PRETTY_FUNCTION__
#include "pretty.h"
#endif

#include <memory>
#include <iterator>

namespace otus_hw3{

    template <typename T, typename Alloc>
    class MyList
    {
        friend class InputIter;
        struct Node
        {
            Node *next;
            T val;

            Node(): next(nullptr) {}
        };
    public:
        using value_type = T; 
        using reference = T&; 
        using const_reference = T const&; 
        using size_type = size_t;
        using allocator_type = Alloc; 
        using pointer = typename std::allocator_traits<Alloc>::pointer;
        using const_pointer = typename std::allocator_traits<Alloc>::const_pointer;

        class InputIter : public std::iterator<std::input_iterator_tag, value_type> {
            friend class MyList;           
            Node* cur_node_;
            InputIter( MyList<T, Alloc>::Node* node = nullptr) : cur_node_(node){} 
        public:
            InputIter& operator++(){ if(cur_node_) cur_node_ = cur_node_->next; return *this; }
            InputIter  operator++(int){ InputIter rv = *this; ++(*this); return rv; }
            bool operator==(InputIter const& rhs) const { return cur_node_ == rhs.cur_node_; }
            bool operator!=(InputIter const& rhs) const { return !(*this == rhs); }
            reference operator*() const { return cur_node_->val; }            
        };
         
        using iterator = InputIter;
        using const_iterator = InputIter;        
        
        ~MyList() noexcept { clear(); }  
        MyList() : head{nullptr}, size_{} {}  
        MyList(const MyList& rhs) : head{nullptr}, size_{} { *this = rhs; }  
        MyList& operator=(const MyList& rhs)
        {
            if( this != &rhs )
            {
                clear();
                for( Node* p = rhs.head; *p; p = p->next )
                    push_back(p->val);    
            }
            return *this;
        }

        MyList&& operator=(MyList&& rhs)
        {
            if( this != &rhs )
            {
                clear();
                swap(rhs);
            }
            return *this;
        }
        
        MyList(MyList&& rhs) : head{nullptr}, size_{} 
        {
            swap(rhs);
        }  
        
        size_type size() const 
        {
            return size_;
        } 

        size_type empty() const 
        {
            return !size_;
        } 

        void swap(MyList& rhs)
        {
            std::swap(head,      rhs.head);
            std::swap(size_,     rhs.size_);
            std::swap(allocator_, rhs.allocator_);
        }

        void clear()
        {
            while( head )
            {
                Node* tmp = head;
                head = head->next;
                typename Alloc::template rebind<Node>::other nodeAlloc;
                nodeAlloc.destroy(tmp);
                nodeAlloc.deallocate(tmp, 1);
            }
            size_ = 0;
        }

        void push_back(const T &val)
        {
            typename Alloc::template rebind<Node>::other nodeAlloc;
            Node *newNode = nodeAlloc.allocate(1);
            nodeAlloc.construct(newNode);
            allocator_.construct(&newNode->val, val);
            Node** pp = &head;
            for(; *pp ; pp = &(*pp)->next);
            *pp = newNode;
            ++size_;
        }

        iterator begin() const { return iterator(head); }
        iterator end()   const { return iterator{}; }

        iterator cbegin() const { return const_iterator(head); }
        iterator cend()   const { return const_iterator{}; }
    private:
        Node *head;
        size_type size_;
        Alloc allocator_;
    };
    


};  // namespace otus_hw3{

#endif //__MYCONTAINERS_H__