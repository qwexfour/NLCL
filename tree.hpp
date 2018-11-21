#ifndef TREE_LIB
#define TREE_LIB

#include <cassert>
#include <algorithm>
#include <deque>
#include <iostream>
#include <variant>
#include <memory>
#include <array>

template<typename Iter>
void Dump(Iter first, Iter last)
{
    std::for_each(first, last, [] (const auto& elem) {std::cout << elem << " ";});
    std::cout << std::endl;
}

namespace tree
{

namespace detail
{
    struct node_base_t;

    struct pass_t
    {
        enum type_t
        {
            LEAD = 0,
            TAIL
        };

        enum direction_t
        {
            NEXT = 0,
            PRED
        };

        pass_t(node_base_t* node, type_t type) :
            node_(node), type_(type) {}

        pass_t* get(direction_t dir)
        {
            if (dir == direction_t::NEXT)
            {
                return next_;
            }
            else
            {
                return pred_;
            }
        }

        node_base_t* node_;
        type_t type_;

        pass_t* next_;
        pass_t* pred_;
    };

    struct node_base_t
    {
        node_base_t() :
            lead_(this, pass_t::type_t::LEAD),
            tail_(this, pass_t::type_t::TAIL) {}

        pass_t& get(pass_t::type_t type)
        {
            if (type == pass_t::type_t::LEAD)
            {
                return lead_;
            }
            else
            {
                return tail_;
            }
        }

        pass_t lead_;
        pass_t tail_;
    };

    template<typename T>
    struct node_t : public node_base_t
    {
        node_t(const T& data) :
            node_base_t(), data_(data) {}

        node_t(T&& data) :
            node_base_t(), data_(std::move(data)) {}

        T data_;
    };

    struct header_t : public node_base_t
    {
        header_t() :
            node_base_t() {}
    };

    pass_t::type_t opposite_pass_type(pass_t::type_t type)
    {
        switch (type)
        {
            case pass_t::type_t::LEAD:
                return pass_t::type_t::TAIL;

            case pass_t::type_t::TAIL:
                return pass_t::type_t::LEAD;

            default:
                assert(0 && "wrong argument");
        }
    }
}

template<typename T>
struct tree_iterator
{
    using node_t = detail::node_t<T>;
    using node_base_t = detail::node_base_t;
    using pass_t = detail::pass_t;
    using direction_t = detail::pass_t::direction_t;

    // wanted smth like this and use in place of pass_type_t
    //enum traversal_t
    //{
    //    PRE_ORDER = detail::pass_type_t::LEADING,
    //    POST_ORDER = detail::pass_type_t::TRAILING
    //};

    using traversal_t = detail::pass_t::type_t;

    tree_iterator(node_base_t* node, traversal_t traversal) :
        node_(node), traversal_(traversal) {}

    T& operator*()
    {
        return static_cast<node_t*>(node_)->data_;
    }

    T* operator->()
    {
        return &static_cast<node_t*>(node_)->data_;
    }

    tree_iterator& operator++()
    {
        traverse(direction_t::NEXT);
        return *this;
    }

    tree_iterator& operator--()
    {
        traverse(direction_t::PRED);
        return *this;
    }

    friend bool operator==(const tree_iterator& lhs, const tree_iterator& rhs)
    {
        return lhs.node_ == rhs.node_ &&
               lhs.traversal_ == rhs.traversal_;
    }

    friend bool operator!=(tree_iterator& lhs, tree_iterator& rhs)
    {
        return !(lhs == rhs);
    }

    node_base_t* node_;
    traversal_t traversal_;

    private:
        void traverse(direction_t dir)
        {
            pass_t::type_t consider_type = traversal_;
            pass_t::type_t opposite_type = detail::opposite_pass_type(consider_type);

            pass_t* cur_pass = nullptr;
            // passing edges leading to opposite pass
            for (cur_pass = node_->get(consider_type).get(dir);
                 cur_pass->type_ == opposite_type;
                 cur_pass = cur_pass->get(dir));
            
            assert(cur_pass->type_ == consider_type);
            node_ = cur_pass->node_;
        }
};

template<typename T>
struct PostOrder
{
    using iterator = tree_iterator<T>;
    using header_t = detail::header_t;

    PostOrder(header_t& header) :
        header_(header) {}

    iterator end()
    {
        return iterator(&header_, iterator::traversal_t::TAIL);
    }

    iterator begin()
    {
        // genial, isn't it
        return ++end();
    }

    private:
        header_t& header_;
};

template<typename T, typename Alloc = std::allocator<T>>
struct tree
{
    using iterator = tree_iterator<T>;

    tree() : header_(), size_(0)
    {
        make_header(&header_);
        make_leaf(&header_);
    }

    // TODO: implement
    tree(const tree&) = delete;
    tree(tree&&) = delete;
    tree& operator=(const tree&) = delete;
    tree& operator=(tree&&) = delete;

    iterator end()
    {
        return iterator(&header_, iterator::traversal_t::LEAD);
    }

    iterator begin()
    {
        // genial, isn't it
        return ++end();
    }

    PostOrder<T> GetPostOrder()
    {
        return PostOrder<T>(header_);
    }

    iterator insert(iterator pos, const T& value)
    {
        // new node iserts before tail
        pass_t& next_pass = pos.node_->tail_;
        // new node's pred
        pass_t& pred_pass = *next_pass.pred_;

        node_base_t* new_node = create_node(value);
        
        // binding
        make_leaf(new_node);
        new_node->tail_.next_ = &next_pass;
        new_node->lead_.pred_ = &pred_pass;
        next_pass.pred_ = &new_node->tail_;
        pred_pass.next_ = &new_node->lead_;

        return iterator(new_node, pass_t::type_t::LEAD);
    }
    
    size_t size() const
    {
        return size_;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    void clear()
    {
        // TODO: just use post order traversal (faster, though UB is next door)
        while(!empty())
        {
            auto leaf = static_cast<node_t*>(GetPostOrder().begin().node_);
            delete_leaf(leaf);
        }
    }

    ~tree()
    {
        clear();
    }

    private:
        using pass_t = detail::pass_t;
        using node_base_t = detail::node_base_t;
        using node_t = detail::node_t<T>;

        // allocator breed
        using alloc_t = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
        using alloc_traits_t = std::allocator_traits<alloc_t>;
        using node_alloc_t = typename alloc_traits_t::template rebind_alloc<node_t>;
        using node_alloc_traits_t = std::allocator_traits<node_alloc_t>;

        // binds next_[pass_type_t::LEADING]
        // and pred_[pass_type_t::TRAILING]
        void make_leaf(node_base_t* node)
        {
            node->lead_.next_ = &node->tail_;
            node->tail_.pred_ = &node->lead_;
        }

        // binds next_[pass_type_t::LEADING]
        // and pred_[pass_type_t::TRAILING]
        void make_header(node_base_t* node)
        {
            node->tail_.next_ = &node->lead_;
            node->lead_.pred_ = &node->tail_;
        }

        node_t* create_node(const T& value)
        {
            node_t* new_node = nullptr;
            new_node = node_alloc.allocate(1);
            node_alloc.construct(new_node, value);
            ++size_;
            return new_node;
        }

        void delete_node(node_t* node)
        {
            node_alloc.destroy(node);
            node_alloc.deallocate(node, 1);
            --size_;
        }

        void delete_leaf(node_t* leaf)
        {
            assert(leaf->lead_.next_->node_ == leaf &&
                leaf->tail_.pred_->node_ == leaf &&
                "wrong argument: must be a leaf");
            pass_t& pred_pass = *leaf->lead_.pred_;
            pass_t& next_pass = *leaf->tail_.next_;

            // binding
            pred_pass.next_ = &next_pass;
            next_pass.pred_ = &pred_pass;

            delete_node(leaf);
        }

        detail::header_t header_;
        size_t size_;
        node_alloc_t node_alloc;
};

} //tree
#endif //TREE_LIB
