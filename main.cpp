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
    //
    return 0;
}
