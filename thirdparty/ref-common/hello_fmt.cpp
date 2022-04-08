#include <fmt/format.h>
#include <iostream>

int main(int argc, char **argv) {
    std::string s = fmt::format("The answer is {:03d}{:03d}{:03d}", 42, 42, 42);
    std::string ss = fmt::format("The answer is {:>10.2f}", 42.2);
    std::string sss = fmt::format("{:>60s}", std::string("hello, fmt"));
    std::cout << s << "\n";
    std::cout << ss << "\n";
    std::cout << sss << "\n";
    char a[1024 + 1];
    snprintf(a, 1024 + 1, "%3s", "hello");
    snprintf(a, 1024 + 1, "%60s", "hello, fmt");
    std::cout << a << std::endl;
    return 0;
}