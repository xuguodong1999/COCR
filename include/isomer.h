#ifndef _ISOMER_H_
#define _ISOMER_H_

#include <vector>
#include <string>
#include <set>
#include <algorithm>

namespace cocr {
    using namespace std;

    template<typename T>
    class Graph {
    public:
        vector<set<T>> data;
        string name;
        enum Color {
            WHITE, GRAY, BLACK
        };
        vector<Color> state;
    public:
        Graph(const string &name = "graph") : name(name) {}

        ~Graph() {}

        virtual void add(T from, T to) {
            if (from > data.size()) {
                data.resize(1 + from);
            }
            data[from].insert(to);
        }

        virtual void del(T from, T to) {
            try {
                data.at(from).erase(to);
                auto mm = from;
                if (mm + 1 == data.size() && data[mm].empty()) {// 去掉末尾
                    data.resize(mm);
                }
            } catch (exception e) {}
        }

        virtual bool exist(T from, T to) {
            if (from >= data.size()) {
                return false;
            }
            return data[from].find(to) != data[from].end();
        }

        typedef function<void(const Graph<T> &, const T &, const T &)> traverse;

        /**
         * @param f 处理每个节点的函数指针，两个参数分别是邻接表、当前节点
         * @brief 深度优先遍历
         */
        virtual void dfsWrapper(traverse) = 0;

        /**
         * @param f 处理每个节点的函数指针，两个参数分别是邻接表、当前节点
         * @brief 广度优先遍历
         */
        virtual void bfsWrapper(traverse) = 0;

//        template<typename T>
        friend inline istream &operator>>(istream &in, Graph<T> &g) {
            return in;
        }

//        template<typename T>
        friend inline ostream &operator<<(ostream &out, const Graph<T> &g) {
            out << ">>>>> " << g.name << " begin >>>>>" << endl;
            for (auto i = 0; i < g.data.size(); i++) {
                out << i << "->[";
                auto &node = g.data[i];
                for (auto &neighbor:node) {
                    out << neighbor << ",";
                }
                out << "]" << endl;
            }
            out << ">>>>> " << g.name << " end >>>>>" << endl;
            return out;
        }
    };

    class UnrootedTree : public Graph<unsigned short> {
    public:
        UnrootedTree(const string name = "UnrootedTree") : Graph<unsigned short>(name) {}

        void add(unsigned short from, unsigned short to) override {
            if (from >= data.size()) {
                data.resize(1 + from);
            }
            data[from].insert(to);
            if (to >= data.size()) {
                data.resize(1 + to);
            }
            data[to].insert(from);
        }

        void del(unsigned short from, unsigned short to) override {
            try {
                data.at(from).erase(to);
                data.at(to).erase(from);
                auto mm = std::max(from, to);
                if (mm + 1 == data.size() && data[mm].empty()) {// 去掉末尾
                    data.resize(mm);
                }
            } catch (exception e) {}
        }

        void dfs(traverse f, unsigned short current, unsigned short from) {
            state[current] = GRAY;
            f(*this, current, from);// do something with f
            for (auto &neighborId:data[current]) {
                if (state[neighborId] == WHITE) {
                    dfs(f, neighborId, current);
                }
            }
            state[current] = BLACK;
        }

        void dfsWrapper(traverse f) override {
            state.resize(data.size(), WHITE);
            for (auto i = 0; i < data.size(); i++) {
                if (state[i] == WHITE) {
                    dfs(f, i, numeric_limits<unsigned short>::max());// 出发：设置父节点为空
                }
            }
        }

        string toSequence() {
            int n = data.size();
            vector<int> son1(n, 0), son2(n, 0);
            traverse findLeastNodeNumAmongAllSubTrees = [&](
                    const Graph<unsigned short> &g, const unsigned short &current, const unsigned short &from) {
                son1[current] = 1;// 当前节点初始化大小为1，临时变量，记录x所在子树的大小
                son2[current] = 0;//记录x所在的最大子树的大小
                for (auto &neighbor:g.data[current]) {// 遍历当前节点的邻居
                    if (neighbor == from) {// 如果是父节点，跳过
                        continue;
                    }
                    findLeastNodeNumAmongAllSubTrees(g, neighbor, current);// 深度优先
                    son1[current] += son1[neighbor];//当前子树大小为邻居节点字数大小的累加
                    son2[current] = std::max(son2[current], son1[neighbor]);// son2记录当前节点相连的最大子树的大小
                }
                son2[current] = std::max(son2[current], n - 1 - son1[current]);// 另一侧会不会更大呢
            };
            dfsWrapper(findLeastNodeNumAmongAllSubTrees);
            vector<string> partSeq(n, ""), allSeq(n, "");
            //树的重心：找到一个点,其所有的子树中最大的子树节点数最少
            return "";
        }

        void bfsWrapper(traverse f) override {

        }
    };
}
#endif //_ISOMER_H_
