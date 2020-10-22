#include "isomer.hpp"
int main(int argc, char **argv) {
    auto &IC = IsomerCounter::GetInstance();
    IC.calculate(32, "/home/xgd/Downloads/alkane");
    return 0;
}
