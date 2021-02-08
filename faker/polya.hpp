/**
 * https://github.com/snljty/CountAlkylIsomer
 */
#include "bignumber.hpp"
#include <iostream>
#include <vector>

class PolyaIsomerCounter {
    using count_type = UnsignedInteger;
    std::vector<count_type> A, P, Q, C;
    const count_type two = 2, three = 3, six = 6, eight = 8, twenty_four = 24;

    void a_i(const size_t &m);

    void p_i(const size_t &m);

    void q_i(const size_t &m);

    void c_i(const size_t &m);

    void mkSpace(const size_t &buffer_size);

public:
    static PolyaIsomerCounter &GetInstance();

    void count(const size_t &carbonNum);

private:
    PolyaIsomerCounter() = default;

    ~PolyaIsomerCounter() = default;

    PolyaIsomerCounter(const PolyaIsomerCounter &) = delete;

    const PolyaIsomerCounter &operator=(const PolyaIsomerCounter &) = delete;
};

