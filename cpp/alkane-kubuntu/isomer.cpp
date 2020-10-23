#include "isomer.hpp"
#include <fstream>

template<typename T>
std::vector<NodeColor> AlkaneGraph<T>::state;

void Timer::start() {
    start_stamp = std::chrono::system_clock::now();
}

void Timer::display_duration() {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_stamp - start_stamp);
    std::cout << "Timer: [takes "
              << duration.count() * std::chrono::milliseconds::period::num / std::chrono::milliseconds::period::den
              << " ms]" << std::endl;
}

void Timer::stop(bool _display_duration) {
    end_stamp = std::chrono::system_clock::now();
    if (_display_duration) {
        display_duration();
    }
}

IsomerCounter &IsomerCounter::GetInstance() {
    static IsomerCounter e;
    return e;
}

void IsomerCounter::dump(const char *save_dir, const int &carbonNum) {
    std::ofstream ofsm(save_dir + std::string("/") + std::to_string(carbonNum) + ".dat",
                       std::ios::out | std::ios::binary);
    hash_type len = curSet.size();
    ofsm.write(reinterpret_cast<const char *>( &len ), sizeof(len));
    for (auto &i:curSet) {
        ofsm.write(reinterpret_cast<const char *>( &i ), sizeof(i));
    }
    ofsm.close();
}

void IsomerCounter::recover(const char *save_dir, const int &carbonNum) {
    std::ifstream ifsm(save_dir + std::string("/") + std::to_string(carbonNum) + ".dat",
                       std::ios::in | std::ios::binary);
    hash_type len;
    ifsm.read(reinterpret_cast<char *>(&len), sizeof(len));
    lastVec.resize(len);
    for (hash_type i = 0; i < len; i++) {
        ifsm.read(reinterpret_cast< char *>(&lastVec[i]), sizeof(hash_type));
    }
    ifsm.close();
}

bool IsomerCounter::calculate(const int &numOfCarbon, const char *cache_dir) {
    if (numOfCarbon <= 0) {
        return false;
    } else if (numOfCarbon >= 28) {
        std::cerr << "numOfCarbon=" << numOfCarbon << ">=28" << std::endl;
    }
    n = numOfCarbon;
    counter.clear();
    counter.resize(n + 1, 0);

//            smiles.insert(1);
    lastVec.clear();
    lastVec.push_back(graph::GetInstance().hash());                    // C1
    if (n == 1) {
        return true;
    }
    graph current;                                 // current structure
    for (int i = 2; i <= n; i++) {        // 由少到多迭代地计算
        Timer timer;
        timer.start();
        while (!lastVec.empty()) {
            // 记录，对一个合法结构添加新碳
            current = graph(lastVec.back());
            lastVec.pop_back();
            const auto size = current.size();
            for (auto j = 0; j < size; j++) {       // 尝试在这个结构的所有位置插入新原子
                auto &node = current.at(j);
                if (node.size() >= 4) { continue; }
                auto check_and_record = [&]() -> void {
                    auto hash_value = current.hash();
                    if (curSet.insert(hash_value).second) {// 遇到一种新结构
                        counter[i]++;                   // 计数++
                    }
                };
                current.add_do_del(j, size, check_and_record);
            }
        }
        std::cout << "c-" << i << ": " << counter[i] << std::endl;
        timer.stop(true);
        std::vector<hash_type>().swap(lastVec);
        if (nullptr == cache_dir) {
            while (!curSet.empty()) {
                lastVec.push_back(*curSet.begin());
                curSet.erase(curSet.begin());
            }
            curSet.clear();
            lastVec.resize(lastVec.size());
        } else {
            dump(cache_dir, i);
            curSet.clear();
            recover(cache_dir, i);
        }
    }


    return true;
}

IsomerCounter::IsomerCounter() {

}

IsomerCounter::~IsomerCounter() {}

void IsomerCounter::calculate_i_from_imm(const char *_cache_dir, const int &_i) {
    if (_i <= 2) {
        std::cerr << "cache file <= 2-1 not generated" << std::endl;
        exit(-1);
    }
    std::ifstream ifsm(_cache_dir + std::string("/") + std::to_string(_i - 1) + ".dat",
                       std::ios::in | std::ios::binary);
    const size_t cache_size = 1024ULL*1024ULL;
    hash_type len, tmp,  count=0;
    std::vector<hash_type>tmp1(cache_size);
    ifsm.read(reinterpret_cast<char *>(&len), sizeof(len));
    std::cout << "len=" << len << std::endl;
    std::vector<hash_type>().swap(lastVec);
    graph current;
    Timer timer;
    timer.start();
    hash_type i = 0;
    Timer timer2;
    timer2.start();
    while (i < len) {
        if (i + cache_size < len) {
            for (int k = 0; k < cache_size; k++) {
                ifsm.read(reinterpret_cast< char *>(&tmp1[k]), sizeof(hash_type));
            }
            for (int k = 0; k < cache_size; k++) {
                // 记录，对一个合法结构添加新碳
                current = graph(tmp1[k]);
                const auto size = current.size();
                for (auto j = 0; j < size; j++) {
                    auto &node = current.at(j);
                    if (node.size() >= 4) { continue; }
                    auto check_and_record = [&]() -> void {
                        auto hash_value = current.hash();
                        if (curSet.insert(hash_value).second) {
                            count++;
                            if (count % 1000000 == 0) {
                                std::cout << "count/1000000=" << count / 1000000 << std::endl;
				timer2.stop(true);
				timer2.start();
                            }
                        }
                    };
                    current.add_do_del(j, size, check_and_record);
                }
            }
            i += cache_size;
        } else {
            i++;
            ifsm.read(reinterpret_cast< char *>(&tmp), sizeof(hash_type));
            // 记录，对一个合法结构添加新碳
            current = graph(tmp);
            const auto size = current.size();
            for (auto j = 0; j < size; j++) {
                auto &node = current.at(j);
                if (node.size() >= 4) { continue; }
                auto check_and_record = [&]() -> void {
                    auto hash_value = current.hash();
                    if (curSet.insert(hash_value).second) {
                        count++;
                        if (count % 1000000 == 0) {
                            std::cout << "count/1000000=" << count / 1000000 << std::endl;
                        }
                    }
                };
                current.add_do_del(j, size, check_and_record);
            }
        }
    }
    ifsm.close();
    dump(_cache_dir, _i);
    std::cout << "c-" << _i << ": " << count << std::endl;
    timer.stop(true);
}
