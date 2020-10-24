#include "isomer.hpp"
#include <fstream>
#include <unordered_set>

void Timer::start() {
    start_stamp = last_stamp = std::chrono::system_clock::now();
}

void Timer::display_duration() {
    end_stamp = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_stamp - last_stamp);
    std::cout << "Timer mid: [takes "
              << duration.count() * std::chrono::milliseconds::period::num / std::chrono::milliseconds::period::den
              << " ms]" << std::endl;
    last_stamp = end_stamp;
}

void Timer::stop(bool _display_duration) {
    end_stamp = last_stamp = std::chrono::system_clock::now();
    if (_display_duration) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                end_stamp - start_stamp);
        std::cout << "Timer end: [takes "
                  << duration.count() * std::chrono::milliseconds::period::num / std::chrono::milliseconds::period::den
                  << " ms]" << std::endl;
    }
}

IsomerCounter &IsomerCounter::GetInstance() {
    static IsomerCounter e;
    //e.curSet.max_load_factor(std::numeric_limits<float>::max());
    //e.curSet.rehash(1024ULL*1024ULL*512ULL);
    return e;
}


void IsomerCounter::recover(const char *save_dir, const int &carbonNum) {
    std::ifstream ifsm(save_dir + ("/" + std::to_string(carbonNum)) + ".dat",
                       std::ios::in | std::ios::binary);
    if (!ifsm.is_open()) {
        std::cerr << "fail to open "
                  << save_dir + ("/" + std::to_string(carbonNum)) + ".dat"
                  << std::endl;
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

bool IsomerCounter::calculate(const int &numOfCarbon, const char *cache_dir) {
    if (numOfCarbon <= 0) {
        return false;
    } else if (numOfCarbon >= 28) {
        std::cerr << "numOfCarbon=" << numOfCarbon << ">=28" << std::endl;
    }
    n = numOfCarbon;
    lastSet.clear();
    lastSet.push_back(graph::GetInstance().hash());
    if (n == 1) return true;
    for (int i = 2; i <= n; i++) {        // 由少到多迭代地计算
        Timer timer;
        timer.start();
        while (!lastSet.empty()) {
            // 记录，对一个合法结构添加新碳
            graph current = graph(lastSet.back());
            lastSet.pop_back();
            const auto size = current.size();
            for (auto j = 0; j < size; j++) {       // 尝试在这个结构的所有位置插入新原子
                auto &node = current.at(j);
                if (node.size() >= 4) { continue; }
                auto check_and_record = [&]() -> void {
                    auto hash_value = current.hash();
                    if (!curSet.exist(hash_value)) {// 遇到一种新结构
                        curSet.insert(hash_value);
                    }
                };
                current.add_do_del(j, size, check_and_record);
            }
        }
        std::cout << "c-" << i << ": " << curSet.size() << std::endl;
        timer.stop(true);
        curSet.dump(cache_dir, i);
        curSet.clear();
        recover(cache_dir, i);
    }


    return true;
}

IsomerCounter::IsomerCounter() {

}

IsomerCounter::~IsomerCounter() {}

void IsomerCounter::calculate_i_from_i_1(const char *save_dir, const int &carbonNum) {
    if (carbonNum <= 2) {
        std::cerr << "cache file <= 2-1 not generated" << std::endl;
        exit(-1);
    }
    std::ifstream ifsm(save_dir + ("/" + std::to_string(carbonNum - 1)) + ".dat",
                       std::ios::in | std::ios::binary);
    if (!ifsm.is_open()) {
        std::cerr << "fail to open "
                  << save_dir + ("/" + std::to_string(carbonNum)) + ".dat"
                  << std::endl;
        exit(-1);
    }
    size_t pool_size = 1024ULL * 128ULL;
    hash_type count_i_1;
    std::vector<hash_type> hash_pool(pool_size);
    ifsm.read(reinterpret_cast<char *>(&count_i_1), sizeof(count_i_1));
    std::cout << "load_" << carbonNum - 1 << "=" << count_i_1 << std::endl;
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
                // 记录，对一个合法结构添加新碳
                current = graph(hash_pool[k]);
                const auto size = current.size();
                std::vector<hash_type> tmpSet(size, 0);
#pragma omp parallel for num_threads(thread_num)
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
                std::unordered_set<hash_type> tmpSet2;
                for (auto &mm:tmpSet) {
                    if (mm != 0) { tmpSet2.insert(mm); }
                }
                for (auto &mm:tmpSet2) {
                    curSet.insert(mm);
                }
            }
            if (i % pool_size == 0) {
                std::cout << "i=" << i << std::endl;
                std::cout << "curSet.size()/" << std::to_string(pool_size) << "=" << curSet.size() / pool_size
                          << std::endl;
                std::cout << "curSet.size()/1000000=" << curSet.size() / 1000000ULL << std::endl;
                timer.display_duration();
            }
            i += pool_size;
        } else if (pool_size > 4) {
            pool_size /= 2;
            std::cout << "i=" << i << std::endl;
            std::cout << "decrease pool_size=" << pool_size << std::endl;
            timer.display_duration();
            continue;
        } else {
            std::cout << "i=" << i << std::endl;
            std::cout << "curSet.size()/" << std::to_string(pool_size) << "=" << curSet.size() / pool_size
                      << std::endl;
            std::cout << "curSet.size()/1000000=" << curSet.size() / 1000000ULL << std::endl;
            timer.display_duration();
            hash_type tmp;
            ifsm.read(reinterpret_cast< char *>(&tmp), sizeof(hash_type));
            current = graph(tmp);
            const auto size = current.size();
            std::vector<hash_type> tmpSet(size, 0);
#pragma omp parallel for num_threads(thread_num)
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
            std::unordered_set<hash_type> tmpSet2;
            for (auto &mm:tmpSet) {
                if (mm != 0) { tmpSet2.insert(mm); }
            }
            for (auto &mm:tmpSet2) {
                curSet.insert(mm);
            }
            i++;
        }
    }
    ifsm.close();
    std::cout << "generation done." << std::endl;
    hash_type len2 = curSet.size();
    std::cout << "c-" << carbonNum << ": " << len2 << std::endl;
    timer.stop(true);
    curSet.dump(save_dir,carbonNum);
}
