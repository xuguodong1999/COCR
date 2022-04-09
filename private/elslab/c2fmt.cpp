/**
 * a tool to convert format string in c like s(n)printf to fmt::format
 */
#include <csignal>
#include <iostream>
#include <string>
#include <iterator>
#include <regex>

void regex_cppreference();

inline bool isnum(const char &c) {
    return '0' <= c && c <= '9';
}

inline bool isnumorminus(const char &c) {
    return c == '-' || ('0' <= c && c <= '9');
}

inline std::string c2fmt(const std::string &format) {
//    %10.3f%10i%10i
//    std::regex word_regex("%|\\s+");
//    using it = std::sregex_token_iterator;
//    std::vector<std::string> v(it(format.begin(), format.end(), word_regex, -1), it());
//    for (const auto &word: v) {
//        std::cout << "word=" << word << std::endl;
//    }
    std::string fmt_format;
    std::string token;
    auto polishToken = [&]() -> void {
        if (token.size() >= 3 && isnumorminus(token[2])) {
            if ('-' == token[2]) {
                token[2] = '<';
            }
        }
        fmt_format += token;
        token.clear();
    };
    for (decltype(format.size()) i = 0; i < format.size(); i++) {
        const auto &c = format[i];
        switch (c) {
            case '%':
                token = "{:";
                break;
            case 'f':
            case 'g':
            case 's':
            case 'c':
                if (token.empty()) {
                    fmt_format += c;
                    break;
                }
                token += c;
                token += '}';
                polishToken();
                break;
            case 'i':
            case 'd':
            case 'u':
                if (token.empty()) {
                    fmt_format += c;
                    break;
                }
                token += "d}";
                polishToken();
                break;
            default: {
                if (token.empty()) {
                    fmt_format += c;
                } else {
                    token += c;
                }
                break;
            }
        }
    }
    fmt_format += token;
    return "fmt::format(" + fmt_format + ",";
}

int main(int argc, char **argv) {
    auto sig_handler = [](int sig) -> void {
        std::cout << "Interrupt signal (" << sig << ") received.\n";
        exit(sig);
    };
    signal(SIGINT, sig_handler);

    std::string format;
    std::cout << argc << std::endl;
    if (argc == 2) {
        format = argv[1];
        std::cout << "format in c = " << format << std::endl;
        auto fmt_format = c2fmt(format);
        std::cout << "fmt::format = " << fmt_format << std::endl;
    } else {
        while (std::getline(std::cin, format)) {
            std::cout << "format in c = " << format << std::endl;
            auto fmt_format = c2fmt(format);
            std::cout << "fmt::format = " << fmt_format << std::endl;
        }
    }
    return EXIT_SUCCESS;
}

void regex_cppreference() {
    std::string s = "Some people, when confronted with a problem, think "
                    "\"I know, I'll use regular expressions.\" "
                    "Now they have two problems.";

    std::regex self_regex("REGULAR EXPRESSIONS",
                          std::regex_constants::ECMAScript | std::regex_constants::icase);
    if (std::regex_search(s, self_regex)) {
        std::cout << "Text contains the phrase 'regular expressions'\n";
    }

    std::regex word_regex("(\\w+)");
    auto words_begin =
            std::sregex_iterator(s.begin(), s.end(), word_regex);
    auto words_end = std::sregex_iterator();

    std::cout << "Found "
              << std::distance(words_begin, words_end)
              << " words\n";

    const int N = 6;
    std::cout << "Words longer than " << N << " characters:\n";
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        if (match_str.size() > N) {
            std::cout << "  " << match_str << '\n';
        }
    }

    std::regex long_word_regex("(\\w{7,})");
    std::string new_s = std::regex_replace(s, long_word_regex, "[$&]");
    std::cout << new_s << '\n';
}