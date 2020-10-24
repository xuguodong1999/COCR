#include "bignumber.hpp"
#include <iostream>
#include <vector>

class PolyaIsomerCounter {
    using count_type = BigNumber;
    std::vector<count_type> A, P, Q, C;

    void A_m(const size_t &m);

    void P_m(const size_t &m);

    void Q_m(const size_t &m);

    void C_m(const size_t &m);

    void mkSpace(const size_t &buffer_size) {
        A.clear();
        A.resize(buffer_size, 0);
        P.clear();
        P.resize(buffer_size, 0);
        Q.clear();
        Q.resize(buffer_size, 0);
        C.clear();
        C.resize(buffer_size, 0);
    }

public:
    static PolyaIsomerCounter &GetInstance() {
        static PolyaIsomerCounter e;
        return e;
    }

    void count(const size_t &carbonNum) {
        mkSpace(carbonNum + 1);
        A_m(0);
        for (size_t i = 1; i <= carbonNum; i++) {
            A_m(i);
            P_m(i);
            Q_m(i);
            C_m(i);
            // 饱和烷烃自由基
            std::cout << "-c" << i << " : " << A[i] << std::endl;
            // 饱和烷烃
            std::cout << " c" << i << " : " << C[i] << std::endl;
        }
    }

private:
    PolyaIsomerCounter() {}

    ~PolyaIsomerCounter() {}

    PolyaIsomerCounter(const PolyaIsomerCounter &) = delete;

    const PolyaIsomerCounter &operator=(const PolyaIsomerCounter &) = delete;
};

