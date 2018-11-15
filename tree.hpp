#ifndef TREE_LIB
#define TREE_LIB

#include <cassert>
#include <algorithm>
#include <deque>
#include <iostream>
#include <variant>

namespace tree
{

namespace detail
{

    struct node_base_t;

    struct edge_t
    {
        enum type_t
        {
            LEADING,
            TRAILING,
            INVALID
        };

        edge_t(node_base_t* succ, type_t type) :
            succ_(succ), type_(type) {}

        node_base_t* succ_;
        type_t type_;
        
        friend bool operator==(edge_t& lhs, edge_t& rhs)
        {
            return lhs.succ_ == rhs.succ_ &&
                   lhs.type_ == rhs.type_;
        }
        
        friend bool operator!=(edge_t& lhs, edge_t& rhs)
        {
            return !(lhs == rhs);
        }
    };

    const edge_t null_edge(nullptr, edge_t::type_t::INVALID);

    struct node_base_t
    {
        node_base_t(edge_t next, edge_t pred) :
            next_(next), pred_(pred) {}

        edge_t next_;
        edge_t pred_;
    };

    template<typename T>
    struct node_t : public node_base_t
    {
        node_t(T data, edge_t next, edge_t pred) :
            data_(data), node_base_t(next, pred) {}

        T data_;
    };

    struct header_t : public node_base_t
    {
        header_t(edge_t first) :
            node_base_t(first, null_edge) {}
    };

}

template<typename T>
struct tree_iterator
{
    using node_t = detail::node_t<T>;

    tree_iterator(detail::edge_t edge) :
        edge_(edge) {}

    T& operator*()
    {
        detail::node_base_t* curr_node = edge_.succ_;
        return static_cast<node_t*>(curr_node)->data_;
    }

    T* operator->()
    {
        detail::node_base_t* curr_node = edge_.succ_;
        return &static_cast<node_t*>(curr_node)->data_;
    }

    tree_iterator& operator++()
    {
        // smth wise
        return *this;
    }

    friend bool operator==(tree_iterator& lhs, tree_iterator& rhs)
    {
        return lhs.edge_ == rhs.edge_;
    }

    friend bool operator!=(tree_iterator& lhs, tree_iterator& rhs)
    {
        return !(lhs == rhs);
    }

    private:
        // the edge with which we entered
        // current node (edge_.succ_)
        detail::edge_t edge_;
};

template<typename T, typename Alloc = std::allocator<T>>
struct tree
{
    using iterator = tree_iterator<T>;
    tree() :
        header_(detail::edge_t(&header_, detail::edge_t::type_t::TRAILING)) {}

    iterator begin()
    {
        return iterator(header_.next_);
    }

    iterator end()
    {
        return iterator(detail::edge_t(&header_, detail::edge_t::type_t::TRAILING));
    }

    private:
        detail::header_t header_;
};

} //tree
#endif //TREE_LIB
