#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <utility>
#include <queue>
#include <string>
#include <vector>
#include <cmath>


using namespace std;

using Value = int64_t;
using Address = int64_t;
using Text = vector<Value>;
using Heap = map<Value, Value>;



class Memory {

    public:
    Memory(Text& text);
    Value get(Value);
    void set(Address, Value);
    void print();

    private:
    Text text;
    Heap heap;
};

Memory::Memory(Text &text) {
    this->text = text;
}

void Memory::print() {
    cout << "Text" << endl;
    for (auto i: text) {
        cout << i << " ";
    }
    cout << endl;
    cout << "Heap" << endl;
    for (auto &[k, v]: heap) {
        cout << k << " : " << v << endl;;
    }
}

void Memory::set(Address address, Value value) {
    assert(address >= 0);
    if (static_cast<size_t>(address) < text.size()) {
        // cout << " to the text\n";
        text[address] = value;
    }
    else {
        // cout << " to the heap!\n";
        heap[address] = value;
    }
}

Value Memory::get(Address address) {
    assert(address >= 0);
    if (static_cast<size_t>(address) < text.size()) {
        return text.at(address);
    }
    else {
        return heap[address];
    }
}


Text parse_csv_ints(const char *filename) {
    fstream file;
    file.open(filename);
    assert(file.is_open());

    string tp;
    Text integers;
    while (getline(file, tp, ',')) {
        integers.push_back(stoi(tp));
    }
    file.close();

    return integers;
}


class IntcodeComputer {

    public:
    IntcodeComputer(int, Text&);
    Value run(Value);
    Value get_last_run_output();
    bool has_terminated();

    private:
    inline int parse_opcode(int);
    int parse_parameter_mode(int, int);

    Value get_read_param(int);
    Address get_write_address(int);
    void log(const char *);

    int id;
    queue<Value> input;
    Value output;
    bool terminated;

    Memory memory;
    Address ip;
    Address relative_base;
};

IntcodeComputer::IntcodeComputer(int id, Text &text) : id(id), memory(text) {
    relative_base = 0;
    ip = 0;
    terminated = false;
}

inline bool IntcodeComputer::has_terminated() {
    return terminated;
}

inline int IntcodeComputer::parse_opcode(int instruction) {
    return instruction % 100;
}

Value IntcodeComputer::get_last_run_output() {
    return output;
}

int IntcodeComputer::parse_parameter_mode(int instruction, int parameter_offset) {
    assert(parameter_offset <= 3);
    uint32_t divisor = pow(10, parameter_offset + 1);
    uint32_t  modulo = divisor * 10;

    return (instruction % modulo) / divisor;
}

Value IntcodeComputer::get_read_param(int offset) {
    int parameter_mode = parse_parameter_mode(memory.get(ip), offset);

    // position
    if (parameter_mode == 0)
        return memory.get(memory.get(ip+offset));

    // immediate
    if (parameter_mode == 1)
        return memory.get(ip+offset);

    // relative base
    if (parameter_mode == 2)
        return memory.get(memory.get(ip+offset) + relative_base);

    throw runtime_error("Invalid parameter mode");
}

Address IntcodeComputer::get_write_address(int offset) {
    int parameter_mode = parse_parameter_mode(memory.get(ip), offset);

    // position
    if (parameter_mode == 0) {
        return memory.get(ip+offset);
    }

    // relative base
    if (parameter_mode == 2) {
        return memory.get(ip+offset) + relative_base;
    }

    throw runtime_error("Invalid write address mode");
}

void IntcodeComputer::log(const char *msg) {
    cout << "#" << id << " at ip [" << ip << "] : " << msg << endl;
}

Value IntcodeComputer::run(Value input_signal) {
    // control so we don't overwrite ourselves
    bool output_ready = false;

    // local state to break out of the main loop
    bool runnable = true;

    input.push(input_signal);

    // main loop
    // memory.address_in_text(ip) &&
    while (runnable) {

        Value opcode = parse_opcode(memory.get(ip));
        switch (opcode) {
            case 1:
                // addition
                memory.set(get_write_address(3), get_read_param(1) + get_read_param(2));
                ip += 4;
                break;

            case 2:
                // multiplication
                memory.set(get_write_address(3), get_read_param(1) * get_read_param(2));
                ip += 4;
                break;

            case 3:
                // input
                if (input.empty()) {
                    // break out of the loop but keep the ip untouched so it can be resumed
                    runnable = false;
                }
                else {
                    memory.set(get_write_address(1), input.front());
                    input.pop();
                    ip += 2;
                }
                break;

            case 4:
                // output
                // assert(!output_ready);
                output = get_read_param(1);
                cout << "output: " << output << endl;
                output_ready = true;
                ip += 2;
                break;

            case 5:
                // if first param is non-zero, second to ip
                if (get_read_param(1))
                    ip = static_cast<Address>(get_read_param(2));
                else
                    ip += 3;
                break;

            case 6:
                // if first param is zero, second to ip
                if (!get_read_param(1))
                    ip = static_cast<Address>(get_read_param(2));
                else
                    ip += 3;
                break;

            case 7:
                // if first less than second, 1 to third, otherwise 0
                if (get_read_param(1) < get_read_param(2))
                    memory.set(get_write_address(3), 1);
                else
                    memory.set(get_write_address(3), 0);
                ip += 4;
                break;

            case 8:
                // if first equals second, 1 to third, otherwise 0
                if (get_read_param(1) == get_read_param(2))
                    memory.set(get_write_address(3), 1);
                else
                    memory.set(get_write_address(3), 0);
                ip += 4;
                break;

            case 9:
                // add / substract from relative base
                relative_base += get_read_param(1);
                ip += 2;
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



void test() {
    Text text1{109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99};
    Text text2{1102,34915192,34915192,7,4,7,99,0};
    Text text3{104,1125899906842624,99};

    IntcodeComputer computer{0, text1};
    computer.run(1);

    cout << "Passed the computer tests!\n";
}


int main(int argc, char **argv) {
    // test
    // test();

    // parse program
    Text text = parse_csv_ints(argv[argc - 1]);

    IntcodeComputer computer1{0, text};
    computer1.run(1);

    IntcodeComputer computer2{0, text};
    computer2.run(2);

    return 0;
}
