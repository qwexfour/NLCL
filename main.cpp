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
    
    tree::Tree<double> oneTree(0.0);
    oneTree.AddSucc(1.0, oneTree.GetRoot());
    oneTree.AddSucc(2.0, oneTree.GetRoot());
    std::cout << "One tree:" << std::endl;
    Dump(oneTree.begin(), oneTree.end());
    return 0;
}
