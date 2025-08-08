#ifndef RANK1TENSOR_H
#define RANK1TENSOR_H
#include <string>
#include <array>
#include <utility>

#ifndef MOD
#define MOD 3
#endif // this should be prime and small to avoid overflows
const int N = 8; // max width/height of the matrix
const int MATRIX_SIZE = N*N; // total cells in the matrix


constexpr int compute_inv_at_idx(int i, int modulus) {
    if (i == 0) return 0; // ignore 0, we can't take an inverse of 0. It's just here to make the inverse of i inv[i] rather than inv[i-1]
    for (int j = 1; j < modulus; ++j) {
        if ((i * j) % modulus == 1) return j;
    }
    return 0; // hopefully not be reached for prime MOD and i != 0
}

template <int... Is>
constexpr std::array<short, MOD> make_inv_array(std::integer_sequence<int, Is...>) {
    return {compute_inv_at_idx(Is, MOD)...};
}

inline constexpr std::array<short, MOD> inv = make_inv_array(std::make_integer_sequence<int, MOD>{});

class rank1tensor {
public:
    short a[MATRIX_SIZE];
    short b[MATRIX_SIZE];
    short c[MATRIX_SIZE];
    short coeff;

    rank1tensor() : a{}, b{}, c{}, coeff(1) {};

    void update(); // this should normalise a b and c
    bool isnull();

    std::string to_string();
};

#endif //RANK1TENSOR_H
