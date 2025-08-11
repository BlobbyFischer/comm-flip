#include "Rank1Tensor.h"

string Rank1Tensor::to_string() {
    string output = "(";
    bool first = true;
    for (int i=0;i<8;i++) for (int j=0;j<8;j++) {
        if ((a1 >> (i*8+j)) & 1UL) {
            if (first) first = false;
            else output += "+";
            output += "a" + std::to_string(i+1) + std::to_string(j+1);
        }
    }
    for (int i=0;i<8;i++) for (int j=0;j<8;j++) {
        if ((b1 >> (i*8+j)) & 1UL) {
            if (first) first = false;
            else output += "+";
            output += "b" + std::to_string(i+1) + std::to_string(j+1);
        }
    }
    output += ")(";
    first = true;
    for (int i=0;i<8;i++) for (int j=0;j<8;j++) {
        if ((a2 >> (i*8+j)) & 1UL) {
            if (first) first = false;
            else output += "+";
            output += "a" + std::to_string(i+1) + std::to_string(j+1);
        }
    }
    for (int i=0;i<8;i++) for (int j=0;j<8;j++) {
        if ((b2 >> (i*8+j)) & 1UL) {
            if (first) first = false;
            else output += "+";
            output += "b" + std::to_string(i+1) + std::to_string(j+1);
        }
    }
    output += ")(";
    first = true;
    for (int i=0;i<8;i++) for (int j=0;j<8;j++) {
        if ((c >> (i*8+j)) & 1UL) {
            if (first) first = false;
            else output += "+";
            output += "c" + std::to_string(i+1) + std::to_string(j+1);
        }
    }
    output += ")";
    return output;
}
