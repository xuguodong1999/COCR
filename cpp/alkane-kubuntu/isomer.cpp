#include "isomer.hpp"
#include <fstream>


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
    size_t pool_size = 1024ULL * 128ULL;
    hash_type len, tmp;
    std::vector<hash_type> hash_pool(1024ULL * 128ULL);
    ifsm.read(reinterpret_cast<char *>(&len), sizeof(len));
    std::cout << "load_" << _i - 1 << "=" << len << std::endl;
    std::vector<hash_type>().swap(lastVec);
    mm_set curSet2;
    //std::cin.get();
    graph current;
    Timer timer;
    timer.start();
    hash_type i = 0;
    const int thread_num = 11;
    while (i < len) {
        if (i + pool_size < len) {
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
                            if (!curSet2.exist(hash_value)) {
                                tmpSet[j] = hash_value;
                            }
                        };
                        current2.add_do(j, size, check_and_record);
                    }
                }
                std::unordered_set<hash_type> tmpSet2;
                for (auto &mm:tmpSet) {
                    if (mm != 0) {tmpSet2.insert(mm);}
                }
                for (auto &mm:tmpSet2) {
                    curSet2.insert(mm);
                }
            }
            if (i % pool_size == 0) {
                std::cout<<"i="<<i<<std::endl;
                std::cout << "curSet.size()/"<<std::to_string(pool_size)<<"=" << curSet2.size() / pool_size << std::endl;
                std::cout << "curSet.size()/1000000=" << curSet2.size() / 1000000ULL << std::endl;
                timer.display_duration();
            }
            i += pool_size;
        } 
        else if(pool_size > 4){
            pool_size /= 2;
            std::cout<<"i="<<i<<std::endl;
            std::cout<<"decrease pool_size="<<pool_size<<std::endl;
            timer.display_duration();
            continue;
        }else {
            std::cout<<"i="<<i<<std::endl;
            std::cout << "curSet.size()/"<<std::to_string(pool_size)<<"=" << curSet2.size() / pool_size << std::endl;
            std::cout << "curSet.size()/1000000=" << curSet2.size() / 1000000ULL << std::endl;
            timer.display_duration();
            ifsm.read(reinterpret_cast< char *>(&tmp), sizeof(hash_type));
            // 记录，对一个合法结构添加新碳
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
                            if (!curSet2.exist(hash_value)) {
                                tmpSet[j] = hash_value;
                            }
                        };
                        current2.add_do(j, size, check_and_record);
                    }
                }
                std::unordered_set<hash_type> tmpSet2;
                for (auto &mm:tmpSet) {
                    if (mm != 0) {tmpSet2.insert(mm);}
                }
                for (auto &mm:tmpSet2) {
                    curSet2.insert(mm);
                }
            i++;
        }
    }
    ifsm.close();
    std::cout << "generation done." << std::endl;
    hash_type len2 = curSet2.size();
    std::cout << "c-" << _i << ": " << len2 << std::endl;
    timer.stop(true);
    std::ofstream ofsm(_cache_dir + std::string("/") + std::to_string(_i) + ".dat",
                       std::ios::out | std::ios::binary);
    ofsm.write(reinterpret_cast<const char *>( &len2 ), sizeof(len2));
    size_t sss=0;
    for (auto &mmm:curSet2.mData) {
        for(auto&mm:mmm){
            ofsm.write(reinterpret_cast<const char *>( &mm ), sizeof(mm));
        }
    }
    ofsm.close();
}
