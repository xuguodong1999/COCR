#include "polya.hpp"
void PolyaIsomerCounter::A_m(const size_t& m) {
    /*  m >= 0 for A[m]  */
    count_type t = 0, ans = 0;
    size_t i = 0, j = 0;

    /*  A[0] = 1  */
    if (!m) {
        A[m] = 1;
        return;
    }
    /*  term 1: x * A(x)^3  */
    t = 0;
    for (i = 0; i < m; i++)
        for (j = 0; i + j < m; j++)
            t += A[i] * A[j] * A[m - 1 - i - j];
    ans += t;
    /*  term 2: x * 3 * A(x) * A(x^2)  */
    t = 0;
    for (j = 0; 2 * j < m; j++)
        t += A[m - 1 - 2 * j] * A[j];
    ans += 3 * t;
    /* term 3: x * 2 * A(x^3)  */
    t = 0;
    if (!((m - 1) % 3))
        t = A[(m - 1) / 3];
    ans += 2 * t;
    /*  divided by 6  */
    ans /= 6;
    A[m] = ans;
}

void PolyaIsomerCounter::P_m(const size_t& m) {
    /*  m >= 1 for P[m]  */
    count_type t = 0, ans = 0;
    size_t i = 0, j = 0, k = 0;

    /*  term 1: x * A(x)^4  */
    t = 0;
    for (i = 0; i < m; i++)
        for (j = 0; i + j < m; j++)
            for (k = 0; i + j + k < m; k++)
                t += A[i] * A[j] * A[k] * A[m - 1 - i - j - k];
    ans += t;
    /*  term 2: x * 6 * A(x)^2 * A(x^2)  */
    t = 0;
    for (k = 0; 2 * k < m; k++)
        for (i = 0; i + 2 * k < m; i++)
            t += A[i] * A[m - 1 - i - 2 * k] * A[k];
    ans += 6 * t;
    /*  term 3: x * 3 * A(x^2)^2  */
    t = 0;
    if (!((m - 1) % 2))
        for (i = 0; 2 * i < m; i++)
            t += A[i] * A[(m - 1 - 2 * i) / 2];
    ans += 3 * t;
    /*  term 4: x * 8 * A(x) * A(x^3)  */
    t = 0;
    for (j = 0; 3 * j < m; j++)
        t += A[m - 1 - 3 * j] * A[j];
    ans += 8 * t;
    /*  term 5: x * 6 * A(x^4)  */
    t = 0;
    if (!((m - 1) % 4))
        t = A[(m - 1) / 4];
    ans += 6 * t;
    /*  divided by 24  */
    ans /= 24;
    P[m] = ans;
}

void PolyaIsomerCounter::Q_m(const size_t& m) {
    /*  m >= 1 for Q[m]  */
    count_type t = 0, ans = 0;
    size_t i = 0;

    if (m == 1) {
        Q[m] = 0;
        return;
    }
    /*  term 1: A(x)^2  */
    t = 0;
    for (i = 0; i <= m; i++)
        t += A[i] * A[m - i];
    ans += t;
    /*  term 2: A(x^2)  */
    t = 0;
    if (!(m % 2))
        t = A[m / 2];
    ans += t;
    /*  divided by 2  */
    ans /= 2;
    /*  term 3: - A(x)  */
    ans -= A[m];
    Q[m] = ans;
}

void PolyaIsomerCounter::C_m(const size_t& m) {
    /*  m >= 1 for C[m]  */
    count_type ans = 0;
    if (!(m % 2))
        ans = P[m] - Q[m] + A[m / 2];
    else
        ans = P[m] - Q[m];
    C[m] = ans;
}