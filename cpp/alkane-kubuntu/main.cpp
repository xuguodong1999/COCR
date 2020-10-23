#include <string>
#include <exception>
#include <iostream>
#include <iomanip>

#include "isomer.hpp"
#include <sstream>

int main(int argc, char **argv) {
    auto &IC = IsomerCounter::GetInstance();
    //IC.calculate(18, "D:/alkane");
    IC.calculate_i_from_imm(
        "/home/xgd/Downloads/alkane",28
        //"/tmp/alkane",23
    );
    return 0;

}
