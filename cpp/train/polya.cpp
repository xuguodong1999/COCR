#include <cstdio>

#define MAX_CARBON 10240
#define CALC_CARBON 28
unsigned long long A[MAX_CARBON + 1] = {0};
unsigned long long P[MAX_CARBON + 1] = {0}, Q[MAX_CARBON + 1] = {0};
unsigned long long C[MAX_CARBON + 1] = {0};

void TryCalc_A_m(unsigned long long m);

void Calc_A_m(unsigned long long m);

void TryCalc_P_m(unsigned long long m);

void Calc_P_m(unsigned long long m);

void TryCalc_Q_m(unsigned long long m);

void Calc_Q_m(unsigned long long m);

void TryCalc_C_m(unsigned long long m);

void Calc_C_m(unsigned long long m);

int main() {
    unsigned long long num = 0;

    Calc_A_m(0);
    for (num = 1; num <= CALC_CARBON; num++) {
        Calc_A_m(num);
        Calc_P_m(num);
        Calc_Q_m(num);
        Calc_C_m(num);
    }
    printf("CH%u-: %u\n", 2 * 1 + 1, A[1]);
    for (num = 2; num <= CALC_CARBON; num++)
        printf("C%uH%u-: %u\n", num, 2 * num + 1, A[num]);
    puts("");
    printf("CH%u: %u\n", 2 * 1 + 2, A[1]);
    for (num = 2; num <= CALC_CARBON; num++)
        printf("C%uH%u: %u\n", num, 2 * num + 2, C[num]);

    return 0;
}

void TryCalc_A_m(unsigned long long m) {
    /*  m >= 0 for A[m]  */
    unsigned long long i = 0;

    for (i = 0; i <= m; i++)
        if (!A[i])
            Calc_A_m(i);
    return;
}

void Calc_A_m(unsigned long long m) {
    /*  m >= 0 for A[m]  */
    unsigned long long t = 0, ans = 0;
    unsigned long long i = 0, j = 0;

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
    return;
}

void TryCalc_P_m(unsigned long long m) {
    /*  m >= 1 for P[m]  */
    unsigned long long i = 0;

    for (i = 0; i <= m; i++)
        if (!A[i])
            Calc_A_m(i);
    if (!P[m])
        Calc_P_m(m);
    return;
}

void Calc_P_m(unsigned long long m) {
    /*  m >= 1 for P[m]  */
    unsigned long long t = 0, ans = 0;
    unsigned long long i = 0, j = 0, k = 0;

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
    return;
}

void TryCalc_Q_m(unsigned long long m) {
    /*  m >= 1 for Q[m]  */
    unsigned long long i = 0;

    for (i = 0; i <= m; i++)
        if (!A[i])
            Calc_A_m(i);
    if (!Q[m] && m != 1)
        Calc_Q_m(m);
    return;
}

void Calc_Q_m(unsigned long long m) {
    /*  m >= 1 for Q[m]  */
    unsigned long long t = 0, ans = 0;
    unsigned long long i = 0;

    if (m == 1) {
        Q[m] == 0;
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
    return;
}

void TryCalc_C_m(unsigned long long m) {
    /*  m >= 1 for C[m]  */
    unsigned long long i = 1;

    if (!A[0])
        Calc_A_m(0);
    if (!A[1])
        Calc_A_m(1);
    if (!P[1])
        Calc_P_m(1);
    for (i = 2; i <= m; i++) {
        if (!A[i])
            Calc_A_m(i);
        if (!P[i])
            Calc_P_m(i);
        if (!Q[i])
            Calc_Q_m(i);
    }
    return;
}

void Calc_C_m(unsigned long long m) {
    /*  m >= 1 for C[m]  */
    unsigned long long ans = 0;
    if (!(m % 2))
        ans = P[m] - Q[m] + A[m / 2];
    else
        ans = P[m] - Q[m];
    C[m] = ans;
    return;
}