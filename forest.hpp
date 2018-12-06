#ifndef TREE_LIB
#define TREE_LIB

#include <cassert>
#include <algorithm>
#include <utility>
#include <deque>
#include <iostream>
#include <variant>
#include <memory>

template<typename T>
void Dump(T&& any_forest)
{
    auto mini_dump = [&any_forest] (auto first, auto last)
    {
        for(auto it = first; it != last; ++it)
        {
            std::cout << *it << "(l:" << any_forest.get_level(it) << ") ";
        }
        std::cout << std::endl;
    };

    std::cout << "In Oreder:" << std::endl;
    mini_dump(any_forest.begin(), any_forest.end());
    std::cout << "Post Oreder:" << std::endl;
    mini_dump(any_forest.get_post_order().begin(), any_forest.get_post_order().end());
}

namespace forestlib
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
            node_(node), type_(type), next_(nullptr), pred_(nullptr) {}

        pass_t(const pass_t&) = delete;

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
        using level_t = unsigned;
        node_base_t(level_t level = 0) :
            lead_(this, pass_t::type_t::LEAD),
            tail_(this, pass_t::type_t::TAIL),
            level_(level) {}

        node_base_t(const node_base_t& rhs) = delete;
        node_base_t& operator=(const node_base_t& rhs) = delete;

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
        level_t level_;
    };

    template<typename T>
    struct node_t : public node_base_t
    {
        using level_t = node_base_t::level_t;

        node_t(const T& data, level_t level = 0) :
            node_base_t(level), data_(data) {}

        node_t(T&& data, level_t level = 0) :
            node_base_t(level), data_(std::move(data)) {}

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
struct forest_iterator
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

    forest_iterator(node_base_t* node, traversal_t traversal) :
        node_(node), traversal_(traversal) {}

    T& operator*()
    {
        return static_cast<node_t*>(node_)->data_;
    }

    T* operator->()
    {
        return &static_cast<node_t*>(node_)->data_;
    }

    forest_iterator& operator++()
    {
        traverse(direction_t::NEXT);
        return *this;
    }

    forest_iterator& operator--()
    {
        traverse(direction_t::PRED);
        return *this;
    }

    friend bool operator==(const forest_iterator& lhs, const forest_iterator& rhs)
    {
        return lhs.node_ == rhs.node_ &&
               lhs.traversal_ == rhs.traversal_;
    }

    friend bool operator!=(forest_iterator& lhs, forest_iterator& rhs)
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
struct post_order
{
    using iterator = forest_iterator<T>;
    using header_t = detail::header_t;

    post_order(header_t* header) :
        header_(header) {}

    iterator end()
    {
        return iterator(header_, iterator::traversal_t::TAIL);
    }

    iterator begin()
    {
        // genial, isn't it
        return ++end();
    }

    private:
        header_t* header_;
};

template<typename T, typename Alloc = std::allocator<T>>
struct forest
{
    using iterator = forest_iterator<T>;
    using level_t = detail::node_base_t::level_t;

    forest() : header_(new header_t), size_(0)
    {
        make_header(header_);
        make_leaf(header_);
    }

    // TODO: implement
    forest(const forest&) = delete;
    forest(forest&& rhs) noexcept : header_(rhs.header_), size_(rhs.size_)
    {
        rhs.header_ = nullptr;
        rhs.size_ = 0;
    }

    forest& operator=(forest&& rhs) noexcept
    {
        if (this == &rhs)
        {
            return *this;
        }
        auto tmp = std::move(rhs);
        swap(tmp, *this);
        return *this;
    }

    ~forest() noexcept
    {
        clear();
        delete header_;
    }

    forest& operator=(const forest&) = delete;

    iterator end()
    {
        return iterator(header_, iterator::traversal_t::LEAD);
    }

    iterator begin()
    {
        // genial, isn't it
        return ++end();
    }

    // add for const forest
    post_order<T> get_post_order() noexcept
    {
        return post_order<T>(header_);
    }

    level_t get_level(iterator pos) const noexcept
    {
        return pos.node_->level_;
    }

    bool is_leaf(iterator pos) const noexcept
    {
        const node_base_t& node = *pos.node_;
        bool res = node.lead_.next_ == &node.tail_;
        assert(res == (node.tail_.pred_ == &node.lead_));
        return res;
    }

    iterator insert(iterator pos, const T& value)
    {
        // new node iserts before tail
        pass_t& next_pass = pos.node_->tail_;
        // new node's pred
        pass_t& pred_pass = *next_pass.pred_;
        // new node's level
        level_t level = get_level(pos) + 1;

        node_base_t* new_node = create_node(value, level);
        
        // binding
        make_leaf(new_node);
        new_node->tail_.next_ = &next_pass;
        new_node->lead_.pred_ = &pred_pass;
        next_pass.pred_ = &new_node->tail_;
        pred_pass.next_ = &new_node->lead_;

        return iterator(new_node, pass_t::type_t::LEAD);
    }
    
    size_t size() const noexcept
    {
        return size_;
    }

    bool empty() const noexcept
    {
        return size_ == 0;
    }

    void clear() noexcept
    {
        // TODO: just use post order traversal (faster, though UB is next door)
        while(!empty())
        {
            auto leaf = static_cast<node_t*>(get_post_order().begin().node_);
            delete_leaf(leaf);
        }
    }

    friend void swap(forest& lhs, forest& rhs) noexcept
    {
        std::swap(lhs.size_, rhs.size_);
        std::swap(lhs.header_, rhs.header_);
    }

    private:
        using pass_t = detail::pass_t;
        using node_base_t = detail::node_base_t;
        using node_t = detail::node_t<T>;
        using header_t = detail::header_t;

        // allocator breed
        using alloc_t = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
        using alloc_traits_t = std::allocator_traits<alloc_t>;
        using node_alloc_t = typename alloc_traits_t::template rebind_alloc<node_t>;
        using node_alloc_traits_t = std::allocator_traits<node_alloc_t>;

        // binds next_[pass_type_t::LEADING]
        // and pred_[pass_type_t::TRAILING]
        static void make_leaf(node_base_t* node) noexcept
        {
            node->lead_.next_ = &node->tail_;
            node->tail_.pred_ = &node->lead_;
        }

        // binds next_[pass_type_t::LEADING]
        // and pred_[pass_type_t::TRAILING]
        static void make_header(node_base_t* node) noexcept
        {
            node->tail_.next_ = &node->lead_;
            node->lead_.pred_ = &node->tail_;
        }

        node_t* create_node(const T& value, level_t level)
        {
            auto new_node = new node_t (value, level);
            ++size_;
            return new_node;
        }

        void delete_node(node_t* node) noexcept
        {
            delete node;
            --size_;
        }

        void delete_leaf(node_t* leaf) noexcept
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

        header_t* header_;
        size_t size_;
};

} //forest
#endif //TREE_LIB
