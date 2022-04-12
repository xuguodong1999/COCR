#include <iostream>
#include <vector>
#include <optional>
#include <unordered_map>
#include <algorithm>

struct tuple_4_hash {
    template<class T1, class T2, class T3, class T4>
    inline std::size_t operator()(const std::tuple<T1, T2, T3, T4> &pair) const {
        return std::hash<T1>()(std::get<0>(pair))
               ^ std::hash<T2>()(std::get<1>(pair))
               ^ std::hash<T2>()(std::get<2>(pair))
               ^ std::hash<T2>()(std::get<3>(pair));
    }
};

using grid_pos = std::tuple<size_t, size_t, size_t, size_t>;
using grid_pair = std::pair<grid_pos, int>;
using grid_map = std::unordered_map<grid_pos, int, tuple_4_hash>;
static grid_map SSM;

void increaseGridMap(grid_map &grid, size_t x0, size_t y0, size_t x1, size_t y1, const int &c = 1) {
    if (x0 < x1 || (x0 == x1 && y0 < y1)) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    grid_pos pos(x0, y0, x1, y1);
    auto it = grid.find(pos);
    if (it == grid.end()) {
        grid[pos] = 1;
    } else {
        it->second += c;
    }
}

void unionGridMap(grid_map &dst, const grid_map &src) {
    for (const auto&[pos, c]: src) {
        const auto&[x0, y0, x1, y1]=pos;
        increaseGridMap(dst, x0, y0, x1, y1, c);
    }
}

void printGridMap(grid_map &grid) {
    std::vector<grid_pair> pairs;
    pairs.reserve(grid.size());
    for (const auto&[pos, c]: grid) {
        pairs.emplace_back(pos, c);
    }
    std::sort(pairs.begin(), pairs.end(), [](const grid_pair &a, const grid_pair &b) {
        return a.second > b.second;
    });
    std::unordered_map<int, int> mm;
    for (const auto&[pos, c]: pairs) {
        auto it = mm.find(c);
        if (it == mm.end()) {
            mm[c] = 1;
        } else {
            it->second++;
        }
    }
    for (const auto&[c, m]: mm) {
        std::cout << "repeat=" << m << ", cost=" << c << '\n';
    }
    std::cout << std::endl;
}

template<class T>
using vec = std::vector<T>;

template<class T>
using matrix = std::vector<std::vector<T>>;

using scalar = float;
const scalar zero(0), one(1);

template<class T>
matrix<T> make_matrix(const size_t &m, const size_t &n, const T &t) {
    matrix<T> mat;
    mat.resize(m, vec<T>(n, t));
    return mat;
}

template<class T, class len_type>
std::optional<matrix<T>> gemm(const matrix<T> &A, const matrix<T> &B,
                              const len_type &M, const len_type &N, const len_type &K) {
    // A:   MxK;    B:   KxN;   C:   MxN
    matrix<T> C = make_matrix(M, N, zero);
    for (len_type m = 0; m < M; m++) {
        std::vector<grid_map> SSMS(N);
#pragma omp parallel for default(none) shared(A, B, C, m, K, N)
        for (long long n = 0; n < N; n++) {
            C[m][n] = 0;
            for (len_type k = 0; k < K; k++) {
                C[m][n] += A[m][k] * B[k][n];
//                increaseGridMap(SSMS[n], m, k, k, n);
            }
        }
//        for (auto &ssm: SSMS) {
//            unionGridMap(SSM, ssm);
//        }
    }
    printGridMap(SSM);
    return C;
}

template<class T>
void print(const matrix<T> &mat, const std::string &name = "") {
    std::cout << "matrix " << name << " =[";
    for (const auto &row: mat) {
        std::cout << "\n\t";
        for (const auto &val: row) {
            std::cout << val << ",";
        }
    }
    std::cout << "\n]" << std::endl;
}

template<class T>
std::optional<matrix<T>> gemm(const matrix<T> &A, const matrix<T> &B) {
    using len_type = decltype(A.size());
    len_type A_row(A.size()), B_row(B.size());
    if (!(A_row && B_row)) {
        std::cerr << "matrix row == 0" << std::flush;
        return std::nullopt;
    }
    len_type A_col(A[0].size()), B_col(B[0].size());
    if (A_col != B_row) {
        std::cerr << "matrix A_col != B_row" << std::flush;
        return std::nullopt;
    }
    for (const auto &row: A) {
        if (row.size() != A_col) {
            std::cerr << "not a matrix for different col by row" << std::flush;
            return std::nullopt;
        }
    }
    for (const auto &row: B) {
        if (row.size() != B_col) {
            std::cerr << "not a matrix for different col by row" << std::flush;
            return std::nullopt;
        }
    }
    len_type M(A_row), N(B_col), K(A_col);
    return gemm(A, B, M, N, K);
}


int main() {
    const size_t M = 3000;
    const size_t N = 4000;
    const size_t K = 5000;
    matrix<scalar> A, B, C;
    A = make_matrix<scalar>(M, K, one * 1);
    B = make_matrix<scalar>(K, N, one * 1);
    auto result = gemm(A, B);
    if (result) {
        C = result.value();
//        print(C);
    }
    return 0;
}