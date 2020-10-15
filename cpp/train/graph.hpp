#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_

#include <vector>
#include <string>
#include <set>
#include <queue>
#include <algorithm>
#include <functional>

template<typename T>
class Graph {
public:
    Graph(const std::string &name = "graph") : name(name) {}

    Graph(const Graph &g) {
        dat = g.dat;
//            std::copy(g.dat.begin(), g.dat.end(), dat);
        name = g.name;
        state.clear();
    }

    void operator=(const Graph &g) {
        dat = g.dat;
//            std::copy(g.dat.begin(), g.dat.end(), dat);
        name = g.name;
        state.clear();
    }

    ~Graph() {}

    virtual void add(T from, T to) {
        if (from > dat.size()) {
            dat.resize(1 + from);
        }
        dat[from].insert(to);
    }

    virtual void del(T from, T to) {
        try {
            dat.at(from).erase(to);
            auto mm = from;
            if (mm + 1 == dat.size() && dat[mm].empty()) {// 去掉末尾
                dat.resize(mm);
            }
        } catch (std::exception e) {}
    }

    virtual bool exist(T from, T to) {
        if (from >= dat.size()) {
            return false;
        }
        return dat[from].find(to) != dat[from].end();
    }

    typedef std::function<void(Graph<T> &, const T &, const T &)> traverse;
    const traverse idle = [](Graph<T> &, const T &, const T &) {};

    /**
     * @param before 来到一个结点，处理它的邻居节点之前
     * @param in 处理一个结点的一个邻居节点后
     * @param after 处理完一个节点的额所有节点，即将离开这个节点
     */
    virtual void dfsWrapper(traverse before, traverse in, traverse after) =0;

    /**
     * @param enterQueue 遇到一个节点，将它加入队列
     * @param in 从队列取出一个节点，处理这个节点
     * @param after 一个节点的所有邻居入队列，即将离开or丢失这个节点
     */
    virtual void bfsWrapper(traverse enterQueue, traverse in, traverse after) =0;
    virtual std::string toString() = 0;

    friend inline std::istream &operator>>(std::istream &in, Graph<T> &g) {
        T x, y;
        while (in >> x >> y) {
            g.add(x, y);
        }
        return in;
    }

    friend inline std::ostream &operator<<(std::ostream &out, const Graph<T> &g) {
        out << ">>>>> " << g.name << " begin >>>>>" << std::endl;
        for (auto i = 0; i < g.dat.size(); i++) {
            out << (long long) i << "->[";
            auto &node = g.dat[i];
            for (auto &neighbor:node) {
                out << (long long) neighbor << ",";
            }
            out << "]" << std::endl;
        }
        out << ">>>>> " << g.name << " end >>>>>" << std::endl;
        return out;
    }

protected:
    std::vector<std::set<T>> dat;
    std::string name;
    enum Color {
        WHITE, GRAY, BLACK
    };
    std::vector<Color> state;

    void clearNodeState() {
        std::vector<Color> tmp;
        state.swap(tmp);// 释放颜色数组的内存
    }

    void initNodeState() {
        state.clear();
        state.resize(dat.size(), WHITE);
    }

public:
    decltype(dat.size()) getNodeNum() const {
        return dat.size();
    }

    const std::vector<std::set<T>> &getData() const {
        return dat;
    }

};

template<typename T>
class UGraph : public Graph<T> {
public:
    using Graph<T>::initNodeState;
    using Graph<T>::clearNodeState;
    using Graph<T>::getNodeNum;
    using Graph<T>::state;
    using Graph<T>::dat;
    using Graph<T>::idle;

    UGraph(const std::string name = "UnrootedTree") : Graph<T>(name) {
        dat.resize(1);
    }

    void add(T from, T to) override {
        if (from >= dat.size()) {
            dat.resize(1 + from);
        }
        dat[from].insert(to);
        if (to >= dat.size()) {
            dat.resize(1 + to);
        }
        dat[to].insert(from);
    }

    void del(T from, T to) override {
        try {
            dat.at(from).erase(to);
            dat.at(to).erase(from);
            auto mm = std::max(from, to);
            if (mm + 1 == dat.size() && dat[mm].empty()) {// 去掉末尾
                dat.resize(mm);
            }
        } catch (std::exception e) {}
    }

    using typename Graph<T>::traverse;
    /**
     * @param before 来到一个结点，处理它的邻居节点之前
     * @param in 处理一个结点的一个邻居节点后
     * @param after 处理完一个节点的额所有节点，即将离开这个节点
     */
    virtual void dfsWrapper(traverse before, traverse in, traverse after) override {
        initNodeState();
        for (auto i = 0; i < dat.size(); i++) {
            if (state[i] == Graph<T>::WHITE) {    // 出发：设置父节点为空
                dfs(i, std::numeric_limits<T>::max(), before, in, after);
            }
        }
        clearNodeState();
    }

    /**
     * @param enterQueue 遇到一个节点，将它加入队列
     * @param in 从队列取出一个节点，处理这个节点
     * @param after 一个节点的所有邻居入队列，即将离开or丢失这个节点
     */
    virtual void bfsWrapper(traverse enterQueue, traverse in, traverse after) override {
        initNodeState();
        std::queue<std::pair<T, T>> q;
        auto enQueue = [&](const T &current, const T &from) {
            q.push(std::make_pair(current, from));
            state[current] = Graph<T>::GRAY;
            enterQueue(*this, current, from);
        };
        for (auto bfsRoot = 0; bfsRoot < dat.size(); bfsRoot++) {
            if (state[bfsRoot] == Graph<T>::WHITE) {
                enQueue(bfsRoot, std::numeric_limits<T>::max());
                while (!q.empty()) {    // 完成从一个节点出发的广度优先遍历
                    auto currentNode = q.front();
                    q.pop();
                    auto &current = currentNode.first, &from = currentNode.second;
                    in(*this, current, from);
                    state[current] = Graph<T>::BLACK;
                    for (auto &neighbor:dat[current]) {
                        if (state[neighbor] == Graph<T>::WHITE) {
                            enQueue(neighbor, current);
                        }
                    }
                    after(*this, current, from);
                }
            }
        }
        clearNodeState();
    }

    /**
     * 从图的重心出发
     * @return 唯一字典序
     */
    std::string toString() override {
        using namespace std;
        auto size = getNodeNum();
        vector<int> subTreeSize(size, 0), maxSubTreeSize(size, 0);
        auto before = [&](Graph<T> &g, const T &current, const T &from) {
            subTreeSize[current] = 1;       // 当前节点初始化大小为1，临时变量，记录current所在子树的大小
            maxSubTreeSize[current] = 0;    // 记录current所在的最大子树的大小
        };
        auto in = [&](Graph<T> &g, const T &current, const T &from) {
            subTreeSize[from] += subTreeSize[current];    //当前子树大小为邻居节点字数大小的累加
            maxSubTreeSize[from] = max(maxSubTreeSize[from],//当前最大邻居
                                       subTreeSize[current]// 邻居
            );// son2记录当前节点相连的最大子树的大小
        };
        auto after = [&](Graph<T> &g, const T &current, const T &from) {
            maxSubTreeSize[current] = max(
                    maxSubTreeSize[current],
                    (int) size - subTreeSize[current]);// 另一侧会不会更大呢
        };
        dfsWrapper(before, in, after);
        auto minST = *min_element(maxSubTreeSize.begin(), maxSubTreeSize.end());//  最大子树的节点数
        cout << minST << std::endl;

//            std::string finalSeq = "";
//            std::vector<std::string> partSeq, allSeq;
//            partSeq.resize(size, "");
//            allSeq.resize(size, "");
//            auto before2 = [&](cocr::Graph<unsigned char> &_g, const unsigned char &current,
//                               const unsigned char &from) {
//                allSeq[current] = "(";
//            };
//            auto in2 = [&](cocr::Graph<unsigned char> &_g, const unsigned char &current, const unsigned char &from) {
//
//            };
//            auto after2 = [&](cocr::Graph<unsigned char> &_g, const unsigned char &current, const unsigned char &from) {
//                int now = 0;
//                for (auto &neighbor:_g.getData().at(current)) {
//                    if (neighbor != from) {
//                        partSeq[now++] = allSeq[neighbor];
//                    }
//                }
//                sort(partSeq.begin(), partSeq.begin() + now);
//                for (int i = 0; i < now; i++)
//                    allSeq[current] += partSeq[i];
//                allSeq[current] += ")";
//            };
//            for (auto i = 0; i < size; i++) {                //  处理两个重心
//                if (maxSubTreeSize[i] == minST) {        //  等于最大子树的节点数
//                    dfsWrapper(before2, in2, after2);
//                    if (allSeq[i] > finalSeq)
//                        finalSeq = allSeq[i];
//                }
//            }
//            return finalSeq;
        return "";
    }

private:
    void dfs(const T &current, const T &from, traverse before, traverse in, traverse after) {
        state[current] = Graph<T>::GRAY;
        before(*this, current, from);
        for (auto &neighbor:dat[current]) {
            if (state[neighbor] == Graph<T>::WHITE) {
                dfs(neighbor, current, before, in, after);
                in(*this, neighbor, current);
            }
        }
        after(*this, current, from);
        state[current] = Graph<T>::BLACK;
    }
};

#endif //!_GRAPH_HPP_