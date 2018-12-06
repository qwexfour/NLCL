#include <iostream>
#include <algorithm>
#include <cassert>

#include "tree.hpp"

auto main() -> int
{
    tree::tree<int> empty_one;
    std::cout << "Can I do at least empty tree:" << std::endl;
    std::cout << "Tree has " << empty_one.size() << " nodes." << std::endl;
    Dump(empty_one);

    tree::tree<long> first_one;
    first_one.insert(first_one.end(), 1);
    first_one.insert(first_one.end(), 2);
    std::cout << "We've got insert:" << std::endl;
    std::cout << "Tree has " << first_one.size() << " nodes." << std::endl;
    Dump(first_one);
   
    auto move_tree = std::move(first_one);
    std::cout << "Moved the one above" << std::endl;
    //Dump(move_tree);

    /*
     *      forest
     *      /    \
     *    [1]    [6]
     *    / \
     *  [2] [3]
     *      / \
     *    [4] [5]
     */
    std::cout << "OK, we can do smth more tree (or forest):" << std::endl;
    std::cout << "      forest" << std::endl;
    std::cout << "      /    \\" << std::endl;
    std::cout << "    [1]    [6]" << std::endl;
    std::cout << "    / \\" << std::endl;
    std::cout << "  [2] [3]" << std::endl;
    std::cout << "      / \\" << std::endl;
    std::cout << "    [4] [5]" << std::endl;
    
    tree::tree<unsigned> second_one;
    
    // filling
    auto left_subtree = second_one.insert(second_one.end(), 1);
    second_one.insert(second_one.end(), 6);
    second_one.insert(left_subtree, 2);
    
    auto three = second_one.insert(left_subtree, 3);
    assert(second_one.is_leaf(second_one.insert(three, 4)));
    assert(second_one.is_leaf(second_one.insert(three, 5)));
    
    std::cout << "Tree has " << second_one.size() << " nodes." << std::endl;
    Dump(second_one);

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


    return 0;
}
