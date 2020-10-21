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
    std::ofstream ofsm(save_dir + std::string("/") + std::to_string(carbonNum) + ".dat");
    for (auto &i:curSet) {
        ofsm.write(reinterpret_cast<const char *>( &i ), sizeof(i));
    }
    ofsm.close();
}

bool IsomerCounter::calculate(const int &numOfCarbon) {
    if (numOfCarbon <= 0) {
        return false;
    }
    n = numOfCarbon;
    counter.clear();
    counter.resize(n + 1, 0);
    try {
//            smiles.insert(1);
        std::unordered_set<hash_type> lastSet;
        lastSet.insert(graph::GetInstance().hash());                    // C1
        if (n == 1) {
            return true;
        } else {
            graph current;                                 // current structure
            for (int i = 2; i <= n; i++) {        // 由少到多迭代地计算
                Timer timer;
                timer.start();
                curSet.clear();
                while (!lastSet.empty()) {
//                    for (auto &old:lastSet) {     // 对于i-1碳的所有合法结构
                    current = graph(*lastSet.begin(), i - 1);    // 记录，对一个合法结构添加新碳
                    lastSet.erase(lastSet.begin());
                    const auto size = current.size();
                    for (auto j = 0; j < size; j++) {       // 尝试在这个结构的所有位置插入新原子
                        auto &node = current.at(j);
                        if (node.size() >= 4) { continue; }
                        auto check_and_record = [&]() -> void {
                            auto hash_value = current.hash();
                            if (curSet.insert(hash_value).second) {// 遇到一种新结构
                                counter[i]++;                   // 计数++
//                                curSet.push(current.hash());   // 记下新结构的图数据
//                                if (hash_value != graph(hash_value, i).hash()) {
//                                    std::cerr << hash_value << " vs "
//                                              << graph(hash_value, i).hash() << std::endl;
//                                }
                            }
                        };
                        current.add_do_del(j, size, check_and_record);
                    }
                }
                std::cout << "c-" << i << ": " << counter[i] << std::endl;
                timer.stop(true);
                dump("E:/alkane/",i);
                swap(lastSet, curSet);  // 用i碳图结构置换i-1碳图结构
            }
        }
    } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

IsomerCounter::IsomerCounter() {

}

IsomerCounter::~IsomerCounter() {}