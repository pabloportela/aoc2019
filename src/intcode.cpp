#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "intcode.hpp"


using namespace std;


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


IntcodeComputer::IntcodeComputer(int id, Text &text) : id(id), memory(text) {
    relative_base = 0;
    ip = 0;
    terminated = false;
}

bool IntcodeComputer::has_terminated() {
    return terminated;
}

inline int IntcodeComputer::parse_opcode(int instruction) {
    return instruction % 100;
}

size_t IntcodeComputer::output_size() {
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

    return output.front();
}

