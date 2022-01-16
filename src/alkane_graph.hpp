#ifndef _ALKANE_GRAPH_HPP_
#define _ALKANE_GRAPH_HPP_

#include "cocr_types.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <stack>
#include <functional>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>

enum NodeColor {
    WHITE, GRAY, BLACK
};

template<typename T>
class AlkaneGraph {
    std::vector<NodeColor> state;
    std::vector<std::vector<T>> content;
    using traverse = std::function<void(const T &, const T &)>;
    using idle = std::function<void(void)>;

    void dfs(const T &current, const T &from,
             const traverse &before, const traverse &in, const traverse &after) {
        state[current] = GRAY;
        before(current, from);
        for (auto &neighbor:content[current]) {
            if (state[neighbor] == WHITE) {
                dfs(neighbor, current, before, in, after);
                in(neighbor, current);
            }
        }
        after(current, from);
        state[current] = BLACK;
    }

    void dfs_from(const T &current, const T &from,
                  const traverse &before, const traverse &after) {
        before(current, from);
        for (auto &neighbor:content[current]) {
            if (neighbor != from || from == EMPTY_NODE) {
                dfs_from(neighbor, current, before, after);
            }
        }
        after(current, from);
    }

    std::vector<size_t> findMaxSubTreeSize(const size_t &nodeNum) {
        std::vector<size_t> subTreeSize(nodeNum, 0), maxSubTreeSize(nodeNum, 0);
        auto before = [&](const T &current, const T &from) {
            subTreeSize[current] = 1;
            maxSubTreeSize[current] = 0;
        };
        auto in = [&](const T &current, const T &from) {
            subTreeSize[from] += subTreeSize[current];
            maxSubTreeSize[from] = std::max(maxSubTreeSize[from],
                                            subTreeSize[current]
            );
        };
        auto after = [&](const T &current, const T &from) {
            maxSubTreeSize[current] = std::max(
                    maxSubTreeSize[current],
                    nodeNum - subTreeSize[current]);
        };
        dfsWrapper(before, in, after);
        return std::move(maxSubTreeSize);
    }

public:
    inline static const char prefix = '1', suffix = '0';
    inline static const T EMPTY_NODE = std::numeric_limits<T>::max();

    std::string toString() {
        size_t nodeNum = content.size();
        std::string finalSeq;
        if (nodeNum == 1) {
            finalSeq += prefix;
            finalSeq += suffix;
            return std::move(finalSeq);
        }
        auto maxSubTreeSize = findMaxSubTreeSize(nodeNum);
        auto minST = *min_element(maxSubTreeSize.begin(), maxSubTreeSize.end());
        std::vector<std::string> partSeq(nodeNum), allSeq(nodeNum);
        auto before = [&](const T &current, const T &from) {
            allSeq[current] = prefix;
        };
        auto after = [&](const T &current, const T &from) {
            int now = 0;
            for (auto &neighbor:content[current]) {
                if (neighbor != from || from == EMPTY_NODE) {
                    partSeq[now++] = allSeq[neighbor];
                }
            }
            sort(partSeq.begin(), partSeq.begin() + now);
            for (int i = 0; i < now; i++) {
                allSeq[current] += partSeq[i];
            }
            allSeq[current] += suffix;
        };
        for (auto i = 0; i < nodeNum; i++) {
            if (maxSubTreeSize[i] == minST) {
                dfs_from(i, EMPTY_NODE, before, after);
                if (allSeq[i] > finalSeq) {
                    finalSeq = allSeq[i];
                }
            }
        }
        return std::move(finalSeq);
    }

    AlkaneGraph() {
        content.resize(1);
    }

    AlkaneGraph(const hash_type &hash_value) {
        fromHash(hash_value);
    }

    AlkaneGraph(const AlkaneGraph &_ag) {
        content = _ag.content;
    }

    void operator=(const AlkaneGraph &_ag) {
        content = _ag.content;
    }

    size_t size() {
        return content.size();
    }

    std::vector<T> &at(T i) {
        return content[i];
    }

    void dfsWrapper(const traverse &before, const traverse &in, const traverse &after) {
        state.clear();
        state.resize(content.size(), WHITE);
        for (auto i = 0; i < content.size(); i++) {
            if (state[i] == WHITE) {
                dfs(i, EMPTY_NODE, before, in, after);
            }
        }
        state.clear();
    }

    std::vector<std::unordered_set<T>> dfsTraceGroup() {
        std::vector<std::unordered_set<T>> result;
        auto func = [&](const T &_a, const T &_b) {
            if (_a != EMPTY_NODE) {
                result.back().insert(_a);
            }
            if (_b != EMPTY_NODE) {
                result.back().insert(_b);
            }
        };
        state.clear();
        state.resize(content.size(), WHITE);
        for (auto i = 0; i < content.size(); i++) {
            if (state[i] == WHITE) {
                result.push_back(std::unordered_set<T>());
                dfs(i, EMPTY_NODE, func, func, func);
            }
        }
        state.clear();
        return std::move(result);
    }

    void push_node(T a) {
        if (content.size() <= a) {
            content.resize(a + 1);
        }
    }

    void push_back(T a, T b) {
        auto maxAB = std::max(a, b);
        if (content.size() <= maxAB) {
            content.resize(maxAB + 1);
        }
        content[a].push_back(b);
        content[b].push_back(a);
    }

    void pop_back(T a, T b) {
        content[a].pop_back();
        content[b].pop_back();
        if (content[std::max(a, b)].empty()) {
            content.pop_back();
        }
    }

    void add_do_del(T a, T b, idle func) {
        push_back(a, b);
        func();
        pop_back(a, b);
    }

    void add_do(T a, T b, idle func) {
        push_back(a, b);
        func();
    }

    unsigned long long hash() {
        return std::stoull(toString(), 0, 2);
    }

    static AlkaneGraph<T> GetInstance() {
        return AlkaneGraph<T>();
    }

    void fromHash(const hash_type &_hash_value) {
        if (_hash_value <= 2) {
            *this = GetInstance();
            return;
        }
        std::stringstream ss;
        ss << std::setbase(8) << _hash_value;
        std::string hash_string_tmp = std::move(ss.str());
        static std::unordered_map<char, std::string> table = {
                {'0', "000"},
                {'1', "001"},
                {'2', "010"},
                {'3', "011"},
                {'4', "100"},
                {'5', "101"},
                {'6', "110"},
                {'7', "111"},
        };
        std::string hash_string;
        for (auto &i:hash_string_tmp) {
            hash_string.append(table[i]);
        }
        std::reverse(hash_string.begin(), hash_string.end());
        while (!hash_string.empty() && hash_string.back() == suffix) {
            hash_string.pop_back();
        }
        content.clear();
        std::stack<T> nodeStack;
        T index = 0;
        while (!hash_string.empty()) {
            char c = hash_string.back();
            hash_string.pop_back();
            if (c == prefix) {
                nodeStack.push(index++);
            } else {
                auto cur = nodeStack.top();
                nodeStack.pop();
                if (!nodeStack.empty()) {
                    push_back(nodeStack.top(), cur);
                }
            }
        }
    }

    void fromSMILES(const std::string &_smiles) {
        std::string hash_string;
        for (auto &c:_smiles) {
            switch (c) {
                case ')':
                    hash_string.push_back(suffix);
                    break;
                case '(':
                    hash_string.push_back(prefix);
                    break;
            }
        }
        if (!_smiles.empty() && 'C' == _smiles[0]) {
            hash_string = prefix + hash_string + suffix;
        }
        std::reverse(hash_string.begin(), hash_string.end());
        content.clear();
        std::stack<T> nodeStack;
        T index = 0;
        while (!hash_string.empty()) {
            char c = hash_string.back();
            hash_string.pop_back();
            if (c == prefix) {
                nodeStack.push(index++);
            } else {
                auto cur = nodeStack.top();
                nodeStack.pop();
                if (!nodeStack.empty()) {
                    push_back(nodeStack.top(), cur);
                }
            }
        }
    }
};

template<typename T>
inline std::string convertAlkaneHashToSMILES(const hash_type &hash_value) {
    AlkaneGraph<T> alkane;
    alkane.fromHash(hash_value);
    auto s_1_0 = alkane.toString();
    std::string smiles;
    for (auto &c:s_1_0) {
        if (c == alkane.prefix) {
            smiles.append("(C");
        } else {
            smiles.push_back(')');
        }
    }
    if (smiles.size() > 2) {
        if (smiles.front() == '(' && smiles.back() == ')') {
            smiles = smiles.substr(1, smiles.size() - 2);
        }
    }
    return std::move(smiles);
};
#endif
