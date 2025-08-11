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
    vector<tuple<int,int,char,char>> move_list; // tuples are ind1, ind2 (which make the flip), char1, char2 (where it flips around)

    Scheme();
    virtual ~Scheme();
    void from_file(string filename);

    bool update();
    bool flip(int ind1, int ind2, char flip_around1, char flip_around2);
    void plus();

    void random_walk(int pathlength, int earlystop);

    void write_to_file();
    void print();

    //void check();
};

//Scheme expanded(Scheme input_scheme);

#endif // SCHEME_H
