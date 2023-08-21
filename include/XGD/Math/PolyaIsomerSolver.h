/**
 * https://github.com/snljty/CountAlkylIsomer
 */
#include "xgd_math_export.h"
#include <boost/multiprecision/cpp_int.hpp>

#include <iostream>
#include <vector>

class XGD_MATH_EXPORT PolyaIsomerCounter {
    using count_type = boost::multiprecision::cpp_int;
    using data_pair = std::pair<count_type, count_type>;
    std::vector<count_type> A, P, Q, C;
    const count_type two = 2, three = 3, six = 6, eight = 8, twenty_four = 24;

    void a_i(const size_t &m);

    void p_i(const size_t &m);

    void q_i(const size_t &m);

    void c_i(const size_t &m);

    void mkSpace(const size_t &buffer_size);

public:
    static PolyaIsomerCounter &GetInstance();

    /**
     * @return count of alkane and alkyl radical
     */
    data_pair count(const size_t &carbonNum);

    void log(const size_t &carbonNum);

    PolyaIsomerCounter(const PolyaIsomerCounter &) = delete;

private:
    PolyaIsomerCounter() = default;

    ~PolyaIsomerCounter() = default;

    const PolyaIsomerCounter &operator=(const PolyaIsomerCounter &) = delete;
};

