#pragma once

#include "els_math_export.h"
#include "math/alkane_graph.h"
#include "base/bignumber.h"
#include <fstream>
#include <map>
#include <iostream>
#include <algorithm>

class ELS_MATH_EXPORT IsomerCounter {
    using hash_type = uint64_t;
    using node_type = unsigned char;
    using graph = AlkaneGraph<node_type, hash_type, node_type>;
    const int thread_num = 11;
    static size_t cache_size_mb;
public:
    static IsomerCounter &GetInstance();

    std::optional<UnsignedInteger> calculate(const int &numOfCarbon, const char *cache_dir = nullptr);

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

    template<size_t MB>
    class quick_set {
        using bucket = std::vector<hash_type>;
    public:
        std::vector<bucket> mData;
        size_t mSize;
        // 536870923ULL for c1-28,1024ULL*1024ULL*512ULL,12G
        // 1073741824ULL for c29 and c30, 24G
        const size_t max_size = MB * 1024ULL * 1024ULL / sizeof(bucket);

        quick_set() : mSize(0) {
            mData.resize(max_size);
            mData.shrink_to_fit();
        }

        void clear() {
            mSize = 0;
            mData.clear();
            mData.resize(max_size, bucket());
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
            for (auto &i: mData[index]) {
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
            for (auto &i: mData) {
                for (auto &j: i) {
                    ofsm.write(reinterpret_cast<const char *>( &j ), sizeof(hash_type));
                }
            }
            ofsm.close();
        }
    };

    quick_set<1024> curSet;
};