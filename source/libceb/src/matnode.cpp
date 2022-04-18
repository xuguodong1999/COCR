#include "matnode.h"
#include <algorithm>

using namespace std;

MatNode::MatNode() {
    num = 0;
    isHead = 0;
    isNum = 0;
}

MatNode::~MatNode() {
    clear();
}

void MatNode::clear() {
    block.clear();
    num = 0;
    isHead = 0;
    isNum = 0;
}

void MatNode::print() {
    if (isNum) {
        cout << "[\"" << "isNum\", \"num=" << num << "\"]" << "---";
    } else {
        cout << "[\"" << "isEle\", \"ele=";
        for (int i = 0; i < (int) block.size(); i++)
            cout << block[i].name << ",";
        cout << "\"]" << "---";
    }
}


void MatNode::compress() {    //  压缩元素阵列 合并相同的元素 累加元素数量
    sort(block.begin(), block.end(), namelessthan);
    int j = 0;
    for (int i = 0; i < (int) block.size() - 1; i++) {
        j = i + 1;
        while (j < (int) block.size() && block[i].name == block[j].name) {
            block[i].amount += block[j].amount;
            j++;
        }
        block.erase(block.begin() + i + 1, block.begin() + j);
    }
}
