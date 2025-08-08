#include <iostream>
#include <ctime>
#include <unistd.h>

#include "Scheme.h"

using namespace std;

// I think I fixed eflips...

int main(int argc, char* argv[])
{
    if (3 > argc or 5 < argc) {
        cout << "USAGE: ./flip filename pathlength (check) (earlystop)" << endl;
        return 1;
    }
    srand(static_cast<unsigned int>(time(0)) + getpid());
    string filename = argv[1];
    int pathlength = stoi(argv[2]);
    int check;
    int earlystop;
    if (argc>3) check = stoi(argv[3]);
    else check = 0;
    if (argc>4) earlystop = stoi(argv[4]);
    else earlystop = 0;
    Scheme scheme;
    Scheme checker1;
    Scheme checker2;
    scheme.from_file(filename);
    if (check!=0) checker1 = expanded(scheme);
    scheme.update();
    scheme.random_walk(pathlength,earlystop);
    scheme.write_to_file();
    if (check!=0) {
        checker2 = expanded(scheme);
        if (checker1.tensors.size() != checker2.tensors.size()) {
            cerr << "ERROR HERE";
            return 1;
        }
        for (int j=0;j<checker1.tensors.size();j++) {
            if (checker1.tensors[j].a[0] != checker2.tensors[j].a[0] || checker1.tensors[j].b[0] != checker2.tensors[j].b[0] || checker1.tensors[j].c[0] != checker2.tensors[j].c[0] || checker1.tensors[j].coeff != checker2.tensors[j].coeff) {
                cerr << "ERROR HERE";
                return 1;
            }
        }
        cout << "correct" << endl;
    }
    /*
    //DON'T KEEP THIS IN NORMALLY
    for (int i=0; i<scheme.move_list.size(); i++) {
        cout << get<0>(scheme.move_list[i]) << " " << get<1>(scheme.move_list[i]) << " " << get<2>(scheme.move_list[i]) << " " << get<3>(scheme.move_list[i]) << " " << endl;
    }
    */
    return 0;
}
