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
#include <numeric>
#include <unordered_map>
#include <sstream>


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


class IntcodeComputer {

    public:
    IntcodeComputer(int, Text&);
    void push_input(Value);
    Value run();
    Value pop_output();
    size_t output_size();
    bool has_terminated();

    private:
    inline int parse_opcode(int);
    int parse_parameter_mode(int, int);

    Value get_read_param(int);
    Address get_write_address(int);
    void log(const char *);

    int id;
    queue<Value> input;
    queue<Value> output;
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

inline size_t IntcodeComputer::output_size() {
    return output.size();
}

Value IntcodeComputer::pop_output() {
    Value aux = output.front();
    output.pop();
    return aux;
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

void IntcodeComputer::push_input(Value value) {
    input.push(value);
}

Value IntcodeComputer::run() {
    // local state to break out of the main loop
    bool runnable = true;


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
                // assert(output.empty());
                output.push(get_read_param(1));
                // cout << "output: " << output.front() << endl;
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

    assert(!output.empty());
    return output.front();
}

Text parse_csv_ints(const char *filename) {
    fstream file;
    file.open(filename);
    assert(file.is_open());

    string tp;
    Value value;
    Text integers;
    while (getline(file, tp, ',')) {
        stringstream ss(tp);
        ss >> value;
        integers.push_back(value);
    }
    file.close();

    return integers;
}


int main(int argc, char **argv) {
    Text text = parse_csv_ints(argv[argc - 1]);

    // Part 1
    IntcodeComputer computer1{1, text};
    computer1.run();

    size_t i{}, q_blocks{};
    while (computer1.output_size()) {
        Value output = computer1.pop_output();
        if ((i % 3) == 2 && output == 2)
            // offset is tile id and tile is block
            q_blocks++;
        i++;
    }
    cout << "There are " << q_blocks << " blocks.\n";

    return 0;
}
