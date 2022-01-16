/**
 * This file is a part of COCR Project
 * @author 徐国栋
 */
#ifndef _ISOMER_HPP_
#define _ISOMER_HPP_

#include "alkane_graph.hpp"

#include <fstream>
#include <map>
#include <algorithm>

class IsomerCounter {
    using graph = AlkaneGraph<node_type>;
    const int thread_num = 11;
public:
    static IsomerCounter &GetInstance();

    bool calculate(const int &numOfCarbon, const char *cache_dir = nullptr);

    void calculate_i_from_i_1(const char *save_dir, const int &carbonNum);

    std::vector<std::string> getIsomers(const std::vector<int> &_numsOfCarbon, const int &_maxCarbon = 20);

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
        // 536870923ULL for c1-28,1024ULL*1024ULL*512ULL,12G
        // 1073741824ULL for c29 and c30, 24G
        const size_t max_size = 1073741827ULL;

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
