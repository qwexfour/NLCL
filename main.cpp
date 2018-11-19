#include <iostream>
#include <algorithm>

#include "tree.hpp"

template<typename Iter>
void Dump(Iter first, Iter last)
{
    std::for_each(first, last, [] (const auto& elem) {std::cout << elem << " ";});
    std::cout << std::endl;
}

auto main() -> int
{
    tree::tree<int> empty_one;
    std::cout << "Empty tree:" << std::endl;
    Dump(empty_one.begin(), empty_one.end());

    tree::tree<long> first_one;
    first_one.insert(first_one.end(), 1);
    first_one.insert(first_one.end(), 2);
    std::cout << "First tree:" << std::endl;
    Dump(first_one.begin(), first_one.end());
    
    tree::tree<unsigned> second_one;
    auto left_subtree = second_one.insert(second_one.end(), 1);
    second_one.insert(second_one.end(), 4);
    second_one.insert(left_subtree, 2);
    second_one.insert(left_subtree, 3);
    std::cout << "First tree:" << std::endl;
    Dump(second_one.begin(), second_one.end());
    return 0;
}
