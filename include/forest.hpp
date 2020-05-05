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

    if (any_forest.empty())
    {
        std::cout << "Forest is empty" << std::endl;
    }
    else
    {
        std::cout << "Forest has " << any_forest.size() << " nodes." << std::endl;
        std::cout << "In Order:" << std::endl;
        mini_dump(any_forest.begin(), any_forest.end());
        std::cout << "Post Order:" << std::endl;
        mini_dump(any_forest.get_post_order().begin(), any_forest.get_post_order().end());
    }
}

namespace forestlib
{

namespace detail
{
    struct pass_base_t
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

        pass_base_t(type_t type) noexcept :
            type_(type), next_(nullptr), pred_(nullptr) {}

        pass_base_t(const pass_base_t&) = delete;
        pass_base_t(pass_base_t&&) = delete;
        pass_base_t& operator=(const pass_base_t&) = delete;
        pass_base_t& operator=(pass_base_t&&) = delete;

        // for convenience sake
        pass_base_t* get(direction_t dir) const noexcept
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

        type_t type_;
        pass_base_t* next_;
        pass_base_t* pred_;
    };

    template<pass_base_t::type_t Type>
    struct pass_t : public pass_base_t
    {
        pass_t() : pass_base_t(Type) {}
    };

    struct node_base_t : public pass_t<pass_base_t::type_t::LEAD>,
                         public pass_t<pass_base_t::type_t::TAIL>
    {
        using level_t = unsigned;

        node_base_t(level_t level = 0) noexcept :
            pass_t<pass_base_t::type_t::LEAD>(),
            pass_t<pass_base_t::type_t::TAIL>(),
            level_(level) {}

        node_base_t(const node_base_t& rhs) = delete;
        node_base_t(node_base_t&& rhs) = delete;
        node_base_t& operator=(const node_base_t& rhs) = delete;
        node_base_t& operator=(node_base_t&& rhs) = delete;

        pass_base_t& get_lead_pass() noexcept
        {
            return static_cast<pass_base_t&>(
                static_cast<pass_t<pass_base_t::type_t::LEAD>&>(*this));
        }

        const pass_base_t& get_lead_pass() const noexcept
        {
            return const_cast<node_base_t*>(this)->get_lead_pass();
        }

        const pass_base_t& get_tail_pass() const noexcept
        {
            return const_cast<node_base_t*>(this)->get_tail_pass();
        }

        pass_base_t& get_tail_pass() noexcept
        {
            return static_cast<pass_base_t&>(
                static_cast<pass_t<pass_base_t::type_t::TAIL>&>(*this));
        }

        pass_base_t& get_pass(pass_base_t::type_t type) noexcept
        {
            if (type == pass_base_t::type_t::LEAD)
            {
                return get_lead_pass();
            }
            else
            {
                assert(type == pass_base_t::type_t::TAIL);
                return get_tail_pass();
            }
        }

        const pass_base_t& get_pass(pass_base_t::type_t type) const noexcept
        {
            return const_cast<node_base_t*>(this)->get_pass(type);
        }

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

    pass_base_t::type_t opposite_pass_type(pass_base_t::type_t type) noexcept;

    node_base_t* traverse(node_base_t* node,
                          pass_base_t::type_t traversal,
                          pass_base_t::direction_t direction) noexcept;

    const node_base_t* traverse(const node_base_t* node,
                                pass_base_t::type_t traversal,
                                pass_base_t::direction_t direction) noexcept;

    node_base_t* get_node(pass_base_t* pass) noexcept;
    const node_base_t* get_node(const pass_base_t* pass) noexcept;
}

template<typename T>
struct forest_iterator
{
    using difference_type = ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    using node_t = detail::node_t<T>;
    using node_base_t = detail::node_base_t;
    using pass_base_t = detail::pass_base_t;
    using direction_t = detail::pass_base_t::direction_t;

    // wanted smth like this and use in place of pass_type_t
    //enum traversal_t
    //{
    //    PRE_ORDER = detail::pass_type_t::LEADING,
    //    POST_ORDER = detail::pass_type_t::TRAILING
    //};

    using traversal_t = detail::pass_base_t::type_t;

    forest_iterator(node_base_t* node, traversal_t traversal) noexcept :
        node_(node), traversal_(traversal) {}

    reference operator*() const noexcept
    {
        return static_cast<node_t*>(node_)->data_;
    }

    pointer operator->() const noexcept
    {
        return &static_cast<node_t*>(node_)->data_;
    }

    forest_iterator& operator++() noexcept
    {
        node_ = detail::traverse(node_, traversal_, direction_t::NEXT);
        return *this;
    }

    forest_iterator operator++(int) noexcept
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    forest_iterator& operator--() noexcept
    {
        node_ = detail::traverse(node_, traversal_, direction_t::PRED);
        return *this;
    }

    forest_iterator operator--(int) noexcept
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    friend bool operator==(const forest_iterator& lhs, const forest_iterator& rhs) noexcept
    {
        return lhs.node_ == rhs.node_ &&
               lhs.traversal_ == rhs.traversal_;
    }

    friend bool operator!=(const forest_iterator& lhs, const forest_iterator& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    node_base_t* node_;
    traversal_t traversal_;
};

template<typename T>
struct const_forest_iterator
{
    using difference_type = ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;

    using node_t = detail::node_t<T>;
    using node_base_t = detail::node_base_t;
    using pass_base_t = detail::pass_base_t;
    using direction_t = detail::pass_base_t::direction_t;

    using traversal_t = detail::pass_base_t::type_t;

    const_forest_iterator(const node_base_t* node, traversal_t traversal) noexcept :
        node_(node), traversal_(traversal) {}

    reference operator*() const noexcept
    {
        return static_cast<const node_t*>(node_)->data_;
    }

    pointer operator->() const noexcept
    {
        return &static_cast<const node_t*>(node_)->data_;
    }

    const_forest_iterator& operator++() noexcept
    {
        node_ = traverse(node_, traversal_, direction_t::NEXT);
        return *this;
    }

    const_forest_iterator operator++(int) noexcept
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    const_forest_iterator& operator--() noexcept
    {
        node_ = traverse(node_, traversal_, direction_t::PRED);
        return *this;
    }

    const_forest_iterator operator--(int) noexcept
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    friend bool operator==(const const_forest_iterator& lhs, const const_forest_iterator& rhs) noexcept
    {
        return lhs.node_ == rhs.node_ &&
               lhs.traversal_ == rhs.traversal_;
    }

    friend bool operator!=(const const_forest_iterator& lhs, const const_forest_iterator& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    const node_base_t* node_;
    traversal_t traversal_;
};

template<typename T>
struct post_order
{
    using iterator = forest_iterator<T>;
    using header_t = detail::header_t;

    post_order(header_t* header) noexcept :
        header_(header) {}

    iterator end() noexcept
    {
        return iterator(header_, iterator::traversal_t::TAIL);
    }

    iterator begin() noexcept
    {
        // genial, isn't it
        return ++end();
    }

    private:
        header_t* header_;
};

template<typename T>
struct forest
{
    using iterator = forest_iterator<T>;
    using const_iterator = const_forest_iterator<T>;
    using level_t = detail::node_base_t::level_t;

    forest() : header_(new header_t), size_(0)
    {
        make_header(header_);
        make_leaf(header_);
    }

    forest(const forest& rhs) : forest()
    {
        // for exeption safety
        forest tmp;

        // parent of insertable node
        // starting from header_ (insert(end(), first))
        node_base_t* cur_parent = tmp.header_;
        // previously inserted node
        node_base_t* last_inserted = nullptr;

        auto tmp_insert_helper = [&tmp] (auto it, node_base_t* cur_parent) mutable
            {
                auto pos = iterator(cur_parent, iterator::traversal_t::LEAD);
                auto new_pos = tmp.insert(pos, *it);
                return new_pos.node_;
            };

        // copping
        // exception safety is based on the fact, that
        // state chahges only only by insert (this function is exception safe)
        // after each insert *this is a valid forest (equal to rhs's subforest)
        for (auto it = rhs.begin(); it != rhs.end(); ++it)
        {
            auto cur_level = rhs.get_level(it);
            auto inc_parent_level = cur_parent->level_ + 1;
            // parent is the same
            if (cur_level == inc_parent_level)
            {
                last_inserted = tmp_insert_helper(it, cur_parent);
            }
            else
            {
                // going deeper
                if (cur_level > inc_parent_level)
                {
                    assert(cur_level - inc_parent_level == 1 &&
                           "can go deeper only by one level");
                    cur_parent = last_inserted;
                    last_inserted = tmp_insert_helper(it, cur_parent);
                }
                else
                {
                    // rolling back towards header
                    assert(cur_level < inc_parent_level);
                    // how much steps towards header we have to make
                    auto steps = inc_parent_level - cur_level;
                    assert(steps > 0 && "must be at least 1");
                    // stepping back
                    for (decltype(steps) i = 0; i < steps; ++i)
                    {
                        cur_parent = detail::get_node(cur_parent->get_tail_pass().next_);
                    }
                    last_inserted = tmp_insert_helper(it, cur_parent);
                }
            }
        }

        swap(tmp, *this);
    }

    forest(forest&& rhs) noexcept : header_(rhs.header_), size_(rhs.size_)
    {
        rhs.header_ = nullptr;
        rhs.size_ = 0;
    }

    forest& operator=(const forest& rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }
        auto tmp = rhs;
        swap(tmp, *this);
        return *this;
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

    iterator end() noexcept
    {
        return iterator(header_, iterator::traversal_t::LEAD);
    }

    iterator begin() noexcept
    {
        // genial, isn't it
        return ++end();
    }

    const_iterator end() const noexcept
    {
        return const_iterator(header_, iterator::traversal_t::LEAD);
    }

    const_iterator begin() const noexcept
    {
        // genial, isn't it
        return ++end();
    }

    // add for const forest
    post_order<T> get_post_order() noexcept
    {
        return post_order<T>(header_);
    }

    template<typename Iter>
    level_t get_level(const Iter pos) const noexcept
    {
        return pos.node_->level_;
    }

    template<typename Iter>
    bool is_leaf(const Iter pos) const noexcept
    {
        return is_leaf(pos.node_);
    }

    iterator insert(iterator pos, const T& value)
    {
        // new node iserts before tail
        auto& next_pass = pos.node_->get_tail_pass();
        // new node's pred
        auto& pred_pass = *next_pass.pred_;
        // new node's level
        auto level = get_level(pos) + 1;

        auto new_node = construct_node(value, level);

        // Kalbs line
        //---------------------------------------------------

        // binding
        make_leaf(new_node);
        new_node->get_tail_pass().next_ = &next_pass;
        new_node->get_lead_pass().pred_ = &pred_pass;
        next_pass.pred_ = &new_node->get_tail_pass();
        pred_pass.next_ = &new_node->get_lead_pass();

        return iterator(new_node, pos.traversal_);
    }

    iterator erase(iterator pos) noexcept
    {
        auto tmp = pos;
        wise_delete_node(pos.node_);
        return ++tmp;
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
        while(!empty())
        {
            auto node = begin().node_;
            dumb_delete_node(node);
        }
    }

    friend void swap(forest& lhs, forest& rhs) noexcept
    {
        std::swap(lhs.size_, rhs.size_);
        std::swap(lhs.header_, rhs.header_);
    }

    private:
        using pass_base_t = detail::pass_base_t;
        using node_base_t = detail::node_base_t;
        using node_t = detail::node_t<T>;
        using header_t = detail::header_t;

        // binds next_[pass_type_t::LEADING]
        // and pred_[pass_type_t::TRAILING]
        static void make_leaf(node_base_t* node) noexcept
        {
            node->get_lead_pass().next_ = &node->get_tail_pass();
            node->get_tail_pass().pred_ = &node->get_lead_pass();
        }

        // binds next_[pass_type_t::LEADING]
        // and pred_[pass_type_t::TRAILING]
        static void make_header(node_base_t* node) noexcept
        {
            node->get_tail_pass().next_ = &node->get_lead_pass();
            node->get_lead_pass().pred_ = &node->get_tail_pass();
        }

        bool is_leaf(node_base_t* node) const noexcept
        {
            bool res = node->get_lead_pass().next_ == &node->get_tail_pass();
            // check if node is consistent
            assert(res == (node->get_tail_pass().pred_ == &node->get_lead_pass()));
            return res;
        }

        node_t* construct_node(const T& value, level_t level)
        {
            auto new_node = new node_t (value, level);
            ++size_;
            return new_node;
        }

        void destruct_node(node_t* node) noexcept
        {
            delete node;
            --size_;
        }

        void delete_leaf(node_base_t* leaf) noexcept
        {
            assert(detail::get_node(leaf->get_lead_pass().next_) == leaf &&
                detail::get_node(leaf->get_tail_pass().pred_) == leaf &&
                "wrong argument: must be a leaf");
            pass_base_t& pred_pass = *leaf->get_lead_pass().pred_;
            pass_base_t& next_pass = *leaf->get_tail_pass().next_;

            // binding
            pred_pass.next_ = &next_pass;
            next_pass.pred_ = &pred_pass;

            destruct_node(static_cast<node_t*>(leaf));
        }

        // corrects nothing after deletion (level etc.)
        // except size
        void dumb_delete_internal(node_base_t* node) noexcept
        {
            assert(detail::get_node(node->get_lead_pass().next_) != node &&
                detail::get_node(node->get_tail_pass().pred_) != node &&
                "wrong argument: must be an internal node");

            // rebinding
            // [pred_pass]--X-->[node->pass]--X-->[next_pass]
            //           \                          ^
            //            \_________________________|

            // lead pass
            (node->get_lead_pass().pred_)->next_ = node->get_lead_pass().next_;
            (node->get_lead_pass().next_)->pred_ = node->get_lead_pass().pred_;
            // tail pass
            (node->get_tail_pass().pred_)->next_ = node->get_tail_pass().next_;
            (node->get_tail_pass().next_)->pred_ = node->get_tail_pass().pred_;

            destruct_node(static_cast<node_t*>(node));
        }

        // at least meant to be so
        void wise_delete_internal(node_base_t* node) noexcept
        {
            // traversing subtree of node to delete
            for (auto it = ++iterator(node, iterator::traversal_t::LEAD);
                 get_level(it) > node->level_;
                 ++it)
            {
                --(it.node_->level_);
            }
            // now can delete this sucker
            dumb_delete_internal(node);
        }

        // corrects nothing after deletion (level etc.)
        // except size
        void dumb_delete_node(node_base_t* node) noexcept
        {
            if (is_leaf(node))
            {
                delete_leaf(node);
            }
            else
            {
                dumb_delete_internal(node);
            }
        }

        void wise_delete_node(node_base_t* node) noexcept
        {
            if (is_leaf(node))
            {
                delete_leaf(node);
            }
            else
            {
                wise_delete_internal(node);
            }
        }

        header_t* header_;
        size_t size_;
};

template<typename T>
bool operator==(const forest<T>& lhs, const forest<T>& rhs) noexcept
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }

    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T>
bool operator!=(const forest<T>& lhs, const forest<T>& rhs) noexcept
{
    return !(lhs == rhs);
}

} //forest
#endif //TREE_LIB
