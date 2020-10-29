#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
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


class IntcodeProgram {

    public:
    IntcodeProgram(vector<int>, int, int);
    int run();

    private:
    inline uint8_t parse_opcode(int);
    int get_input();
    uint8_t parse_parameter_mode(int, int);
    int parse_parameter(int);

    // instruction pointer
    vector<int> text;
    size_t ip;
    queue<int> input;
    int output;
};

IntcodeProgram::IntcodeProgram(vector<int> text, int phase_setting, int input_signal) : text(text) {
    input.push(phase_setting);
    input.push(input_signal);
}

inline uint8_t IntcodeProgram::parse_opcode(int instruction) {
    return instruction % 100;
}

int IntcodeProgram::get_input() {
    if (input.empty()) {
        return 1;
    }
    else {
        int aux = input.front();
        input.pop();
        return aux;
    }
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

int IntcodeProgram::run() {
    bool output_ready = false;
    bool terminated = false;
    ip = 0;

    while (ip < text.size() && !terminated) {
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
                text[text[ip+1]] = get_input();
                ip += 2;
                break;

            case 4:
                // output
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
                terminated = true;
                break;

            default:
                throw runtime_error("Invalid operation code");
        }
    }
    assert(output_ready);
    // cout << "will return output " << output << endl;
    return output;
}


int get_thruster_signal(vector<int> &text, vector<int> &phases) {
    int signal = 0;
    for (auto phase_setting: phases) {
        // fresh intcode program
        IntcodeProgram p{text, phase_setting, signal};
        signal = p.run();
    }

    return signal;
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
    while(std::next_permutation(phases.begin(), phases.end()));

    // just in case
    assert(max_signal != -1);

    return max_signal;
}


int main(int argc, char **argv) {
    // parse program
    vector<int> text = parse_csv_ints(argv[1]);

    // purposedly set to first lexical permutation
    vector<int> phases{4, 3, 2, 1, 0};

    // test
    vector<int> test_text{3, 15, 3, 16, 1002, 16, 10, 16, 1, 16, 15, 15, 4, 15, 99, 0, 0};
    assert(get_max_thruster_signal(test_text, phases) == 43210);

    int max_thruster_signal = get_max_thruster_signal(text, phases);
    cout << "Max thruster signal is " << max_thruster_signal << endl;

    return 0;
}
