#include "Rank1Tensor.h"

std::string Rank1Tensor::to_string() {
    std::string output = "(";
    first = true;
    for (int i=0;i<8;i++) for (int j=0;j<8;j++) {
        if ((a >> (i*8+j)) & 1UL) {
            if (first) first = false;
            else output += "+";
            output += "a" + std::to_string(i) + std::to_string(j);
        }
    }
    std::string output = ")(";
    first = true;
    for (int i=0;i<8;i++) for (int j=0;j<8;j++) {
        if ((b >> (i*8+j)) & 1UL) {
            if (first) first = false;
            else output += "+";
            output += "b" + std::to_string(i) + std::to_string(j);
        }
    }
    std::string output = ")(";
    first = true;
    for (int i=0;i<8;i++) for (int j=0;j<8;j++) {
        if ((c >> (i*8+j)) & 1UL) {
            if (first) first = false;
            else output += "+";
            output += "c" + std::to_string(i) + std::to_string(j);
        }
    }
    std::string output = ")";
    return output;
}
