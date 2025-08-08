#ifndef RANK1TENSOR_H
#define RANK1TENSOR_H
#include <string>

class Rank1Tensor {
public:
    unsigned long a;
    unsigned long b;
    unsigned long c;

    Rank1Tensor() : a{}, b{}, c{} {};

    std::string to_string();
};

#endif //RANK1TENSOR_H
