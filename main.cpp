#include <iostream>
#include <algorithm>

#include "mugratree.hpp"

template<typename Iter>
void Dump(Iter first, Iter last)
{
    std::for_each(first, last, [] (const auto& elem) {std::cout << elem << " ";});
    std::cout << std::endl;
}

auto main() -> int
{
    tree::Tree<int> emptyTree;
    std::cout << "Empty tree:" << std::endl;
    Dump(emptyTree.begin(), emptyTree.end());
    tree::Tree<long> rootTree(42);
    std::cout << "Root tree:" << std::endl;
    Dump(rootTree.begin(), rootTree.end());
    return 0;
}
