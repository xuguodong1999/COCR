/**
 * https://github.com/snljty/CountAlkylIsomer
 */
#include "polya.hpp"

void PolyaIsomerCounter::a_i(const size_t &m) {
    /*  m >= 0 for A[m]  */
    /*  A[0] = 1  */
    if (!m) {
        A[m] = 1;
        return;
    }
    /*  term 1: x * A(x)^3  */
    count_type t = 0;
    for (size_t i = 0; i < m; i++)
        for (size_t j = 0; i + j < m; j++)
            t += A[i] * A[j] * A[m - 1 - i - j];
    A[m] += t;
    /*  term 2: x * 3 * A(x) * A(x^2)  */
    t = 0;
    for (size_t j = 0; 2 * j < m; j++)
        t += A[m - 1 - 2 * j] * A[j];
    A[m] += three * t;
    /* term 3: x * 2 * A(x^3)  */
    t = 0;
    if (!((m - 1) % 3))
        t = A[(m - 1) / 3];
    A[m] += two * t;
    /*  divided by 6  */
    A[m] /= six;
}

void PolyaIsomerCounter::p_i(const size_t &m) {
    /*  m >= 1 for P[m]  */
    /*  term 1: x * A(x)^4  */
    count_type t = 0;
    for (size_t i = 0; i < m; i++)
        for (size_t j = 0; i + j < m; j++)
            for (size_t k = 0; i + j + k < m; k++)
                t += A[i] * A[j] * A[k] * A[m - 1 - i - j - k];
    P[m] += t;
    /*  term 2: x * 6 * A(x)^2 * A(x^2)  */
    t = 0;
    for (size_t k = 0; 2 * k < m; k++)
        for (size_t i = 0; i + 2 * k < m; i++)
            t += A[i] * A[m - 1 - i - 2 * k] * A[k];
    P[m] += six * t;
    /*  term 3: x * 3 * A(x^2)^2  */
    t = 0;
    if (!((m - 1) % 2))
        for (size_t i = 0; 2 * i < m; i++)
            t += A[i] * A[(m - 1 - 2 * i) / 2];
    P[m] += three * t;
    /*  term 4: x * 8 * A(x) * A(x^3)  */
    t = 0;
    for (size_t j = 0; 3 * j < m; j++)
        t += A[m - 1 - 3 * j] * A[j];
    P[m] += eight * t;
    /*  term 5: x * 6 * A(x^4)  */
    t = 0;
    if (!((m - 1) % 4))
        t = A[(m - 1) / 4];
    P[m] += six * t;
    /*  divided by 24  */
    P[m] /= twenty_four;
}

void PolyaIsomerCounter::q_i(const size_t &m) {
    /*  m >= 1 for Q[m]  */
    if (m == 1) {
        Q[m] = 0;
        return;
    }
    /*  term 1: A(x)^2  */
    count_type t = 0;
    for (size_t i = 0; i <= m; i++)
        t += A[i] * A[m - i];
    Q[m] += t;
    /*  term 2: A(x^2)  */
    t = 0;
    if (!(m % 2))
        t = A[m / 2];
    Q[m] += t;
    /*  divided by 2  */
    Q[m] /= two;
    /*  term 3: - A(x)  */
    Q[m] -= A[m];
}

void PolyaIsomerCounter::c_i(const size_t &m) {
    /*  m >= 1 for C[m]  */
    if (!(m % 2))
        C[m] = P[m] - Q[m] + A[m / 2];
    else
        C[m] = P[m] - Q[m];
}

void PolyaIsomerCounter::mkSpace(const size_t &buffer_size) {
    A.clear();
    A.resize(buffer_size, 0);
    P.clear();
    P.resize(buffer_size, 0);
    Q.clear();
    Q.resize(buffer_size, 0);
    C.clear();
    C.resize(buffer_size, 0);
}

PolyaIsomerCounter &PolyaIsomerCounter::GetInstance() {
    static PolyaIsomerCounter e;
    return e;
}

void PolyaIsomerCounter::count(const size_t &carbonNum) {
    mkSpace(carbonNum + 1);
    a_i(0);
    for (size_t i = 1; i <= carbonNum; i++) {
        a_i(i);
        p_i(i);
        q_i(i);
        c_i(i);
        // 饱和烷烃自由基
        std::cout << "radical-c" << i << " : " << A[i] << std::endl;
        // 饱和烷烃
        std::cout << "alkane--c" << i << " : " << C[i] << std::endl;
    }
}