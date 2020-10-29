#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <utility>
#include <queue>
#include <string>
#include <vector>

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


class IntcodeProgram {

    public:
    IntcodeProgram(int, vector<int>, int);
    int run(int);
    int get_last_run_output();
    bool has_terminated();

    private:
    inline uint8_t parse_opcode(int);
    uint8_t parse_parameter_mode(int, int);
    int parse_parameter(int);
    void log(const char *);

    int id;
    size_t ip;
    vector<int> text;
    queue<int> input;
    int output;
    bool terminated;
};

IntcodeProgram::IntcodeProgram(int id, vector<int> text, int phase_setting) : id(id) {
    input.push(phase_setting);
    this->text = text;
    ip = 0;
    terminated = false;
}

inline bool IntcodeProgram::has_terminated() {
    return terminated;
}

inline uint8_t IntcodeProgram::parse_opcode(int instruction) {
    return instruction % 100;
}

int IntcodeProgram::get_last_run_output() {
    return output;
}

uint8_t IntcodeProgram::parse_parameter_mode(int instruction, int arg_offset) {
    if (arg_offset == 1)
        // first argument, look for the hundred value
        return (instruction % 1000) / 100;

    if (arg_offset == 2)
        // second argument, look for the thousand value
        return (instruction % 10000) / 1000;

    throw runtime_error("Invalid argument offset");
}

int IntcodeProgram::parse_parameter(int parameter_offset) {
    uint8_t parameter_mode = parse_parameter_mode(text[ip], parameter_offset);

    if (parameter_mode == 0)
        // position
        return text[text[ip+parameter_offset]];

    if (parameter_mode == 1)
        // immediate
        return text[ip+parameter_offset];

    throw runtime_error("Invalid parameter mode");
}


void IntcodeProgram::log(const char *msg) {
    cout << "#" << id << " at ip [" << ip << "] : " << msg << endl;
}


int IntcodeProgram::run(int input_signal) {
    // control so we don't overwrite ourselves
    bool output_ready = false;

    // local state to break out of the main loop
    bool runnable = true;

    input.push(input_signal);

    // main loop
    while (ip < text.size() && runnable) {
        switch (parse_opcode(text[ip])) {
            case 1:
                // addition
                text[text[ip+3]] = parse_parameter(1) + parse_parameter(2);
                ip += 4;
                break;

            case 2:
                // multiplication
                text[text[ip+3]] = parse_parameter(1) * parse_parameter(2);
                ip += 4;
                break;

            case 3:
                // input
                if (input.empty()) {
                    // break out of the loop but keep the ip untouched so it can be resumed
                    runnable = false;
                }
                else {
                    text[text[ip+1]] = input.front();
                    input.pop();
                    ip += 2;
                }
                break;

            case 4:
                // output
                assert(!output_ready);
                output = text[text[ip+1]];
                output_ready = true;
                ip += 2;
                break;

            case 5:
                // if first param is non-zero, second to ip
                if (parse_parameter(1))
                    ip = parse_parameter(2);
                else
                    ip += 3;
                break;

            case 6:
                // if first param equals zero, second to ip
                if (!parse_parameter(1))
                    ip = parse_parameter(2);
                else
                    ip += 3;
                break;

            case 7:
                // if first less than second, 1 to third, otherwise 0
                if (parse_parameter(1) < parse_parameter(2))
                    text[text[ip+3]] = 1;
                else
                    text[text[ip+3]] = 0;
                ip += 4;
                break;

            case 8:
                // if first equals second, 1 to third, otherwise 0
                if (parse_parameter(1) == parse_parameter(2))
                    text[text[ip+3]] = 1;
                else
                    text[text[ip+3]] = 0;
                ip += 4;
                break;

            case 99:
                // graceful exit
                runnable = false;
                terminated = true;
                break;

            default:
                terminated = true;
                throw runtime_error("Invalid operation code");
        }
    }

    assert(output_ready);
    return output;
}


int get_thruster_signal(vector<int> &text, vector<int> &phases) {
    vector<IntcodeProgram> programs;
    for (int i{}; i<5; i++) {
        // each program with its own text copy
        programs.emplace_back(i, text, phases[i]);
    }

    int i{}, signal{};

    // do round-robin until a program halts
    do {
        signal = programs[i % 5].run(signal);
        i++;
    }
    while (!programs[4].has_terminated());

    // return last output of 5th program
    return programs[4].get_last_run_output();
}


int get_max_thruster_signal(vector<int> &text, vector<int> &phases) {
    // as I already know some sample signals are above this
    int signal;
    int max_signal = -1;

    // take vector to the first lexicographical permutation
    sort(phases.begin(), phases.end());

    // try all 120 phase permutations
    do {
        signal = get_thruster_signal(text, phases);
        if (signal > max_signal)
            max_signal = signal;
    }
    while (std::next_permutation(phases.begin(), phases.end()));

    // just in case
    assert(max_signal != -1);

    return max_signal;
}


void test(){
    vector<int> test_phase{9, 8, 7, 6, 5};

    vector<int> test_text1{3, 26, 1001, 26, -4, 26, 3, 27, 1002, 27, 2, 27, 1, 27, 26, 27, 4, 27, 1001, 28, -1, 28, 1005, 28, 6, 99, 0, 0, 5};
    assert(get_max_thruster_signal(test_text1, test_phase) == 139629729);

    vector<int> test_text2{3, 52, 1001, 52, -5, 52, 3, 53, 1, 52, 56, 54, 1007, 54, 5, 55, 1005, 55, 26, 1001, 54,  -5, 54, 1105, 1, 12, 1, 53, 54, 53, 1008, 54, 0, 55, 1001, 55, 1, 55, 2, 53, 55, 53, 4,  53, 1001, 56, -1, 56, 1005, 56, 6, 99, 0, 0, 0, 0, 10};
    assert(get_max_thruster_signal(test_text2, test_phase) == 18216);

    cout << "Passed the tests!\n";
}


int main(int argc, char **argv) {
    // parse program
    vector<int> text = parse_csv_ints(argv[1]);

    // test
    test();

    vector<int> phases{5, 6, 7, 8, 9};
    int max_thruster_signal = get_max_thruster_signal(text, phases);
    cout << "Max thruster signal is " << max_thruster_signal << endl;

    return 0;
}
