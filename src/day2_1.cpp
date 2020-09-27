#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


vector<int> parse_csv_ints(const char *filename) {
    fstream file;
    string tp;
    vector<int> integers;
    file.open(filename);
    if (!file.is_open()) {
        throw runtime_error(string("Unable to open file ") + string(filename));
    }
    while (getline(file, tp, ',')) {
        integers.push_back(stoi(tp));
    }
    file.close();

    return integers;
}


void print_intcode_program(vector<int> &intcode_program) {
    for (auto i: intcode_program) {
        cout << i << " ";
    }
    cout << endl;
}


void run_intcode_program(vector<int> &p) {
    
    for (size_t i{}; i<p.size(); i+=4) {

        switch (p[i]) {
            case 1:
                p[p[i+3]] = p[p[i+1]] + p[p[i+2]];
                break;

            case 2:
                p[p[i+3]] = p[p[i+1]] * p[p[i+2]];
                break;

            case 99:
                return;

            default:
                throw runtime_error("Invalid opcode");
        }
    }
}


int main(int argc, char **argv) {
    vector<int> intcode_program = parse_csv_ints(argv[1]);

    intcode_program[1] = 12;
    intcode_program[2] = 2;
    run_intcode_program(intcode_program);

    cout << "First position after running: " << intcode_program[0] << endl;

    return 0;
}
