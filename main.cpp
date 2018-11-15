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
    return 0;
}
