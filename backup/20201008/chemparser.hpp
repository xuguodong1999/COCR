//
// Created by xgd on 2020/9/18.
//

#ifndef PLAY_OPENCV_CHEMPARSER_HPP
#define PLAY_OPENCV_CHEMPARSER_HPP

#include <boost/tokenizer.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

class ChemParser {
    std::vector<std::vector<std::string>> mData;
public:
    ChemParser() {
        using namespace std;
        using namespace boost;
        using boost::algorithm::replace_all_copy;
        std::string input = "Zn+H(+)+NO3(-)=Zn(2+)+NH4(-)+H2O= = =Zn+H(+)+NO3(-)";
        input = replace_all_copy(input, " ", "");
        input = replace_all_copy(input, "+)", "pos)");
        input = replace_all_copy(input, "-)", "neg)");
        char_separator<char> sep(
                "+ ", "=", drop_empty_tokens);
        tokenizer<char_separator<char> > tok(input, sep);
        auto pos = tok.begin();
        while (pos != tok.end()) {
            std::cout << "[" << *pos << "]";
            if (*pos == "=") {
                while (*pos == "=")
                    pos++;
            } else {
                pos++;
            }
        }
        std::cout << std::endl;
    }
};


#endif //PLAY_OPENCV_CHEMPARSER_HPP
