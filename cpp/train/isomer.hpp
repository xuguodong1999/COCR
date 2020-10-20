#ifndef _ISOMER_HPP_
#define _ISOMER_HPP_

#include "graph.hpp"
#include <vector>
#include <list>
#include <string>


class IsomerCounter {
public:
    static IsomerCounter &getInstance() {
        static IsomerCounter e;
        return e;
    }

    int n;
    typedef UGraph<unsigned char> CGraph;
    std::set<std::string> smiles;                   // C1-n alkanetopos' structure
    std::set<std::string> newSmiles;                // 新增部分的smiles
    std::list<CGraph> lastSet, curSet;              // C(i,i-1) alkanetopos' structure
    std::vector<long long> counter;                 // C1-n alkanetopo 数量计数器
    std::string convertGraphToString(CGraph &g) {
        return "";
    }

    CGraph getCarbon() {
        return CGraph();
    }

    bool calculate(const int &numOfCarbon) {
        if (numOfCarbon <= 0) {
            return false;
        }
        n = numOfCarbon;
        counter.clear();
        counter.resize(n + 1, 0);
        try {
            smiles.insert("C");
            lastSet.emplace_back(getCarbon());                    // C1
            if (n == 1) {
                return true;
            } else {
                CGraph current;                                 // current structure
                for (int i = 2; i <= n; i++) {        // 由少到多迭代地计算
                    newSmiles.clear();
                    curSet.clear();
                    for (auto &old:lastSet) {                   // 对于i-1碳的所有合法结构
                        current = old;                          // 记录，对一个合法结构添加新碳
                        const auto size = current.getData().size();
                        for (auto j = 0; j < size; j++) {       // 尝试在这个结构的所有位置插入新原子
                            auto &node = current.getData()[j];
                            if (node.size() >= 4) { continue; }
                            current.add(j, size);// 获得一个新结构
                            auto hash = current.toString();
                            if (newSmiles.insert(hash).second) {// 遇到一种新结构
                                counter[j]++;                   // 计数++
                                curSet.emplace_back(current);   // 记下新结构的图数据
                            }
                            current.del(j, size);               // 撤销新结构
                        }
                    }
                    swap(lastSet, curSet);               // 用i碳图结构置换i-1碳图结构
                    smiles.insert(newSmiles.begin(), newSmiles.end());
                }
            }
        } catch (std::exception e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

private:
    IsomerCounter() {}

    ~IsomerCounter() {}

    IsomerCounter(const IsomerCounter &) = delete;

    const IsomerCounter &operator=(const IsomerCounter &) = delete;
};

#endif //_ISOMER_HPP_
