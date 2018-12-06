#include <iostream>
#include <algorithm>
#include <cassert>

#include "forest.hpp"

auto main() -> int
{
    forestlib::forest<int> empty_one;
    std::cout << "Can I do at least empty forest:" << std::endl;
    std::cout << "Tree has " << empty_one.size() << " nodes." << std::endl;
    Dump(empty_one);
    std::cout << std::endl;

    forestlib::forest<long> first_one;
    first_one.insert(first_one.end(), 1);
    first_one.insert(first_one.end(), 2);
    std::cout << "We've got insert:" << std::endl;
    std::cout << "Tree has " << first_one.size() << " nodes." << std::endl;
    Dump(first_one);
    std::cout << std::endl;
   
    auto move_forest = std::move(first_one);
    std::cout << "Moved the one above" << std::endl;
    Dump(move_forest);
    std::cout << std::endl;

    /*
     *      forest
     *      /    \
     *    [1]    [6]
     *    / \
     *  [2] [3]
     *      / \
     *    [4] [5]
     */
    std::cout << "OK, we can do smth more forest (or forest):" << std::endl;
    std::cout << "      forest" << std::endl;
    std::cout << "      /    \\" << std::endl;
    std::cout << "    [1]    [6]" << std::endl;
    std::cout << "    / \\" << std::endl;
    std::cout << "  [2] [3]" << std::endl;
    std::cout << "      / \\" << std::endl;
    std::cout << "    [4] [5]" << std::endl;
    
    forestlib::forest<unsigned> second_one;
    
    // filling
    auto left_subforest = second_one.insert(second_one.end(), 1);
    second_one.insert(second_one.end(), 6);
    second_one.insert(left_subforest, 2);
    
    auto three = second_one.insert(left_subforest, 3);
    assert(second_one.is_leaf(second_one.insert(three, 4)));
    assert(second_one.is_leaf(second_one.insert(three, 5)));
    
    Dump(second_one);
    std::cout << std::endl;

    forestlib::forest copied_one = second_one;

    std::cout << "Does clear work?" << std::endl;
    second_one.clear();
    if (second_one.empty() && second_one.begin() == second_one.end())
    {
        std::cout << "Yeah, it works" << std::endl;
    }
    else
    {
        std::cout << "No, it sucks" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Is it ok after copying?" << std::endl;
    Dump(copied_one);

    return 0;
}
