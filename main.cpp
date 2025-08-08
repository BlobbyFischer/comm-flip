#include <iostream>
#include <ctime>
#include <unistd.h>

#include "Scheme.h"

using namespace std;

// I think I fixed eflips...

int main(int argc, char* argv[])
{
    if (3 > argc or 4 < argc) {
        cout << "USAGE: ./flip filename pathlength (earlystop)" << endl;
        return 1;
    }
    srand(static_cast<unsigned int>(time(0)) + getpid());
    string filename = argv[1];
    int pathlength = stoi(argv[2]);
    int earlystop;
    if (argc>3) earlystop = stoi(argv[3]);
    else earlystop = 0;
    Scheme scheme;
    scheme.from_file(filename);
    scheme.update();
    scheme.random_walk(pathlength,earlystop);
    scheme.write_to_file();
    return 0;
}
