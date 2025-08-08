#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "Scheme.h"

Scheme::Scheme() {
    //ctor
}

Scheme::~Scheme() {
    //dtor
}

bool Scheme::update() {
    move_list.clear();
    // first we update each tensor
    for (int i=0; i<tensors.size(); i++) {
        if (tensors[i].update()) {
            tensors.erase(tensors.begin()+i);
            //cout << endl << endl << endl << "normal reduction" << endl << endl << endl;
            update();
            return true; // because there was a reduction
        }
    }
    // then we go through all pairs and check for flips
    for (int i=0; i<tensors.size()-1; i++) {
        Rank1Tensor& tensor1 = tensors[i];
        for (int j=i+1; j<tensors.size(); j++) {
            Rank1Tensor& tensor2 = tensors[j];
            // aeq, beq, ceq to help with future things
            int aeq = 0;
            while (tensor1.a[aeq] == tensor2.a[aeq]) aeq++;
            int beq = 0;
            while (tensor1.b[beq] == tensor2.b[beq]) beq++;
            int ceq = 0;
            while (tensor1.c[ceq] == tensor2.c[ceq]) ceq++;
            // is there a flip?
            if (aeq >= MAX_ORDER - tensor1.coeff or aeq >= MAX_ORDER - tensor2.coeff) move_list.push_back(tuple<int,int,int,char>(i,j,-1,'a')); // -1 means its a normal flip
            if (beq >= MAX_ORDER - tensor1.coeff or beq >= MAX_ORDER - tensor2.coeff) move_list.push_back(tuple<int,int,int,char>(i,j,-1,'b'));
            if (ceq >= MAX_ORDER - tensor1.coeff or ceq >= MAX_ORDER - tensor2.coeff) move_list.push_back(tuple<int,int,int,char>(i,j,-1,'c'));
            // a SENSIBLE eflip? i.e an eflip that can be followed by a NEW flip (this tells us what our choice of Gamma' should be)
            // THE EFLIP REDUCTIONS DON'T WORK YET, EVERYTHING ELSE DOES THOUGH
            if (beq>0 && ceq>0) {
                // does this lead to a reduction?
                int min_eq = min(beq,ceq);
                int max_coeff = max(tensor1.coeff,tensor2.coeff);
                if (MAX_ORDER - min_eq <= max_coeff) {
                    if (tensor1.coeff == max_coeff) { // then we need to reduce tensor1
                        int powdiff = tensor1.coeff - tensor2.coeff;
                        for (int ii=0;ii+powdiff < MAX_ORDER-tensor2.coeff;ii++) tensor2.a[ii+powdiff] ^= tensor1.a[ii];
                        tensors.erase(tensors.begin()+i);
                    } else { // then we need to reduce tensor2
                        int powdiff = tensor2.coeff - tensor1.coeff;
                        for (int ii=0;ii+powdiff < MAX_ORDER-tensor1.coeff;ii++) tensor1.a[ii+powdiff] ^= tensor2.a[ii];
                        tensors.erase(tensors.begin()+j);
                    }
                    update();
                    //cout << endl << endl << endl << "eflip reduction" << endl << endl << endl;
                    return true;
                }
                // is there a third tensor which might go well with either of them?
                int k = 0;
                while (k<tensors.size()) {
                    while (k==i or k==j) {
                        k++;
                        if (k>=tensors.size()) break; // we are done in the while loops
                    }
                    if (k>=tensors.size()) break;
                    Rank1Tensor& tensor3 = tensors[k];
                    // so is tensor3 a good choice?
                    int aeq1 = 0;
                    while (tensor1.a[aeq1] == tensor3.a[aeq1]) aeq1++;
                    int aeq2 = 0;
                    while (tensor2.a[aeq2] == tensor3.a[aeq2]) aeq2++;
                    if (aeq1 >= MAX_ORDER - min_eq - tensor1.coeff) move_list.push_back(tuple<int,int,int,char>(i,j,k,'a'));
                    if (aeq2 >= MAX_ORDER - min_eq - tensor2.coeff) move_list.push_back(tuple<int,int,int,char>(j,i,k,'a'));
                    k++;
                }
            }
            // DO SAME FOR b AND c
            if (aeq>0 && ceq>0) {
                // does this lead to a reduction?
                int min_eq = min(aeq,ceq);
                int max_coeff = max(tensor1.coeff,tensor2.coeff);
                if (MAX_ORDER - min_eq <= max_coeff) {
                    if (tensor1.coeff == max_coeff) { // then we need to reduce tensor1
                        int powdiff = tensor1.coeff - tensor2.coeff;
                        for (int ii=0;ii+powdiff < MAX_ORDER-tensor2.coeff;ii++) tensor2.b[ii+powdiff] ^= tensor1.b[ii];
                        tensors.erase(tensors.begin()+i);
                    } else { // then we need to reduce tensor2
                        int powdiff = tensor2.coeff - tensor1.coeff;
                        for (int ii=0;ii+powdiff < MAX_ORDER-tensor1.coeff;ii++) tensor1.b[ii+powdiff] ^= tensor2.b[ii];
                        tensors.erase(tensors.begin()+j);
                    }
                    update();
                    //cout << endl << endl << endl << "eflip reduction" << endl << endl << endl;
                    return true;
                }
                // is there a third tensor which might go well with either of them?
                int k = 0;
                while (k<tensors.size()) {
                    while (k==i or k==j) {
                        k++;
                        if (k>=tensors.size()) break; // we are done in the while loops
                    }
                    if (k>=tensors.size()) break;
                    Rank1Tensor& tensor3 = tensors[k];
                    // so is tensor3 a good choice?
                    int beq1 = 0;
                    while (tensor1.b[beq1] == tensor3.b[beq1]) beq1++;
                    int beq2 = 0;
                    while (tensor2.b[beq2] == tensor3.b[beq2]) beq2++;
                    if (beq1 >= MAX_ORDER - min_eq - tensor1.coeff) move_list.push_back(tuple<int,int,int,char>(i,j,k,'b'));
                    if (beq2 >= MAX_ORDER - min_eq - tensor2.coeff) move_list.push_back(tuple<int,int,int,char>(j,i,k,'b'));
                    k++;
                }
            }
            if (aeq>0 && beq>0) {
                // does this lead to a reduction?
                int min_eq = min(aeq,beq);
                int max_coeff = max(tensor1.coeff,tensor2.coeff);
                if (MAX_ORDER - min_eq <= max_coeff) {
                    if (tensor1.coeff == max_coeff) { // then we need to reduce tensor1
                        int powdiff = tensor1.coeff - tensor2.coeff;
                        for (int ii=0;ii+powdiff < MAX_ORDER-tensor2.coeff;ii++) tensor2.c[ii+powdiff] ^= tensor1.c[ii];
                        tensors.erase(tensors.begin()+i);
                    } else { // then we need to reduce tensor2
                        int powdiff = tensor2.coeff - tensor1.coeff;
                        for (int ii=0;ii+powdiff < MAX_ORDER-tensor1.coeff;ii++) tensor1.c[ii+powdiff] ^= tensor2.c[ii];
                        tensors.erase(tensors.begin()+j);
                    }
                    update();
                    //cout << endl << endl << endl << "eflip reduction" << endl << endl << endl;
                    return true;
                }
                // is there a third tensor which might go well with either of them?
                int k = 0;
                while (k<tensors.size()) {
                    while (k==i or k==j) {
                        k++;
                        if (k>=tensors.size()) break; // we are done in the while loop
                    }
                    if (k>=tensors.size()) break;
                    Rank1Tensor& tensor3 = tensors[k];
                    // so is tensor3 a good choice?
                    int ceq1 = 0;
                    while (tensor1.c[ceq1] == tensor3.c[ceq1]) ceq1++;
                    int ceq2 = 0;
                    while (tensor2.c[ceq2] == tensor3.c[ceq2]) ceq2++;
                    if (ceq1 >= MAX_ORDER - min_eq - tensor1.coeff) move_list.push_back(tuple<int,int,int,char>(i,j,k,'c'));
                    if (ceq2 >= MAX_ORDER - min_eq - tensor2.coeff) move_list.push_back(tuple<int,int,int,char>(j,i,k,'c'));
                    k++;
                }
            }
        }
    }
    return false;
}

//REMEMBER TO WRITE IN FORM a0*e^2 ELSE IT WON'T WORK

void Scheme::from_file(string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file for reading: " << filename << std::endl;
        return;
    }
    tensors.clear();
    std::string line;
    while (std::getline(file, line)) {
        Rank1Tensor tensor;
        tensor.coeff = 0; // default
        std::smatch match;
        std::regex coeff_regex(R"(e(\^(\d+))?\*)");
        if (std::regex_search(line, match, coeff_regex)) {
            tensor.coeff = match[2].matched ? std::stoi(match[2]) : 1;
            line = line.substr(match[0].length());
        }
        std::regex group_regex(R"(\((.*?)\))");
        auto it = std::sregex_iterator(line.begin(), line.end(), group_regex);
        int group_idx = 0;
        for (; it != std::sregex_iterator() && group_idx < 3; ++it, ++group_idx) {
            std::string group = (*it)[1];
            std::istringstream terms(group);
            std::string term;
            while (std::getline(terms, term, '+')) {
                std::smatch tm;
                std::regex term_regex(R"(([abc])(\d+)(\*e(\^(\d+))?)?)");
                if (std::regex_match(term, tm, term_regex)) {
                    char var = tm[1].str()[0];
                    int index = std::stoi(tm[2]);
                    int pow = tm[5].matched ? std::stoi(tm[5]) : (tm[3].matched ? 1 : 0);
                    if (index < 0 || index >= N || pow < 0 || pow >= MAX_ORDER) continue;
                    if (var == 'a') tensor.a[pow][index] = 1;
                    else if (var == 'b') tensor.b[pow][index] = 1;
                    else if (var == 'c') tensor.c[pow][index] = 1;
                }
            }
        }
        tensors.push_back(tensor);
    }
    file.close();
    update();
}

void Scheme::write_to_file() {
    string filename = "k" + to_string(rand())+".exp";
    ostringstream output;
    for (int i=0;i<tensors.size();i++) {
        Rank1Tensor& tensor = tensors[i];
        if (i>0) {
            output << "\n";
        }
        if (tensor.coeff > 0) {
            output << "e";
            if (tensor.coeff > 1) {
                output << "^" << to_string(tensor.coeff);
            }
            output << "*";
        }
        bool first = true;
        output << "(";
        for (int pow=0;pow<MAX_ORDER-tensor.coeff;pow++) {
            for (int j=0;j<N;j++) {
                if (tensor.a[pow][j]==1) {
                    if (not first) {
                        output << "+";
                    } else {
                        first = false;
                    }
                    output << "a" + to_string(j);
                    if (pow > 0) {
                        output << "*e";
                        if (pow > 1) {
                            output << "^" << to_string(pow);
                        }
                    }
                }
            }
        }
        first = true;
        output << ")(";
        for (int pow=0;pow<MAX_ORDER-tensor.coeff;pow++) {
            for (int j=0;j<N;j++) {
                if (tensor.b[pow][j]==1) {
                    if (not first) {
                        output << "+";
                    } else {
                        first = false;
                    }
                    output << "b" << to_string(j);
                    if (pow > 0) {
                        output << "*e";
                        if (pow > 1) {
                            output << "^" << to_string(pow);
                        }
                    }
                }
            }
        }
        first = true;
        output << ")(";
        for (int pow=0;pow<MAX_ORDER-tensor.coeff;pow++) {
            for (int j=0;j<N;j++) {
                if (tensor.c[pow][j]==1) {
                    if (not first) {
                        output << "+";
                    } else {
                        first = false;
                    }
                    output << "c" << to_string(j);
                    if (pow > 0) {
                        output << "*e";
                        if (pow > 1) {
                            output << "^" << to_string(pow);
                        }
                    }
                }
            }
        }
        output << ")";
    }
    output << "\n";
    ofstream file(filename);
    if (file.is_open()) {
        file << output.str();
        file.close();
        cout << filename << "," << tensors.size() << endl;
    } else {
        cerr << "Failed to open file" << endl;
    }
}

/*
EXPLANATION FOR THE NORMAL FLIPS BEFORE I FORGET

we start with:
e^x a.b1.c1
e^y a.b2.c2
 ||
 ||
\  /
 \/
a.e^w1 b1.e^(x-w1) c1
a.e^w2 b2.e^(y-w2) c2
 ||
 ||
\  /
 \/
a.e^w1 b1 + e^(k+w2) b2.e^(x-w1) c1
a.e^w2 b2.e^(y-w2) c2 + e^(k+x-w1) c1
 ||
 ||
\  /
 \/
e^(x-w1) a.e^w1 b1 + e^(k+w2) b2. c1
a.b2.e^y c2 + e^(k+x-w1+w2) c1

so to do a general flip we choose 0<=w1<=x, 0<=w2<=y and k (we probably want to weight the choice of k)

*/

bool Scheme::flip(int ind1, int ind2, char flip_around) {
    Rank1Tensor& tensor1 = tensors[ind1];
    Rank1Tensor& tensor2 = tensors[ind2];
    int w1 = rand() % (tensor1.coeff + 1);
    int w2 = rand() % (tensor2.coeff + 1);
    int k = rand() % MAX_ORDER; // FIND A BETTER WAY TO CHOOSE k
    // Let's first ensure they're equal at flip_around (TURNS OUT THIS IS NECESSARY)
    switch(flip_around) {
        case 'a': {
            if (tensor1.coeff <= tensor2.coeff) {
                for (int i=MAX_ORDER-tensor2.coeff; i<MAX_ORDER; i++) {
                    tensor2.a[i] = tensor1.a[i];
                }
            } else {
                for (int i=MAX_ORDER-tensor1.coeff; i<MAX_ORDER; i++) {
                    tensor1.a[i] = tensor2.a[i];
                }
            }
            break;
        }
        case 'b': {
            if (tensor1.coeff <= tensor2.coeff) {
                for (int i=MAX_ORDER-tensor2.coeff; i<MAX_ORDER; i++) {
                    tensor2.b[i] = tensor1.b[i];
                }
            } else {
                for (int i=MAX_ORDER-tensor1.coeff; i<MAX_ORDER; i++) {
                    tensor1.b[i] = tensor2.b[i];
                }
            }
            break;
        }
        case 'c': {
            if (tensor1.coeff <= tensor2.coeff) {
                for (int i=MAX_ORDER-tensor2.coeff; i<MAX_ORDER; i++) {
                    tensor2.c[i] = tensor1.c[i];
                }
            } else {
                for (int i=MAX_ORDER-tensor1.coeff; i<MAX_ORDER; i++) {
                    tensor1.c[i] = tensor2.c[i];
                }
            }
            break;
        }
    }
    //now we do a flip
    switch(flip_around) {
        case 'a': {
            int powdiff = k + tensor1.coeff + w2 - w1 - tensor2.coeff;
            if (powdiff >= 0) {
                for (int i=powdiff;i<MAX_ORDER;i++) {
                    tensor2.c[i] ^= tensor1.c[i-powdiff];
                }
            } else {
                tensor2.coeff = k + tensor1.coeff - w1 + w2;
                for (int i=MAX_ORDER-1;i+powdiff>=0;i--) {
                    tensor2.c[i] = tensor1.c[i] ^ tensor2.c[i+powdiff];
                }
                for (int i=-powdiff-1;i>=0;i--) {
                    tensor2.c[i] = tensor1.c[i];
                }
            }
            powdiff = k + w2 - w1;
            if (powdiff >= 0) {
                for (int i=powdiff;i<MAX_ORDER;i++) {
                    tensor1.b[i] ^= tensor2.b[i-powdiff];
                }
            } else {
                tensor1.coeff += k + w2 - w1;
                for (int i=MAX_ORDER-1;i+powdiff>=0;i--) {
                    tensor1.b[i] = tensor2.b[i] ^ tensor1.b[i+powdiff];
                }
                for (int i=-powdiff-1;i>=0;i--) {
                    tensor1.b[i] = tensor2.b[i];
                }
            }
            break;
        }
        case 'b': {
            int powdiff = k + tensor1.coeff + w2 - w1 - tensor2.coeff;
            if (powdiff >= 0) {
                for (int i=powdiff;i<MAX_ORDER;i++) {
                    tensor2.a[i] ^= tensor1.a[i-powdiff];
                }
            } else {
                tensor2.coeff = k + tensor1.coeff - w1 + w2;
                for (int i=MAX_ORDER-1;i+powdiff>=0;i--) {
                    tensor2.a[i] = tensor1.a[i] ^ tensor2.a[i+powdiff];
                }
                for (int i=-powdiff-1;i>=0;i--) {
                    tensor2.a[i] = tensor1.a[i];
                }
            }
            powdiff = k + w2 - w1;
            if (powdiff >= 0) {
                for (int i=powdiff;i<MAX_ORDER;i++) {
                    tensor1.c[i] ^= tensor2.c[i-powdiff];
                }
            } else {
                tensor1.coeff += k + w2 - w1;
                for (int i=MAX_ORDER-1;i+powdiff>=0;i--) {
                    tensor1.c[i] = tensor2.c[i] ^ tensor1.c[i+powdiff];
                }
                for (int i=-powdiff-1;i>=0;i--) {
                    tensor1.c[i] = tensor2.c[i];
                }
            }
            break;
        }
        case 'c': {
            int powdiff = k + tensor1.coeff + w2 - w1 - tensor2.coeff;
            if (powdiff >= 0) {
                for (int i=powdiff;i<MAX_ORDER;i++) {
                    tensor2.b[i] ^= tensor1.b[i-powdiff];
                }
            } else {
                tensor2.coeff = k + tensor1.coeff - w1 + w2;
                for (int i=MAX_ORDER-1;i+powdiff>=0;i--) {
                    tensor2.b[i] = tensor1.b[i] ^ tensor2.b[i+powdiff];
                }
                for (int i=-powdiff-1;i>=0;i--) {
                    tensor2.b[i] = tensor1.b[i];
                }
            }
            powdiff = k + w2 - w1;
            if (powdiff >= 0) {
                for (int i=powdiff;i<MAX_ORDER;i++) {
                    tensor1.a[i] ^= tensor2.a[i-powdiff];
                }
            } else {
                tensor1.coeff += k + w2 - w1;
                for (int i=MAX_ORDER-1;i+powdiff>=0;i--) {
                    tensor1.a[i] = tensor2.a[i] ^ tensor1.a[i+powdiff];
                }
                for (int i=-powdiff-1;i>=0;i--) {
                    tensor1.a[i] = tensor2.a[i];
                }
            }
            break;
        }
    }
    return update();
}

/*

EXPLANATION FOR EFLIPS

e^x a1.b1.c1
e^y a1+e^q a2.b1+e^q b2.c2
 ||
_||_
\  /
 \/
a1.b1.e^x c1
a1+e^q a2.b1+e^q b2.e^y c2
 ||
_||_
\  /
 \/
a1.b1.e^x c1 + e^(MAX_ORDER - q) g
a1+e^q a2.b1+e^q b2.e^y c2 + e^(MAX_ORDER - q) g
 ||
_||_
\  /
 \/
e^x a1.b1.c1 + e^(MAX_ORDER - q - x) g
e^y a1+e^q a2.b1+e^q b2.c2 + e^(MAX_ORDER - q - y) g


And we choose g sensibly, such that c1+e^(MAX_ORDER-q-x) g = c3
where tensors contains e^z a3.b3.c3
and then we do a flip.

*/

bool Scheme::eflip(int ind1, int ind2, int ind3, char flip_around) { // flipping around c works, but not the others... I wonder why...
    //cout << "eflip" << endl;
    Rank1Tensor& tensor1 = tensors[ind1];
    Rank1Tensor& tensor2 = tensors[ind2];
    Rank1Tensor& tensor3 = tensors[ind3];
    bitset<N> shifted_g[MAX_ORDER];
    switch(flip_around) {
        case 'a':{
            // first we calculate e^(MAX_ORDER-q-x)g
            for (int i=0;i<MAX_ORDER;i++) {
            // note the redundancy: we know the first few of these will be 0 since tensor1.c and tensor3.c are equal up to some point. Could improve code by passing eqc and using that here
                shifted_g[i] = tensor1.a[i] ^ tensor3.a[i];
                tensor1.a[i] = tensor3.a[i];
            }
            // now we have to add it to tensor2.c
            // note we know shifting g further will work nicely because else we would have reduced it earlier
            int start=0;
            if (tensor1.coeff-tensor2.coeff<0) {
                start = tensor2.coeff-tensor1.coeff;
            }
            for (int i=start;i<MAX_ORDER and i+tensor1.coeff-tensor2.coeff<MAX_ORDER;i++) {
                tensor2.a[i+tensor1.coeff-tensor2.coeff] ^= shifted_g[i];
            }
            break;
        }
        case 'b':{
            // first we calculate e^(MAX_ORDER-q-x)g
            for (int i=0;i<MAX_ORDER;i++) {
            // note the redundancy: we know the first few of these will be 0 since tensor1.c and tensor3.c are equal up to some point. Could improve code by passing eqc and using that here
                shifted_g[i] = tensor1.b[i] ^ tensor3.b[i];
                tensor1.b[i] = tensor3.b[i];
            }
            // now we have to add it to tensor2.c
            // note we know shifting g further will work nicely because else we would have reduced it earlier
            int start=0;
            if (tensor1.coeff-tensor2.coeff<0) {
                start = tensor2.coeff-tensor1.coeff;
            }
            for (int i=start;i<MAX_ORDER and i+tensor1.coeff-tensor2.coeff<MAX_ORDER;i++) {
                tensor2.b[i+tensor1.coeff-tensor2.coeff] ^= shifted_g[i];
            }
            break;
        }
        case 'c':{
            // first we calculate e^(MAX_ORDER-q-x)g
            for (int i=0;i<MAX_ORDER;i++) {
            // note the redundancy: we know the first few of these will be 0 since tensor1.c and tensor3.c are equal up to some point. Could improve code by passing eqc and using that here
                shifted_g[i] = tensor1.c[i] ^ tensor3.c[i];
                tensor1.c[i] = tensor3.c[i];
            }
            // now we have to add it to tensor2.c
            // note we know shifting g further will work nicely because else we would have reduced it earlier
            int start=0;
            if (tensor1.coeff-tensor2.coeff<0) {
                start = tensor2.coeff-tensor1.coeff;
            }
            for (int i=start;i<MAX_ORDER and i+tensor1.coeff-tensor2.coeff<MAX_ORDER;i++) {
                tensor2.c[i+tensor1.coeff-tensor2.coeff] ^= shifted_g[i];
            }
            break;
        }
    }
    //now do a flip
    if (rand()%2==0) return flip(ind1,ind3,flip_around);
    else return flip(ind3,ind1,flip_around);
}

void Scheme::print() {
    for (int i=0;i<tensors.size();i++) {
        Rank1Tensor& tensor = tensors[i];
        if (i>0) {
            cout << "\n";
        }
        if (tensor.coeff > 0) {
            cout << "e";
            if (tensor.coeff > 1) {
                cout << "^" << to_string(tensor.coeff);
            }
            cout << "*";
        }
        bool first = true;
        cout << "(";
        for (int pow=0;pow<MAX_ORDER-tensor.coeff;pow++) {
            for (int j=0;j<N;j++) {
                if (tensor.a[pow][j]==1) {
                    if (not first) {
                        cout << "+";
                    } else {
                        first = false;
                    }
                    cout << "a" + to_string(j);
                    if (pow > 0) {
                        cout << "*e";
                        if (pow > 1) {
                            cout << "^" << to_string(pow);
                        }
                    }
                }
            }
        }
        first = true;
        cout << ")(";
        for (int pow=0;pow<MAX_ORDER-tensor.coeff;pow++) {
            for (int j=0;j<N;j++) {
                if (tensor.b[pow][j]==1) {
                    if (not first) {
                        cout << "+";
                    } else {
                        first = false;
                    }
                    cout << "b" << to_string(j);
                    if (pow > 0) {
                        cout << "*e";
                        if (pow > 1) {
                            cout << "^" << to_string(pow);
                        }
                    }
                }
            }
        }
        first = true;
        cout << ")(";
        for (int pow=0;pow<MAX_ORDER-tensor.coeff;pow++) {
            for (int j=0;j<N;j++) {
                if (tensor.c[pow][j]==1) {
                    if (not first) {
                        cout << "+";
                    } else {
                        first = false;
                    }
                    cout << "c" << to_string(j);
                    if (pow > 0) {
                        cout << "*e";
                        if (pow > 1) {
                            cout << "^" << to_string(pow);
                        }
                    }
                }
            }
        }
        cout << ")";
    }
    cout << endl << endl;
}

void Scheme::random_walk(int pathlength, int earlystop) {
    //Scheme compare_against = expanded(*this);
    for (int i=0;i<pathlength;i++) {
        if (move_list.size() == 0) break;
        tuple<int,int,int,char> next_flip = move_list[rand() % move_list.size()];
        //print();
        //cout << get<0>(next_flip) << get<1>(next_flip) << get<2>(next_flip) << get<3>(next_flip) << endl;
        if (get<2>(next_flip) == -1) {
            if (rand() % 2) {if (flip(get<0>(next_flip),get<1>(next_flip),get<3>(next_flip)) and earlystop) return;}
            else {if (flip(get<1>(next_flip),get<0>(next_flip),get<3>(next_flip)) and earlystop) return;}
        } else {if (eflip(get<0>(next_flip),get<1>(next_flip),get<2>(next_flip),get<3>(next_flip)) and earlystop) return;}
        //print();
        //cout << endl;
        /*Scheme new_compare_against = expanded(*this);
        if (compare_against.tensors.size() != new_compare_against.tensors.size()) {
            cerr << "ERROR HERE";
            return;
        }
        for (int j=0;j<compare_against.tensors.size();j++) {
            if (compare_against.tensors[j].a[0] != new_compare_against.tensors[j].a[0] || compare_against.tensors[j].b[0] != new_compare_against.tensors[j].b[0] || compare_against.tensors[j].c[0] != new_compare_against.tensors[j].c[0] || compare_against.tensors[j].coeff != new_compare_against.tensors[j].coeff) {
                cerr << "ERROR HERE";
                return;
            }
        }*/
    }
}

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
}
