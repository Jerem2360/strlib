#include "string.hpp"
#include <iostream>


int main()
{
    string a = "abcdef";

    for (auto c : a) {
        std::cout << c;
    }
    std::cout << '\n';
    std::cout << a[6] << '\n';
}
