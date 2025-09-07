#ifndef RANK1TENSOR_H
#define RANK1TENSOR_H
#include <string>
#include <unistd.h>
#include <cstdint>

using namespace std;

class Rank1Tensor {
public:
    uint64_t a1;
    uint64_t b1;
    uint64_t a2;
    uint64_t b2;
    uint64_t c;

    Rank1Tensor() : a1{}, b1{}, a2{}, b2{}, c{} {};

    string to_string();
};

#endif //RANK1TENSOR_H
