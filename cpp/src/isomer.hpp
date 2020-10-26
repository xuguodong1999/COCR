#ifndef _ISOMER_HPP_
#define _ISOMER_HPP_

#include "config.hpp"

#include <stack>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <fstream>

enum NodeColor {
    WHITE, GRAY, BLACK
};

/**
 * 无向图
 * @tparam T 节点类型：支持算术操作符的基本数据类型
 */
template<typename T>
class AlkaneGraph {
    std::vector<NodeColor> state;
    std::vector<std::vector<T>> content;
    using traverse = std::function<void(const T &, const T &)>;
    using idle = std::function<void(void)>;

    void dfs(const T &current, const T &from, traverse before, traverse in, traverse after) {
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

    void dfsWrapper(traverse before, traverse in, traverse after) {
        state.resize(content.size(), WHITE);
        for (auto i = 0; i < content.size(); i++) {
            if (state[i] == WHITE) {    // 出发：设置父节点为空
                dfs(i, std::numeric_limits<T>::max(), before, in, after);
            }
        }
        state.clear();
    }

    void dfs_from(const T &current, const T &from, traverse before, traverse after) {
        before(current, from);
        for (auto &neighbor:content[current]) {
            if (neighbor != from || from == std::numeric_limits<T>::max()) {
                dfs_from(neighbor, current, before, after);
            }
        }
        after(current, from);
    }

    std::vector<size_t> findMaxSubTreeSize(const size_t &nodeNum) {
        std::vector<size_t> subTreeSize(nodeNum, 0), maxSubTreeSize(nodeNum, 0);
        auto before = [&](const T &current, const T &from) {
            subTreeSize[current] = 1;// 当前节点初始化大小为1，临时变量，记录current所在子树的大小
            maxSubTreeSize[current] = 0;// 记录current所在的最大子树的大小
        };
        auto in = [&](const T &current, const T &from) {
            subTreeSize[from] += subTreeSize[current];//当前子树大小为邻居节点字数大小的累加
            maxSubTreeSize[from] = std::max(maxSubTreeSize[from],//当前最大邻居
                                            subTreeSize[current]// 邻居
            );
        };
        auto after = [&](const T &current, const T &from) {
            maxSubTreeSize[current] = std::max(
                    maxSubTreeSize[current],
                    nodeNum - subTreeSize[current]);// 另一侧会不会更大呢
        };
        dfsWrapper(before, in, after);
        return std::move(maxSubTreeSize);
    }

    const char prefix = '1';
    const char suffix = '0';
public:
    std::string toString() {
        size_t nodeNum = content.size();
        std::string finalSeq;
        if (nodeNum == 1) {
            finalSeq += prefix;
            finalSeq += suffix;
            return std::move(finalSeq);
        }
        auto maxSubTreeSize = findMaxSubTreeSize(nodeNum);
        //  最大子树的节点数最小
        auto minST = *min_element(maxSubTreeSize.begin(), maxSubTreeSize.end());
        std::vector<std::string> partSeq(nodeNum), allSeq(nodeNum);
        auto before2 = [&](const T &current, const T &from) {
            allSeq[current] = prefix;
        };
        auto after2 = [&](const T &current, const T &from) {
            int now = 0;
            for (auto &neighbor:content[current]) {
                if (neighbor != from || from == std::numeric_limits<T>::max()) {
                    partSeq[now++] = allSeq[neighbor];
                }
            }
            sort(partSeq.begin(), partSeq.begin() + now);
            for (int i = 0; i < now; i++) {
                allSeq[current] += partSeq[i];
            }
            allSeq[current] += suffix;
        };
        for (auto i = 0; i < nodeNum; i++) { //  处理两个重心
            if (maxSubTreeSize[i] == minST) { //  等于最大子树的节点数
                dfs_from(i, std::numeric_limits<T>::max(), before2, after2);
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

    std::string toSMILES(const hash_type &hash_value) {
        AlkaneGraph<T> alkane;
        alkane.fromHash(hash_value);
        auto s_1_0 = alkane.toString();
        std::string smiles;
        for (auto &c:s_1_0) {
            if (c == prefix) {
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

    void operator=(const AlkaneGraph &_ag) {
        content = _ag.content;
    }

    size_t size() {
        return content.size();
    }

    std::vector<T> &at(T i) {
        return content[i];
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

    void fromHash(const hash_type &hash_value) {
        if (hash_value <= 2) {
            *this = GetInstance();
            return;
        }
        std::stringstream ss;
        ss << std::setbase(8) << hash_value;
        std::string hash_string_tmp = std::move(ss.str());
        static std::map<char, std::string> table = {
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
        // after reverse, hash_string looks like input order
        // std::reverse(hash_string.begin(),hash_string.end());
        // but here i use back() to get char one by one
        // normal: (()) 1100
        // input: 000....0001100
        // now, recover structure from normal: hash_string
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


class IsomerCounter {
    using graph = AlkaneGraph<node_type>;
    const int thread_num = 11;
public:
    static IsomerCounter &GetInstance();

    bool calculate(const int &numOfCarbon, const char *cache_dir = nullptr);

    void calculate_i_from_i_1(const char *save_dir, const int &carbonNum);

private:
    IsomerCounter();

    ~IsomerCounter();

    IsomerCounter(const IsomerCounter &) = delete;

    const IsomerCounter &operator=(const IsomerCounter &) = delete;

    /**
     * load save_dir/carbonNum.dat into lastSet
     * @param save_dir
     * @param carbonNum
     */
    void recover(const char *save_dir, const int &carbonNum);

    int n;
    std::vector<hash_type> lastSet;

    class quick_set {
    public:
        std::vector<std::vector<hash_type>> mData;
        size_t mSize;
        const size_t max_size = 268435493ULL / 4;

        quick_set() : mSize(0) {
            mData.resize(max_size);
        }

        void clear() {
            mSize = 0;
            mData.clear();
            mData.resize(max_size, std::vector<hash_type>());
            mData.shrink_to_fit();
        }

        size_t hash(const hash_type &_a) const {
            return _a % max_size;
        }

        size_t size() const {
            return mSize;
        }

        void insert(const hash_type &_a) {
            size_t index = hash(_a);
            mData[index].push_back(_a);
            mData[index].shrink_to_fit();
            ++mSize;
        }

        bool exist(const hash_type &_a) const {
            size_t index = hash(_a);
            for (auto &i:mData[index]) {
                if (i == _a) {
                    return true;
                }
            }
            return false;
        }

        void dump(const char *save_dir, const int &carbonNum) const {
            std::ofstream ofsm(save_dir + ("/" + std::to_string(carbonNum)) + ".dat",
                               std::ios::out | std::ios::binary);
            if (!ofsm.is_open()) {
                std::cerr << "fail to open "
                          << save_dir + ("/" + std::to_string(carbonNum)) + ".dat"
                          << std::endl;
                exit(-1);
            }
            hash_type full_size = mSize;
            ofsm.write(reinterpret_cast<const char *>( &full_size ), sizeof(hash_type));
            for (auto &i:mData) {
                for (auto &j:i) {
                    ofsm.write(reinterpret_cast<const char *>( &j ), sizeof(hash_type));
                }
            }
            ofsm.close();
        }
    } curSet;
};

#endif //_ISOMER_HPP_
