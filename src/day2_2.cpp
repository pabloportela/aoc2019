#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


vector<int> parse_csv_ints(const char *filename) {
    fstream file;
    file.open(filename);
    if (!file.is_open()) {
        throw runtime_error(string("Unable to open file ") + string(filename));
    }

    string tp;
    vector<int> integers;
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


int get_answer(vector<int> &p) {
    for (int noun{}; noun<100; noun++) {
        for (int verb{}; verb<100; verb++) {
            // reset the memory
            vector<int> p_copy = p;

            // try noun and verb
            p_copy[1] = noun;
            p_copy[2] = verb;
            run_intcode_program(p_copy);

            // found it
            if (p_copy[0] == 19690720) {
                return 100 * noun + verb;
            }
        }
    }

    throw runtime_error("Unable to find the answer");
}


int main(int argc, char **argv) {
    vector<int> intcode_program = parse_csv_ints(argv[1]);
    cout << "And the winner is: " << get_answer(intcode_program) << endl;
    return 0;
}
