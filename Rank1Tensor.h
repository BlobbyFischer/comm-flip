#ifndef RANK1TENSOR_H
#define RANK1TENSOR_H
#include <string>

class Rank1Tensor {
public:
    unsigned long a1;
    unsigned long b1;
    unsigned long a2;
    unsigned long b2;
    unsigned long c;

    Rank1Tensor() : a1{}, b1{}, a2{}, b2{}, c{} {};

    std::string to_string();
};

#endif //RANK1TENSOR_H
