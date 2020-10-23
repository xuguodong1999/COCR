#include <string>
#include <exception>
#include <iostream>
#include <iomanip>

#include "isomer.hpp"
#include <sstream>

int main(int argc, char **argv) {
    auto &IC = IsomerCounter::GetInstance();
    //IC.calculate(18, "D:/alkane");
    IC.calculate_i_from_imm("C:/Users/xgd/Downloads/alkane",28);
    return 0;

}
