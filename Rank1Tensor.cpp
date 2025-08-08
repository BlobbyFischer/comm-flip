#include "Rank1Tensor.h"

Rank1Tensor::Rank1Tensor()
{
    //ctor
}

Rank1Tensor::~Rank1Tensor()
{
    //dtor
}

bool Rank1Tensor::update()
{
    int i = 0;
    while (a[i].none()) {
        i++;
    }
    int j = 0;
    while (b[j].none()) {
        j++;
    }
    int k = 0;
    while (c[k].none()) {
        k++;
    }
    if (i+j+k+coeff >= MAX_ORDER) {
        //then we need to remove this term...
        return true;
    } else {
        coeff += i+j+k;
        //shift the i's j's and k's
        if (i>0) {
            for (int ii=0; ii<MAX_ORDER-coeff; ii++) {
                a[ii] = a[ii+i];
            }
        }
        if (j>0) {
            for (int ii=0; ii<MAX_ORDER-coeff; ii++) {
                b[ii] = b[ii+j];
            }
        }
        if (k>0) {
            for (int ii=0; ii<MAX_ORDER-coeff; ii++) {
                c[ii] = c[ii+k];
            }
        }
        return false;
    }
}
