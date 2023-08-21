#include "XGD/Math/GreedyIsomerSolver.h"
#include "XGD/Base/Timer.h"

#include <fstream>
#include <unordered_set>

using namespace std;

IsomerCounter &IsomerCounter::GetInstance() {
    static IsomerCounter e;
    return e;
}


void IsomerCounter::recover(const char *save_dir, const int &carbonNum) {
    ifstream ifsm(save_dir + ("/" + to_string(carbonNum)) + ".dat",
                  ios::in | ios::binary);
    if (!ifsm.is_open()) {
        xgd::serr << "fail to open "
                  << save_dir + ("/" + to_string(carbonNum)) + ".dat"
                  << endl;
        exit(-1);
    }
    hash_type full_size;
    ifsm.read(reinterpret_cast<char *>(&full_size), sizeof(hash_type));
    lastSet.resize(full_size);
    lastSet.shrink_to_fit();
    for (hash_type i = 0; i < full_size; i++) {
        ifsm.read(reinterpret_cast< char *>(&lastSet[i]), sizeof(hash_type));
    }
    ifsm.close();
}

std::optional<IsomerCounter::count_type> IsomerCounter::calculate(const int &numOfCarbon, const char *cache_dir) {
    if (numOfCarbon <= 0) {
        return std::nullopt;
    } else if (numOfCarbon >= 28) {
        xgd::serr << "numOfCarbon=" << numOfCarbon << ">=28" << endl;
    }
    n = numOfCarbon;
    lastSet.clear();
    lastSet.push_back(graph::GetInstance().hash());
    count_type result{1};
    if (n == 1) return result;
    for (int i = 2; i <= n; i++) {
        Timer timer;
        timer.start();
        while (!lastSet.empty()) {
            // save and add one carbon
            graph current = graph(lastSet.back());
            lastSet.pop_back();
            const auto size = current.size();
            vector<hash_type> tmpSet(size, 0);
#pragma omp parallel for
            for (auto j = 0; j < size; j++) {
                auto current2 = current;
                auto &node = current2.at(j);
                if (node.size() < 4) {
                    auto check_and_record = [&]() -> void {
                        auto hash_value = current2.hash();
                        if (!curSet.exist(hash_value)) {
                            tmpSet[j] = hash_value;
                        }
                    };
                    current2.add_do(j, size, check_and_record);
                }
            }
            unordered_set<hash_type> tmpSet2;
            for (auto &mm: tmpSet) {
                if (mm != 0) { tmpSet2.insert(mm); }
            }
            for (auto &mm: tmpSet2) {
                curSet.insert(mm);
            }
        }
        result = curSet.size();
        xgd::sout << "c-" << i << ": " << result << endl;
        timer.stop(true);
        if (nullptr != cache_dir) {
            curSet.dump(cache_dir, i);
            curSet.clear();
            recover(cache_dir, i);
        } else {
            lastSet.clear();
            for (auto &bucket: curSet.mData) {
                for (auto &ele: bucket)
                    lastSet.push_back(ele);
            }
            curSet.clear();
        }
    }
    return result;
}

IsomerCounter::IsomerCounter() {

}

IsomerCounter::~IsomerCounter() {}

void IsomerCounter::calculate_i_from_i_1(const char *save_dir, const int &carbonNum) {
    if (carbonNum <= 2) {
        xgd::serr << "cache file <= 2-1 not generated" << endl;
        exit(-1);
    }
    ifstream ifsm(save_dir + ("/" + to_string(carbonNum - 1)) + ".dat",
                  ios::in | ios::binary);
    if (!ifsm.is_open()) {
        xgd::serr << "fail to open "
                  << save_dir + ("/" + to_string(carbonNum)) + ".dat"
                  << endl;
        exit(-1);
    }
    size_t pool_size = 1024ULL * 128ULL;
    hash_type count_i_1;
    vector<hash_type> hash_pool(pool_size);
    ifsm.read(reinterpret_cast<char *>(&count_i_1), sizeof(count_i_1));
    xgd::sout << "load_" << carbonNum - 1 << "=" << count_i_1 << endl;
    graph current;
    Timer timer;
    timer.start();
    hash_type i = 0;
    while (i < count_i_1) {
        if (i + pool_size < count_i_1) {
            for (int k = 0; k < pool_size; k++) {
                ifsm.read(reinterpret_cast< char *>(&hash_pool[k]), sizeof(hash_type));
            }
            for (int k = 0; k < pool_size; k++) {
                current = graph(hash_pool[k]);
                const auto size = current.size();
                vector<hash_type> tmpSet(size, 0);
#pragma omp parallel for
                for (auto j = 0; j < size; j++) {
                    auto current2 = current;
                    auto &node = current2.at(j);
                    if (node.size() < 4) {
                        auto check_and_record = [&]() -> void {
                            auto hash_value = current2.hash();
                            if (!curSet.exist(hash_value)) {
                                tmpSet[j] = hash_value;
                            }
                        };
                        current2.add_do(j, size, check_and_record);
                    }
                }
                unordered_set<hash_type> tmpSet2;
                for (auto &mm: tmpSet) {
                    if (mm != 0) { tmpSet2.insert(mm); }
                }
                for (auto &mm: tmpSet2) {
                    curSet.insert(mm);
                }
            }
            if (i % pool_size == 0) {
                xgd::sout << "i=" << i << endl;
                xgd::sout << "curSet.size()/" << to_string(pool_size) << "=" << curSet.size() / pool_size
                          << endl;
                xgd::sout << "curSet.size()/1000000=" << curSet.size() / 1000000ULL << endl;
                timer.display_duration();
            }
            i += pool_size;
        } else if (pool_size > 4) {
            pool_size /= 2;
            xgd::sout << "i=" << i << endl;
            xgd::sout << "decrease pool_size=" << pool_size << endl;
            timer.display_duration();
            continue;
        } else {
            xgd::sout << "i=" << i << endl;
            xgd::sout << "curSet.size()/" << to_string(pool_size) << "=" << curSet.size() / pool_size
                      << endl;
            xgd::sout << "curSet.size()/1000000=" << curSet.size() / 1000000ULL << endl;
            timer.display_duration();
            hash_type tmp;
            ifsm.read(reinterpret_cast< char *>(&tmp), sizeof(hash_type));
            current = graph(tmp);
            const auto size = current.size();
            vector<hash_type> tmpSet(size, 0);
#pragma omp parallel for
            for (auto j = 0; j < size; j++) {
                auto current2 = current;
                auto &node = current2.at(j);
                if (node.size() < 4) {
                    auto check_and_record = [&]() -> void {
                        auto hash_value = current2.hash();
                        if (!curSet.exist(hash_value)) {
                            tmpSet[j] = hash_value;
                        }
                    };
                    current2.add_do(j, size, check_and_record);
                }
            }
            unordered_set<hash_type> tmpSet2;
            for (auto &mm: tmpSet) {
                if (mm != 0) { tmpSet2.insert(mm); }
            }
            for (auto &mm: tmpSet2) {
                curSet.insert(mm);
            }
            i++;
        }
    }
    ifsm.close();
    xgd::sout << "generation done." << endl;
    hash_type len2 = curSet.size();
    xgd::sout << "c-" << carbonNum << ": " << len2 << endl;
    timer.stop(true);
    curSet.dump(save_dir, carbonNum);
}

std::vector<std::string> IsomerCounter::getIsomers(
        const vector<int> &_numsOfCarbon, const int &_maxCarbon) {
    std::unordered_set<int> numsOfCarbon;
    for (auto &num: _numsOfCarbon) {
        if (1 <= num && num <= _maxCarbon) {
            numsOfCarbon.insert(num);
        }
    }
    std::vector<std::string> smilesVec;
    if (numsOfCarbon.empty()) {
        return smilesVec;
    }
    if (numsOfCarbon.end() != numsOfCarbon.find(1)) {
        smilesVec.push_back("C");
    }
    auto max_ele = *(std::max_element(numsOfCarbon.begin(), numsOfCarbon.end()));
    if (max_ele < 1) {
        return smilesVec;
    } else if (max_ele == 1) {
        smilesVec.push_back("C");
        return smilesVec;
    }
    lastSet.clear();
    lastSet.push_back(graph::GetInstance().hash());
    for (int i = 2; i <= max_ele; i++) {
        while (!lastSet.empty()) {
            graph current = graph(lastSet.back());
            lastSet.pop_back();
            const auto size = current.size();
            vector<hash_type> tmpSet(size, 0);
#pragma omp parallel for
            for (auto j = 0; j < size; j++) {
                auto current2 = current;
                auto &node = current2.at(j);
                if (node.size() < 4) {
                    auto check_and_record = [&]() -> void {
                        auto hash_value = current2.hash();
                        if (!curSet.exist(hash_value)) {
                            tmpSet[j] = hash_value;
                        }
                    };
                    current2.add_do(j, size, check_and_record);
                }
            }
            unordered_set<hash_type> tmpSet2;
            for (auto &mm: tmpSet) {
                if (mm != 0) { tmpSet2.insert(mm); }
            }
            for (auto &mm: tmpSet2) {
                curSet.insert(mm);
            }
        }
        lastSet.clear();
        for (auto &bucket: curSet.mData) {
            for (auto &ele: bucket)
                lastSet.push_back(ele);
        }
        if (numsOfCarbon.end() != numsOfCarbon.find(i)) {
            for (auto &carbon_hash: lastSet) {
                auto smiles = convertAlkaneHashToSMILES<unsigned char, hash_type>(carbon_hash);
                smilesVec.push_back(smiles);
            }
        }
        curSet.clear();
    }
    return smilesVec;
}
