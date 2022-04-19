#include <fmt/format.h>
#include <iostream>

int main(int argc, char **argv) {
    std::string hello = "hello";
    std::cout << hello.substr(1);
    std::string s = fmt::format("The answer is**- {:+d}:{:03d},{:10.6f},{:c}", 42, 42, 4200., 'c');
    std::string ss = fmt::format("The answer is {:<10.2f}", 42.2);
    std::string sss = fmt::format("{:>60s}", std::string("hello, fmt"));
    std::cout << s << "\n";
    std::cout << ss << "\n";
    std::cout << sss << "\n";
    char a[1024 + 1];
    snprintf(a, 1024 + 1, "%3s%09d", "hello", 12345);
//    snprintf(a, 1024 + 1, "%+60s", "hello, fmt");
    std::cout << a << std::endl;
    return EXIT_SUCCESS;
}