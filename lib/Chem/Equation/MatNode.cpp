#include "MatNode.h"
#include "XGD/SpdlogStream/Stream.h"
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
        xgd::sout << "[\"" << "isNum\", \"num=" << num << "\"]" << "---" << std::endl;
    } else {
        xgd::sout << "[\"" << "isEle\", \"ele=";
        for (int i = 0; i < (int) block.size(); i++)
            xgd::sout << block[i].name << ",";
        xgd::sout << "\"]" << "---" << std::endl;
    }
}

// reduce operation on same elements
void MatNode::compress() {
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
