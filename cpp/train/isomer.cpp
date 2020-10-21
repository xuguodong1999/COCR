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
            current = graph(lastVec.back(), i - 1);
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