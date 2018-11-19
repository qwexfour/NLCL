#ifndef TREE_LIB
#define TREE_LIB

#include <cassert>
#include <algorithm>
#include <deque>
#include <iostream>
#include <variant>
#include <memory>
#include <array>

namespace tree
{

namespace detail
{
    struct node_base_t;

    enum pass_t
    {
        LEADING = 0,
        TRAILING,
        SIZE_OF
    };

    struct edge_t
    {
        edge_t() :
            succ_(nullptr), pass_(pass_t::SIZE_OF) {}

        friend bool operator==(edge_t& lhs, edge_t& rhs)
        {
            return lhs.succ_ == rhs.succ_ &&
                   lhs.pass_ == rhs.pass_;
        }
        
        friend bool operator!=(edge_t& lhs, edge_t& rhs)
        {
            return !(lhs == rhs);
        }

        node_base_t* succ_;
        pass_t pass_;
    };

    struct node_base_t
    {
        using edge_seq_t = std::array<edge_t, pass_t::SIZE_OF>;

        enum direction_t
        {
            NEXT,
            PRED
        };

        edge_seq_t& next_pred(direction_t dir)
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

        edge_seq_t next_;
        edge_seq_t pred_;
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

    // TODO: smth better
    pass_t opposite_pass(pass_t type)
    {
        switch (type)
        {
            case pass_t::LEADING:
                return pass_t::TRAILING;

            case pass_t::TRAILING:
                return pass_t::LEADING;

            default:
                assert(0 && "wrong argument");
        }
    }
}

template<typename T>
struct tree_iterator
{
    using pass_t = detail::pass_t;
    using node_t = detail::node_t<T>;
    using node_base_t = detail::node_base_t;
    using edge_t = detail::edge_t;
    using direction_t = detail::node_base_t::direction_t;

    // wanted smth like this and use in place of pass_t
    //enum traversal_t
    //{
    //    PRE_ORDER = detail::pass_t::LEADING,
    //    POST_ORDER = detail::pass_t::TRAILING
    //};

    using traversal_t = detail::pass_t;

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
        inc_dec(direction_t::NEXT);
        return *this;
    }

    tree_iterator& operator--()
    {
        inc_dec(direction_t::PRED);
        return *this;
    }

    friend bool operator==(tree_iterator& lhs, tree_iterator& rhs)
    {
        return lhs.node_ == rhs.node_;
    }

    friend bool operator!=(tree_iterator& lhs, tree_iterator& rhs)
    {
        return !(lhs == rhs);
    }

    node_base_t* node_;
    traversal_t traversal_;

    private:
        void inc_dec(direction_t dir)
        {
            pass_t considered_pass = traversal_;
            pass_t opposite_pass = detail::opposite_pass(considered_pass);

            edge_t cur_edge;
            // passing edges leading to opposite pass
            for (cur_edge = node_->next_pred(dir)[considered_pass];
                 cur_edge.pass_ == opposite_pass;
                 cur_edge = cur_edge.succ_->next_pred(dir)[opposite_pass]);
            
            assert(cur_edge.pass_ == considered_pass);
            node_ = cur_edge.succ_;
        }
};

template<typename T, typename Alloc = std::allocator<T>>
struct tree
{
    using iterator = tree_iterator<T>;

    tree() : header_()
    {
        make_header(&header_);
        make_leaf(&header_);
    }

    iterator end()
    {
                                                       //PRE_ORDER
        return iterator(&header_, iterator::traversal_t::LEADING);
    }

    iterator begin()
    {
        // genial, isn't it
        return ++end();
    }

    iterator insert(iterator pos, const T& value)
    {
        // new node iserts before trailing pass
        node_base_t* next_node = pos.node_;
        pass_t next_pass = pass_t::TRAILING;

        // new node's pred
        edge_t pred_edge = next_node->pred_[next_pass];
        node_base_t* pred_node = pred_edge.succ_;
        pass_t pred_pass = pred_edge.pass_;

        node_base_t* new_node = create_node(value);
        
        // binding
        make_leaf(new_node);

        new_node->next_[pass_t::TRAILING].succ_ = next_node;
        new_node->next_[pass_t::TRAILING].pass_ = next_pass;
        next_node->pred_[next_pass].succ_ = new_node;
        next_node->pred_[next_pass].pass_ = pass_t::TRAILING;

        new_node->pred_[pass_t::LEADING].succ_ = pred_node;
        new_node->pred_[pass_t::LEADING].pass_ = pred_pass;
        pred_node->next_[pred_pass].succ_ = new_node;
        pred_node->next_[pred_pass].pass_ = pass_t::LEADING;

        return iterator(new_node, pass_t::LEADING);
    }

    private:
        using pass_t = detail::pass_t;
        using node_base_t = detail::node_base_t;
        using edge_t = detail::edge_t;
        using node_t = detail::node_t<T>;

        // allocator breed
        using alloc_t = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
        using alloc_traits_t = std::allocator_traits<alloc_t>;
        using node_alloc_t = typename alloc_traits_t::template rebind_alloc<node_t>;
        using node_alloc_traits_t = std::allocator_traits<node_alloc_t>;

        // binds next_[pass_t::LEADING]
        // and pred_[pass_t::TRAILING]
        void make_leaf(node_base_t* node)
        {
            node->next_[pass_t::LEADING].succ_ = node;
            node->pred_[pass_t::TRAILING].succ_ = node;
            node->next_[pass_t::LEADING].pass_ = pass_t::TRAILING;
            node->pred_[pass_t::TRAILING].pass_ = pass_t::LEADING;
        }

        // binds next_[pass_t::LEADING]
        // and pred_[pass_t::TRAILING]
        void make_header(node_base_t* node)
        {
            node->pred_[pass_t::LEADING].succ_ = node;
            node->next_[pass_t::TRAILING].succ_ = node;
            node->pred_[pass_t::LEADING].pass_ = pass_t::TRAILING;
            node->next_[pass_t::TRAILING].pass_ = pass_t::LEADING;
        }

        node_t* create_node(const T& value)
        {
            node_t* new_node = nullptr;
            new_node = node_alloc.allocate(1);
            node_alloc.construct(new_node, value);
            return new_node;
        }

        detail::header_t header_;
        node_alloc_t node_alloc;
};

} //tree
#endif //TREE_LIB
