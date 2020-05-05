#include "forest.hpp"

#include <cassert>

using namespace forestlib;
using namespace detail;

pass_base_t::type_t detail::opposite_pass_type(pass_base_t::type_t type) noexcept
{
    switch (type)
    {
        case pass_base_t::type_t::LEAD:
            return pass_base_t::type_t::TAIL;

        case pass_base_t::type_t::TAIL:
            return pass_base_t::type_t::LEAD;

        default:
            assert(0 && "wrong argument");
    }
}

node_base_t* detail::traverse(node_base_t* node,
                              pass_base_t::type_t traversal,
                              pass_base_t::direction_t dir) noexcept
{
    auto considered_type = traversal;
    auto opposite_type = opposite_pass_type(considered_type);

    // passing edges leading to opposite pass
    auto cur_pass = node->get_pass(considered_type).get(dir);
    for (;cur_pass->type_ == opposite_type;
         cur_pass = cur_pass->get(dir));

    assert(cur_pass->type_ == considered_type);
    return get_node(cur_pass);
}

const node_base_t* detail::traverse(const node_base_t* node,
                                    pass_base_t::type_t traversal,
                                    pass_base_t::direction_t dir) noexcept
{
    return traverse(const_cast<node_base_t*>(node), traversal, dir);
}

const node_base_t* detail::get_node(const pass_base_t* pass) noexcept
{
    if (pass->type_ == pass_base_t::type_t::LEAD)
    {
        return static_cast<const node_base_t*>(
            static_cast<const pass_t<pass_base_t::type_t::LEAD>*>(pass));
    }
    else
    {
        assert(pass->type_ == pass_base_t::type_t::TAIL);
        return static_cast<const node_base_t*>(
            static_cast<const pass_t<pass_base_t::type_t::TAIL>*>(pass));
    }
}

node_base_t* detail::get_node(pass_base_t* pass) noexcept
{
    return const_cast<node_base_t*>(get_node(static_cast<const pass_base_t*>(pass)));
}
