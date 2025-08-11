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
            update(); // very inefficient but should be so rare it doesn't really matter
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

bool Scheme::flip(int ind1, int ind2, char flip_around1, char flip_around2) {
    Rank1Tensor& tensor1 = tensors[ind1];
    Rank1Tensor& tensor2 = tensors[ind2];
    if (flip_around1 == 'a' && flip_around2 == 'a') {
        // remove unwanted flips
        size_t i = 0;
        while (i<move_list.size()) {
            if ((get<0>(move_list[i]) == ind1 && get<2>(move_list[i]) == 'b') || (get<0>(move_list[i]) == ind2 && get<2>(move_list[i]) == 'c') || (get<1>(move_list[i]) == ind1 && get<3>(move_list[i]) == 'b') || (get<1>(move_list[i]) == ind2 && get<3>(move_list[i]) == 'c')) move_list.erase(move_list.begin()+i);
            else i++;
        }
        // do flip
        tensor1.a2 ^= tensor2.a2;
        tensor1.b2 ^= tensor2.b2;
        tensor2.c ^= tensor1.c;
        // check for reduction
        if ((tensor1.a2 == 0 && tensor1.b2 == 0) || tensor2.c == 0) return update();
        // add flips back to move_list
        for (size_t i=0; i<tensors.size();i++) {
            Rank1Tensor& tensori = tensors[i];
            // is there a flip?
            if (tensor1.a2 == tensori.a1 && tensor1.b2 == tensori.b1 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'b','a'));
            if (tensor1.a2 == tensori.a2 && tensor1.b2 == tensori.b2 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'b','b'));
            if (tensor2.c == tensori.c && i != ind2) move_list.push_back(tuple<int,int,char,char>(ind2,i,'c','c'));
        }
    } else if (flip_around1 == 'a' && flip_around2 == 'b') {
        // remove unwanted flips
        size_t i = 0;
        while (i<move_list.size()) {
            if ((get<0>(move_list[i]) == ind1 && get<2>(move_list[i]) == 'b') || (get<0>(move_list[i]) == ind2 && get<2>(move_list[i]) == 'c') || (get<1>(move_list[i]) == ind1 && get<3>(move_list[i]) == 'b') || (get<1>(move_list[i]) == ind2 && get<3>(move_list[i]) == 'c')) move_list.erase(move_list.begin()+i);
            else i++;
        }
        // do flip
        tensor1.a2 ^= tensor2.a1;
        tensor1.b2 ^= tensor2.b1;
        tensor2.c ^= tensor1.c;
        // check for reduction
        if ((tensor1.a2 == 0 && tensor1.b2 == 0) || tensor2.c == 0) return update();
        // add flips back to move_list
        for (size_t i=0; i<tensors.size();i++) {
            Rank1Tensor& tensori = tensors[i];
            // is there a flip?
            if (tensor1.a2 == tensori.a1 && tensor1.b2 == tensori.b1 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'b','a'));
            if (tensor1.a2 == tensori.a2 && tensor1.b2 == tensori.b2 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'b','b'));
            if (tensor2.c == tensori.c && i != ind2) move_list.push_back(tuple<int,int,char,char>(ind2,i,'c','c'));
        }
    } else if (flip_around1 == 'b' && flip_around2 == 'a') {
        // remove unwanted flips
        size_t i = 0;
        while (i<move_list.size()) {
            if ((get<0>(move_list[i]) == ind1 && get<2>(move_list[i]) == 'a') || (get<0>(move_list[i]) == ind2 && get<2>(move_list[i]) == 'c') || (get<1>(move_list[i]) == ind1 && get<3>(move_list[i]) == 'a') || (get<1>(move_list[i]) == ind2 && get<3>(move_list[i]) == 'c')) move_list.erase(move_list.begin()+i);
            else i++;
        }
        // do flip
        tensor1.a1 ^= tensor2.a2;
        tensor1.b1 ^= tensor2.b2;
        tensor2.c ^= tensor1.c;
        // check for reduction
        if ((tensor1.a1 == 0 && tensor1.b1 == 0) || tensor2.c == 0) return update();
        // add flips back to move_list
        for (size_t i=0; i<tensors.size();i++) {
            Rank1Tensor& tensori = tensors[i];
            // is there a flip?
            if (tensor1.a1 == tensori.a1 && tensor1.b1 == tensori.b1 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'a','a'));
            if (tensor1.a1 == tensori.a2 && tensor1.b1 == tensori.b2 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'a','b'));
            if (tensor2.c == tensori.c && i != ind2) move_list.push_back(tuple<int,int,char,char>(ind2,i,'c','c'));
        }
    } else if (flip_around1 == 'b' && flip_around2 == 'b') {
        // remove unwanted flips
        size_t i = 0;
        while (i<move_list.size()) {
            if ((get<0>(move_list[i]) == ind1 && get<2>(move_list[i]) == 'a') || (get<0>(move_list[i]) == ind2 && get<2>(move_list[i]) == 'c') || (get<1>(move_list[i]) == ind1 && get<3>(move_list[i]) == 'a') || (get<1>(move_list[i]) == ind2 && get<3>(move_list[i]) == 'c')) move_list.erase(move_list.begin()+i);
            else i++;
        }
        // do flip
        tensor1.a1 ^= tensor2.a1;
        tensor1.b1 ^= tensor2.b1;
        tensor2.c ^= tensor1.c;
        // check for reduction
        if ((tensor1.a1 == 0 && tensor1.b1 == 0) || tensor2.c == 0) return update();
        // add flips back to move_list
        for (size_t i=0; i<tensors.size();i++) {
            Rank1Tensor& tensori = tensors[i];
            // is there a flip?
            if (tensor1.a1 == tensori.a1 && tensor1.b1 == tensori.b1 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'a','a'));
            if (tensor1.a1 == tensori.a2 && tensor1.b1 == tensori.b2 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'a','b'));
            if (tensor2.c == tensori.c && i != ind2) move_list.push_back(tuple<int,int,char,char>(ind2,i,'c','c'));
        }
    } else if (flip_around1 == 'c' && flip_around2 == 'c') {
        short choice = rand() % 4;
        if (choice == 0) {
            // remove unwanted flips
            size_t i = 0;
            while (i<move_list.size()) {
                if ((get<0>(move_list[i]) == ind1 && get<2>(move_list[i]) == 'a') || (get<0>(move_list[i]) == ind2 && get<2>(move_list[i]) == 'b') || (get<1>(move_list[i]) == ind1 && get<3>(move_list[i]) == 'a') || (get<1>(move_list[i]) == ind2 && get<3>(move_list[i]) == 'b')) move_list.erase(move_list.begin()+i);
                else i++;
            }
            // do flip
            tensor1.a1 ^= tensor2.a1;
            tensor1.b1 ^= tensor2.b1;
            tensor2.a2 ^= tensor1.a2;
            tensor2.b2 ^= tensor1.b2;
            // check for reduction
            if ((tensor1.a1 == 0 && tensor1.b1 == 0) || (tensor2.a2 == 0 && tensor2.b2 == 0)) return update();
            // add flips back to move_list
            for (size_t i=0; i<tensors.size();i++) {
                Rank1Tensor& tensori = tensors[i];
                // is there a flip?
                if (tensor1.a1 == tensori.a1 && tensor1.b1 == tensori.b1 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'a','a'));
                if (tensor1.a1 == tensori.a2 && tensor1.b1 == tensori.b2 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'a','b'));
                if (tensor2.a2 == tensori.a1 && tensor2.b2 == tensori.b1 && i != ind2) move_list.push_back(tuple<int,int,char,char>(ind2,i,'b','a'));
                if (tensor2.a2 == tensori.a2 && tensor2.b2 == tensori.b2 && i != ind2) move_list.push_back(tuple<int,int,char,char>(ind2,i,'b','b'));
            }
        } else if (choice == 1) {
            // remove unwanted flips
            size_t i = 0;
            while (i<move_list.size()) {
                if ((get<0>(move_list[i]) == ind1 && get<2>(move_list[i]) == 'a') || (get<0>(move_list[i]) == ind2 && get<2>(move_list[i]) == 'a') || (get<1>(move_list[i]) == ind1 && get<3>(move_list[i]) == 'a') || (get<1>(move_list[i]) == ind2 && get<3>(move_list[i]) == 'a')) move_list.erase(move_list.begin()+i);
                else i++;
            }
            // do flip
            tensor1.a1 ^= tensor2.a2;
            tensor1.b1 ^= tensor2.b2;
            tensor2.a1 ^= tensor1.a2;
            tensor2.b1 ^= tensor1.b2;
            // check for reduction
            if ((tensor1.a1 == 0 && tensor1.b1 == 0) || (tensor2.a1 == 0 && tensor2.b1 == 0)) return update();
            // add flips back to move_list
            for (size_t i=0; i<tensors.size();i++) {
                Rank1Tensor& tensori = tensors[i];
                // is there a flip?
                if (tensor1.a1 == tensori.a1 && tensor1.b1 == tensori.b1 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'a','a'));
                if (tensor1.a1 == tensori.a2 && tensor1.b1 == tensori.b2 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'a','b'));
                if (tensor2.a1 == tensori.a1 && tensor2.b1 == tensori.b1 && i != ind2) move_list.push_back(tuple<int,int,char,char>(ind2,i,'a','a'));
                if (tensor2.a1 == tensori.a2 && tensor2.b1 == tensori.b2 && i != ind2) move_list.push_back(tuple<int,int,char,char>(ind2,i,'a','b'));
            }
        } else if (choice == 2) {
            // remove unwanted flips
            size_t i = 0;
            while (i<move_list.size()) {
                if ((get<0>(move_list[i]) == ind1 && get<2>(move_list[i]) == 'b') || (get<0>(move_list[i]) == ind2 && get<2>(move_list[i]) == 'b') || (get<1>(move_list[i]) == ind1 && get<3>(move_list[i]) == 'b') || (get<1>(move_list[i]) == ind2 && get<3>(move_list[i]) == 'b')) move_list.erase(move_list.begin()+i);
                else i++;
            }
            // do flip
            tensor1.a2 ^= tensor2.a1;
            tensor1.b2 ^= tensor2.b1;
            tensor2.a2 ^= tensor1.a1;
            tensor2.b2 ^= tensor1.b1;
            // check for reduction
            if ((tensor1.a2 == 0 && tensor1.b2 == 0) || (tensor2.a2 == 0 && tensor2.b2 == 0)) return update();
            // add flips back to move_list
            for (size_t i=0; i<tensors.size();i++) {
                Rank1Tensor& tensori = tensors[i];
                // is there a flip?
                if (tensor1.a2 == tensori.a1 && tensor1.b2 == tensori.b1 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'b','a'));
                if (tensor1.a2 == tensori.a2 && tensor1.b2 == tensori.b2 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'b','b'));
                if (tensor2.a2 == tensori.a1 && tensor2.b2 == tensori.b1 && i != ind2) move_list.push_back(tuple<int,int,char,char>(ind2,i,'b','a'));
                if (tensor2.a2 == tensori.a2 && tensor2.b2 == tensori.b2 && i != ind2) move_list.push_back(tuple<int,int,char,char>(ind2,i,'b','b'));
            }
        } else {
            // remove unwanted flips
            size_t i = 0;
            while (i<move_list.size()) {
                if ((get<0>(move_list[i]) == ind1 && get<2>(move_list[i]) == 'b') || (get<0>(move_list[i]) == ind2 && get<2>(move_list[i]) == 'a') || (get<1>(move_list[i]) == ind1 && get<3>(move_list[i]) == 'b') || (get<1>(move_list[i]) == ind2 && get<3>(move_list[i]) == 'a')) move_list.erase(move_list.begin()+i);
                else i++;
            }
            // do flip
            tensor1.a2 ^= tensor2.a2;
            tensor1.b2 ^= tensor2.b2;
            tensor2.a1 ^= tensor1.a1;
            tensor2.b1 ^= tensor1.b1;
            // check for reduction
            if ((tensor1.a2 == 0 && tensor1.b2 == 0) || (tensor2.a1 == 0 && tensor2.b1 == 0)) return update();
            // add flips back to move_list
            for (size_t i=0; i<tensors.size();i++) {
                Rank1Tensor& tensori = tensors[i];
                // is there a flip?
                if (tensor1.a2 == tensori.a1 && tensor1.b2 == tensori.b1 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'b','a'));
                if (tensor1.a2 == tensori.a2 && tensor1.b2 == tensori.b2 && i != ind1) move_list.push_back(tuple<int,int,char,char>(ind1,i,'b','b'));
                if (tensor2.a1 == tensori.a1 && tensor2.b1 == tensori.b1 && i != ind2) move_list.push_back(tuple<int,int,char,char>(ind2,i,'a','a'));
                if (tensor2.a1 == tensori.a2 && tensor2.b1 == tensori.b2 && i != ind2) move_list.push_back(tuple<int,int,char,char>(ind2,i,'a','b'));
            }
        }
    }
    return false;
}

void Scheme::plus() {
    int ind1 = rand() % tensors.size();
    int ind2 = rand() % (tensors.size() - 1);
    if (ind2 >= ind1) ind2++;
    Rank1Tensor& tensor1 = tensors[ind1];
    Rank1Tensor& tensor2 = tensors[ind2];
    // now do a plus between them
    Rank1Tensor newtensor;
    short choice = rand() % 5;
    if (choice == 0) {
        newtensor.a1 = tensor1.a1 ^ tensor2.a1;
        newtensor.b1 = tensor1.b1 ^ tensor2.b1;
        newtensor.a2 = tensor1.a2;
        newtensor.b2 = tensor1.b2;
        newtensor.c = tensor1.c;
        tensor1.a1 = tensor2.a1;
        tensor1.b1 = tensor2.b1;
        tensors.push_back(newtensor);
        flip(ind1,ind2,'a','a');
    } else if (choice == 1) {
        newtensor.a1 = tensor1.a1 ^ tensor2.a2;
        newtensor.b1 = tensor1.b1 ^ tensor2.b2;
        newtensor.a2 = tensor1.a1;
        newtensor.b2 = tensor1.b1;
        newtensor.c = tensor1.c;
        tensor1.a1 = tensor2.a2;
        tensor1.b1 = tensor2.b2;
        tensors.push_back(newtensor);
        flip(ind1,ind2,'a','b');
    } else if (choice == 2) {
        newtensor.a1 = tensor1.a2 ^ tensor2.a1;
        newtensor.b1 = tensor1.b2 ^ tensor2.b1;
        newtensor.a2 = tensor1.a1;
        newtensor.b2 = tensor1.b1;
        newtensor.c = tensor1.c;
        tensor1.a2 = tensor2.a1;
        tensor1.b2 = tensor2.b1;
        tensors.push_back(newtensor);
        flip(ind1,ind2,'b','a');
    } else if (choice == 3) {
        newtensor.a1 = tensor1.a2 ^ tensor2.a2;
        newtensor.b1 = tensor1.b2 ^ tensor2.b2;
        newtensor.a2 = tensor1.a1;
        newtensor.b2 = tensor1.b1;
        newtensor.c = tensor1.c;
        tensor1.a2 = tensor2.a2;
        tensor1.b2 = tensor2.b2;
        tensors.push_back(newtensor);
        flip(ind1,ind2,'b','b');
    } else {
        newtensor.a1 = tensor1.a1;
        newtensor.b1 = tensor1.b1;
        newtensor.a2 = tensor1.a2;
        newtensor.b2 = tensor1.b2;
        newtensor.c = tensor1.c ^ tensor2.c;
        tensor1.c = tensor2.c;
        tensors.push_back(newtensor);
        flip(ind1,ind2,'c','c');
    }
    update();
}

void Scheme::print() {
    for (size_t i=0;i<tensors.size();i++) {
        Rank1Tensor& tensor = tensors[i];
        cout << tensor.to_string() << endl;
    }
}

void Scheme::random_walk(int pathlength, int doplus, int earlystop) {
    int init_rank = tensors.size();
    if (doplus) plus();
    vector<Rank1Tensor> best_tensors = tensors;
    for (int i=0;i<pathlength;i++) {
        if (move_list.size() == 0) plus(); // possibly just break instead?
        tuple<int,int,char,char> next_flip = move_list[rand() % move_list.size()];
        if (rand() % 2) {
            if (flip(get<0>(next_flip),get<1>(next_flip),get<2>(next_flip),get<3>(next_flip))) {
                if (tensors.size() < init_rank) {
                    if (earlystop) return;
                    else best_tensors = tensors;
                }
            }
        } else {
            if (flip(get<1>(next_flip),get<0>(next_flip),get<3>(next_flip),get<2>(next_flip))) {
                if (tensors.size() < init_rank) {
                    if (earlystop) return;
                    else best_tensors = tensors;
                }
            }
        }
    }
    if (best_tensors.size() < init_rank) tensors = best_tensors;
}
