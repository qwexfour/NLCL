#include <iostream>

#include "mugratree.hpp"

#define WANNA_DIE 0

auto main() -> int
{
    mugratree::Smth<int, double, char> test;
    test.Set('c');
    test.Dump();
    std::cout << "Get: " << test.Get<char>() << std::endl;
    test.Set(1);
    test.Dump();
    std::cout << "Get: " << test.Get<int>() << std::endl;
    test.Set(1.5);
    test.Dump();
    std::cout << "Get: " << test.Get<double>() << std::endl;

    mugratree::Smth<float, unsigned, double> another(3.0);
    another.Dump();
    std::cout << "Get: " << another.Get<double>() << std::endl;
#if WANNA_DIE
    test.Get<unsigned>();
#endif //WANNA_DIE
    return 0;
}
