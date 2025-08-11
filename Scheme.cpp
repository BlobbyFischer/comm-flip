#include <iostream>
#include <fstream>
#include <sstream>

#include "Scheme.h"

Scheme::Scheme() {
    //ctor
}

Scheme::~Scheme() {
    //dtor
}

bool Scheme::update() {
    move_list.clear();
    // first we ensure each tensor is non-zero
    for (int i=0; i<tensors.size(); i++) {
        if ((tensors[i].a1 == 0 and tensors[i].b1 == 0) or (tensors[i].a2 == 0 and tensors[i].b2 == 0) or tensors[i].c == 0) {
            tensors.erase(tensors.begin()+i);
            update();
            return true;
        }
    }
    // then we go through all pairs and check for flips
    for (int i=0; i<tensors.size()-1; i++) {
        Rank1Tensor& tensor1 = tensors[i];
        for (int j=i+1; j<tensors.size(); j++) {
            Rank1Tensor& tensor2 = tensors[j];
            // is there a flip?
            if (tensor1.a1 == tensor2.a1 && tensor1.b1 == tensor2.b1) move_list.push_back(tuple<int,int,char,char>(i,j,'a','a'));
            if (tensor1.a1 == tensor2.a2 && tensor1.b1 == tensor2.b2) move_list.push_back(tuple<int,int,char,char>(i,j,'a','b'));
            if (tensor1.a2 == tensor2.a1 && tensor1.b2 == tensor2.b1) move_list.push_back(tuple<int,int,char,char>(i,j,'b','a'));
            if (tensor1.a2 == tensor2.a2 && tensor1.b2 == tensor2.b2) move_list.push_back(tuple<int,int,char,char>(i,j,'b','b'));
            if (tensor1.c == tensor2.c) move_list.push_back(tuple<int,int,char,char>(i,j,'c','c'));
        }
    }
    return false;
}

void Scheme::from_file(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open file for reading: " << filename << endl;
        return;
    }
    tensors.clear();
    string line;
    while (std::getline(file, line)) {
        Rank1Tensor tensor;
        int part = 0;
        int charpoint = 0;
        while (charpoint < line.size()) {
            if (line[charpoint] == ')') {
                part++;
                charpoint++;
            } else if (line[charpoint] == 'a') {
                int i = line[charpoint+1] - '0' - 1;
                int j = line[charpoint+2] - '0' - 1;
                if (part == 0) {
                    tensor.a1 |= 1 << (8*i+j);
                } else if (part == 1) {
                    tensor.a2 |= 1 << (8*i+j);
                } else cerr << "Incorrect format" << endl;
                charpoint += 3;
            } else if (line[charpoint] == 'b') {
                int i = line[charpoint+1] - '0' - 1;
                int j = line[charpoint+2] - '0' - 1;
                if (part == 0) {
                    tensor.b1 |= 1 << (8*i+j);
                } else if (part == 1) {
                    tensor.b2 |= 1 << (8*i+j);
                } else cerr << "Incorrect format" << endl;
                charpoint += 3;
            } else if (line[charpoint] == 'c') {
                int i = line[charpoint+1] - '0' - 1;
                int j = line[charpoint+2] - '0' - 1;
                if (part == 2) {
                    tensor.c |= 1 << (8*i+j);
                } else cerr << "Incorrect format" << endl;
                charpoint += 3;
            } else charpoint++;
        }
        tensors.push_back(tensor);
    }
    file.close();
    update();
}

void Scheme::write_to_file() {
    string filename = "k" + to_string(rand())+".exp";
    ostringstream output;
    for (size_t i=0;i<tensors.size();i++) {
        Rank1Tensor& tensor = tensors[i];
        if (i>0) {
            output << "\n";
        }
        output << tensor.to_string();
    }
    //output << "\n";
    ofstream file(filename);
    if (file.is_open()) {
        file << output.str();
        file.close();
        cout << filename << "," << tensors.size() << endl;
    } else {
        cerr << "Failed to open file" << endl;
    }
}

bool Scheme::flip(int ind1, int ind2, char flip_around) {
    Rank1Tensor& tensor1 = tensors[ind1];
    Rank1Tensor& tensor2 = tensors[ind2];
    return update();
}

void Scheme::print() {
    for (size_t i=0;i<tensors.size();i++) {
        Rank1Tensor& tensor = tensors[i];
        cout << tensor.to_string() << endl;
    }
}

void Scheme::random_walk(int pathlength, int earlystop) {
    //Scheme compare_against = expanded(*this);
    for (int i=0;i<pathlength;i++) {
        if (move_list.size() == 0) break;
        tuple<int,int,char,char> next_flip = move_list[rand() % move_list.size()];
    }
}

/*
void Scheme::check() {
    for (int pow=0;pow<MAX_ORDER;pow++) {
        for (int i=0;i<N;i++) {
            for (int j=0;j<N;j++) {
                for (int k=0;k<N;k++) {
                    int counter = 0;
                    for (int ind=0;ind<tensors.size();ind++) {
                        Rank1Tensor tensor = tensors[ind];
                        for (int pow1=0;pow1<=pow;pow1++) {
                            for (int pow2=0;pow1+pow2+tensor.coeff<=pow;pow2++){
                                if (tensor.a[pow1][i] and tensor.b[pow2][j] and tensor.c[pow-pow1-pow2-tensor.coeff][k]){
                                    counter = 1-counter;
                                }
                            }
                        }
                    }
                    if (counter == 1) {
                        cout << "a" << i << " b" << j << " c" << k << " * e^" << pow << endl;
                    }
                }
            }
        }
    }
}

Scheme expanded(Scheme input_scheme) {
    Scheme output;
    for (int pow=0;pow<MAX_ORDER;pow++) {
        for (int i=0;i<N;i++) {
            for (int j=0;j<N;j++) {
                for (int k=0;k<N;k++) {
                    int counter = 0;
                    for (int ind=0;ind<input_scheme.tensors.size();ind++) {
                        Rank1Tensor tensor = input_scheme.tensors[ind];
                        for (int pow1=0;pow1<=pow;pow1++) {
                            for (int pow2=0;pow1+pow2+tensor.coeff<=pow;pow2++){
                                if (tensor.a[pow1][i] and tensor.b[pow2][j] and tensor.c[pow-pow1-pow2-tensor.coeff][k]){
                                    counter = 1-counter;
                                }
                            }
                        }
                    }
                    if (counter == 1) {
                        Rank1Tensor new_tensor;
                        new_tensor.a[0].set(i);
                        new_tensor.b[0].set(j);
                        new_tensor.c[0].set(k);
                        new_tensor.coeff = pow;
                        output.tensors.push_back(new_tensor);
                    }
                }
            }
        }
    }
    return output;
}*/
