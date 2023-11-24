#include "string.hpp"
#include <iostream>


int main()
{
    string ab = "ab";

    string a = "a";
    string b = "b";

    std::cout << a << '\n';

    string ab2 = a + b;

    std::cout << ab2 << '\n';

    std::cout << ab[0] << ab[1] << '\n';

    string user_input;

    std::cin >> user_input;

    std::cout << "input: \"" << user_input << "\"\n";
}
