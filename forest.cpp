#include "forest.hpp"

using namespace forestlib;

detail::pass_t::type_t detail::opposite_pass_type(detail::pass_t::type_t type) noexcept
{
    switch (type)
    {
        case detail::pass_t::type_t::LEAD:
            return detail::pass_t::type_t::TAIL;

        case detail::pass_t::type_t::TAIL:
            return detail::pass_t::type_t::LEAD;

        default:
            assert(0 && "wrong argument");
    }
}

detail::node_base_t* detail::traverse(detail::node_base_t* node,
                                      detail::pass_t::type_t traversal,
                                      detail::pass_t::direction_t dir) noexcept
{
    auto consider_type = traversal;
    auto opposite_type = detail::opposite_pass_type(consider_type);

    // passing edges leading to opposite pass
    auto cur_pass = node->get(consider_type).get(dir);
    for (;cur_pass->type_ == opposite_type;
         cur_pass = cur_pass->get(dir));
    
    assert(cur_pass->type_ == consider_type);
    return cur_pass->node_;
}

const detail::node_base_t* detail::traverse(const detail::node_base_t* node,
                                            detail::pass_t::type_t traversal,
                                            detail::pass_t::direction_t dir) noexcept
{
    // I know what I'm doing (just like in stl_tree from std::map)
    return detail::traverse(const_cast<detail::node_base_t*>(node),
                            traversal, dir);
}
