#ifndef TREE_LIB
#define TREE_LIB

#include <cassert>
#include <algorithm>
#include <deque>
#include <iostream>
#include <variant>
#include <memory>

namespace tree
{

namespace detail
{

    struct node_base_t;

    struct edge_t
    {
        enum type_t
        {
            LEADING,    // edge points into lead_pass (for nexts)
            TRAILING,   // edge points int trail_pass (for nexts)
                        // for preds same as in corresponding nexts
            INVALID
        };

        edge_t() :
            succ_(nullptr), type_(type_t::INVALID) {}

        edge_t(node_base_t* succ, type_t type) :
            succ_(succ), type_(type) {}
        
        friend bool operator==(edge_t& lhs, edge_t& rhs)
        {
            return lhs.succ_ == rhs.succ_ &&
                   lhs.type_ == rhs.type_;
        }
        
        friend bool operator!=(edge_t& lhs, edge_t& rhs)
        {
            return !(lhs == rhs);
        }

        node_base_t* succ_;
        type_t type_;
    };

    const edge_t null_edge(nullptr, edge_t::type_t::INVALID);

    struct node_base_t
    {
        node_base_t() :
            lead_pass_next_(), lead_pass_pred_(),
            trail_pass_next_(), trail_pass_pred_() {}

        edge_t lead_pass_next_;
        edge_t lead_pass_pred_;
        edge_t trail_pass_next_;
        edge_t trail_pass_pred_;
    };

    template<typename T>
    struct node_t : public node_base_t
    {
        node_t(T data) :
            data_(data), node_base_t() {}

        T data_;
    };

    struct header_t : public node_base_t
    {
        header_t() :
            node_base_t() {}
    };

}

template<typename T>
struct tree_iterator
{
    using node_t = detail::node_t<T>;
    using node_base_t = detail::node_base_t;
    using edge_t = detail::edge_t;

    tree_iterator(node_base_t* node) :
        node_(node) {}

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
        edge_t cur_edge;
        node_base_t* cur_node;
        // passing trailing edges
        for (cur_edge = node_->lead_pass_next_,
             cur_node = node_;
             cur_edge.type_ == edge_t::type_t::TRAILING;
             cur_node = cur_edge.succ_,
             cur_edge = cur_edge.succ_->trail_pass_next_);
        
        if (cur_edge.type_ == edge_t::type_t::LEADING)
        {
            node_ = cur_edge.succ_;
        }
        else
        {
            // reached the end
            assert(cur_edge.type_ == edge_t::type_t::INVALID);
            assert(!cur_edge.succ_ && "must be nullptr for the end");
            node_ = cur_node;
        }
        return *this;
    }

    tree_iterator& operator--()
    {
        node_base_t* cur_node = node_;
        // stepping 1 leading edge back
        // unless we're in the end
        if (node_->lead_pass_pred_.succ_)
        {
            assert(node_->lead_pass_pred_.type_ == edge_t::type_t::INVALID &&
                "must be invalid for the end");
            cur_node = node_->lead_pass_pred_.succ_;
        }

        edge_t cur_edge;
        // passing trailing edges back
        // works ok with curr_node == header
        for (cur_edge = cur_node->trail_pass_pred_;
             cur_edge.type_ == edge_t::type_t::TRAILING;
             cur_node = cur_edge.succ_,
             cur_edge = cur_edge.succ_->trail_pass_pred_);
        
        // if we reached header
        if (!cur_edge.succ_)
        {
            assert(cur_edge.type_ == edge_t::type_t::INVALID &&
                "must be invalid for the header");
            assert(0 && "--begin() - bad");
        }

        node_ = cur_node;
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

    private:
        node_base_t* node_;
};

template<typename T, typename Alloc = std::allocator<T>>
struct tree
{
    using iterator = tree_iterator<T>;

    tree() : header_()
    {
        make_leaf(&header_);
    }

    iterator begin()
    {
        return iterator(header_.lead_pass_next_.succ_);
    }

    iterator end()
    {
        return iterator(&header_);
    }

    private:
        using node_base_t = detail::node_base_t;
        using edge_t = detail::edge_t;
        using node_t = detail::node_t<T>;

        // allocator breed
        using alloc = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
        using alloc_traits = std::allocator_traits<alloc>;
        using node_alloc = typename alloc_traits::template rebind_alloc<node_t>;
        using node_alloc_traits = std::allocator_traits<node_alloc>;

        // binds lead_pass_next
        // and trail_pass_pred
        void make_leaf(node_base_t* node)
        {
            node->lead_pass_next_.succ_ = node->trail_pass_pred_.succ_ = node;
            node->lead_pass_next_.type_ = node->trail_pass_pred_.type_ = edge_t::type_t::TRAILING;
        }

        detail::header_t header_;
};

} //tree
#endif //TREE_LIB
