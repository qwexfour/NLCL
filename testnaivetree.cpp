#include <iostream>
#include <algorithm>

#include "naivetree.hpp"

template<typename Iter>
void Dump(Iter first, Iter last)
{
    std::for_each(first, last, [] (const auto& elem) {std::cout << elem << " ";});
    std::cout << std::endl;
}

auto main() -> int
{
    naive_tree::Tree<int> emptyTree;
    emptyTree.SetRoot(42);
    std::cout << "Empty tree:" << std::endl;
    Dump(emptyTree.begin(), emptyTree.end());
    
    naive_tree::Tree<long> rootTree(42);
    rootTree.SetRoot(43);
    std::cout << "Root tree:" << std::endl;
    Dump(rootTree.begin(), rootTree.end());
    
    naive_tree::Tree<double> oneTree(0.0);
    auto oneRoot = oneTree.GetRoot();
    oneTree.AddSucc(1.0, oneRoot);
    oneTree.AddSucc(4.0, oneRoot);
    std::cout << "One tree:" << std::endl;
    std::cout << "Testing succs:" << std::endl;
    auto firstLevel = oneTree.GetSuccs(oneRoot);
    for (auto& leaf : firstLevel)
    {
        std::cout << leaf << " ";
    }
    oneTree.AddSucc(2.0, firstLevel.begin());
    oneTree.AddSucc(3.0, firstLevel.begin());
    std::cout << std::endl << "Dump:" << std::endl;
    Dump(oneTree.GetDF().begin(), oneTree.GetDF().end());
    oneTree.DeleteLeaf(++firstLevel.begin());
    std::cout << "Deleted last" << std::endl << "Dump:" << std::endl;
    Dump(oneTree.GetDF().begin(), oneTree.GetDF().end());

    naive_tree::LasyMuGraTree<int, double> lasyTree;
    lasyTree.AddSucc(std::variant<int, double>(13.5), lasyTree.SetRoot(std::variant<int, double>(13)));
    std::cout << "Lasy Tree:" << std::endl;
    std::cout << std::get<int>(*lasyTree.GetRoot()) << " ";
    std::cout << std::get<double>(*lasyTree.GetSuccs(lasyTree.GetRoot()).begin()) << std::endl;
    
    return 0;
}
