#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

using namespace std;


vector<int> parse_csv_ints(const char *filename) {
    fstream file;
    file.open(filename);
    assert(file.is_open());

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


inline uint8_t parse_opcode(int instruction) {
    return instruction % 100;
}

constexpr inline int get_input() {
    return 5;
}

uint8_t parse_parameter_mode(int instruction, int arg_offset) {
    if (arg_offset == 1)
        // first argument, look for the hundred value
        return (instruction % 1000) / 100;

    if (arg_offset == 2)
        // second argument, look for the thousand value
        return (instruction % 10000) / 1000;

    throw runtime_error("Invalid argument offset");
}

int parse_parameter(vector<int> &p, int ip, int parameter_offset) {
    uint8_t parameter_mode = parse_parameter_mode(p[ip], parameter_offset);

    if (parameter_mode == 0)
        // position mode
        return p[p[ip+parameter_offset]];

    if (parameter_mode == 1)
        // immediate mode
        return p[ip+parameter_offset];

    throw runtime_error("Invalid parameter mode");
}

void run_intcode_program(vector<int> &p) {

    // printf("at 225: %d\n", p[225]);

    // instruction pointer
    size_t ip{};

    while (ip < p.size()) {

        switch (parse_opcode(p[ip])) {
            case 1:
                // addition
                p[p[ip+3]] = parse_parameter(p, ip, 1) + parse_parameter(p, ip, 2);
                ip += 4;
                break;

            case 2:
                // multiplication
                p[p[ip+3]] = parse_parameter(p, ip, 1) * parse_parameter(p, ip, 2);
                ip += 4;
                break;

            case 3:
                // input
                p[p[ip+1]] = get_input();
                ip += 2;
                break;

            case 4:
                // output
                printf("output: %d\n", p[p[ip+1]]);
                ip += 2;
                break;

            case 5:
                // if first param is non-zero, second to ip
                if (parse_parameter(p, ip, 1))
                    ip = parse_parameter(p, ip, 2);
                else
                    ip += 3;
                break;

            case 6:
                // if first param equals zero, second to ip
                if (!parse_parameter(p, ip, 1))
                    ip = parse_parameter(p, ip, 2);
                else
                    ip += 3;
                break;

            case 7:
                // if first less than second, 1 to third, otherwise 0
                if (parse_parameter(p, ip, 1) < parse_parameter(p, ip, 2))
                    p[p[ip+3]] = 1;
                else
                    p[p[ip+3]] = 0;
                ip += 4;
                break;

            case 8:
                // if first less than second, 1 to third, otherwise 0
                if (parse_parameter(p, ip, 1) == parse_parameter(p, ip, 2))
                    p[p[ip+3]] = 1;
                else
                    p[p[ip+3]] = 0;
                ip += 4;
                break;

            case 99:
                // graceful exit
                return;

            default:
                throw runtime_error("Invalid operation code");
        }
    }
}

void test() {
    vector<int> test_program{1, 4, 1, 0, 101, -1, 2, 0, 1001, 3, -45, 5, 1102, 7, 140, 5};
    assert(parse_parameter(test_program, 0, 1) == 101);
    assert(parse_parameter(test_program, 0, 2) == 4);
    assert(parse_parameter(test_program, 4, 1) == -1);
    assert(parse_parameter(test_program, 4, 2) == 1);
    assert(parse_parameter(test_program, 8, 1) == 0);
    assert(parse_parameter(test_program, 8, 2) == -45);
    assert(parse_parameter(test_program, 12, 1) == 7);
    assert(parse_parameter(test_program, 12, 2) == 140);
    printf("Tests ran successfuly\n");
}

int main(int argc, char **argv) {
    test();
    vector<int> program = parse_csv_ints(argv[1]);
    run_intcode_program(program);

    return 0;
}
