#ifndef SCHEME_H
#define SCHEME_H

#include <vector>
#include <tuple>
#include "Rank1Tensor.h"

using namespace std;

class Scheme
{
public:
    vector<Rank1Tensor> tensors;
    vector<tuple<int,int,int,char>> move_list; // tuples are ind1, ind2 (which make the eflip or flip), ind3 (which is -1 if normal flip or the thing ind1 can flip with after an eflip), char it flips around

    Scheme();
    virtual ~Scheme();
    void from_file(string filename);

    bool update();
    bool flip(int ind1, int ind2, char flip_around);
    bool eflip(int ind1, int ind2, int ind3, char flip_around);

    void random_walk(int pathlength, int earlystop);

    void write_to_file();
    void print();

    void check();
};

Scheme expanded(Scheme input_scheme);

#endif // SCHEME_H
