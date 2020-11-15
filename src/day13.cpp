#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <cmath>
#include <numeric>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>


using namespace std;

using Value = int64_t;
using Address = int64_t;
using Text = vector<Value>;
using Heap = map<Value, Value>;


class Point {

    public:

    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {}
    Point() : x(0), y(0) {}

    // operator overloads
    bool operator==(const Point &) const;
    bool operator!=(const Point &) const;
    Point& operator+=(const Point &);
    Point operator+(const Point &) const;
    Point operator-(const Point &) const;
    Point operator/(const int) const;

    // vector logic
    Point direction_to(const Point &) const;

    friend ostream &operator<<(ostream &out, const Point &p) {
        out << '(' << p.x << ',' << p.y << ')';
        return out;
    }
};

Point Point::operator+(const Point &p) const {
    return Point{x + p.x, y + p.y};
}

Point Point::operator-(const Point &p) const {
    return Point{x - p.x, y - p.y};
}

Point Point::operator/(const int divisor) const {
    return Point{x / divisor, y / divisor};
}

Point& Point::operator+=(const Point &p) {
    x += p.x;
    y += p.y;
    return *this;
}

inline bool Point::operator==(const Point &p) const {
    return (x == p.x && y == p.y);
}

inline bool Point::operator!=(const Point &p) const {
    return !(*this == p);
}


Point Point::direction_to(const Point &b) const {
    Point diff{b - *this};
    int gcd_xy{gcd(diff.x, diff.y)};

    if (gcd_xy== 0) {
        return diff;
    }
    else {
        return Point{diff/gcd_xy};
    }
}

struct PointHasher
{
    size_t operator()(const Point& p) const {
        return ((hash<int>()(p.x) ^ (hash<int>()(p.y) << 1)) >> 1);
    }
};


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

auto cmp_point = [](const Point& a, const Point& b){
    return a.y < b.y || (a.y == b.y && a.x < b.x);
};
using Field = map<Point, char, decltype(cmp_point)>;


void print_field(const Field& field) {
    int row{}, col{};

    // go through the points sorted by row and col
    for (const auto &i: field) {
        // make variables human readable
        const Point &point = i.first;
        const char &object = i.second;

        // new line
        if (point.y > row) {
            cout << endl;
            row++;
            // we don't expect rows to be skipped
            assert(row == point.y);
            // carriage return
            col = 0;
        }

        assert(col == point.x);
        cout << object;
        col++;
    }
    cout << endl;
}


Value play_arkanoid(Text text) {
    IntcodeComputer computer{1, text};
    Value x, y, value, current_score;

    // map of points sorted by row and col
    Field field(cmp_point);

    // disable requirement to press enter to have input
    struct termios old_tio, new_tio;
    tcgetattr(STDIN_FILENO,&old_tio);
    new_tio=old_tio;
    new_tio.c_lflag &=(~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

    // computer should wait for input sometimes
    while (true) {
        computer.run();

        // gather three-outputs sequences
        while (computer.output_size()) {
            assert(computer.output_size() >= 3);
            x = computer.pop_output();
            y = computer.pop_output();
            value = computer.pop_output();

            // special coords for score
            if (x == -1 && y == 0) {
                current_score = value;
                cout << "Current score: " << current_score << endl;
            }
            // coords representing field state
            else {
                char c;
                switch (value) {
                    case 0: c = ' '; break; // space
                    case 1: c = '#'; break; // wall
                    case 2: c = '*'; break; // block
                    case 3: c = '@'; break; // paddle
                    case 4: c = 'o'; break; // ball
                    default: throw runtime_error("Invalid object");
                }

                field[Point{static_cast<int>(x), static_cast<int>(y)}] = c;
            }
        }

        // clear the screen, print field and score
        cout << "\033[2J\033[1;1H";
        print_field(field);
        cout << "Score: " << current_score << endl;

        // handle game completion
        if (computer.has_terminated()) {
            break;
        }
        // handle pad motion from user input
        else {
            unsigned char command;
            command = getchar();
            switch (command) {
                case 'h': computer.push_input(-1); break;
                case 'l': computer.push_input(1); break;
                default: computer.push_input(0); break; // otherwise stay
            }
        }
    }

    // restore terminal settings
    tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);

    return current_score;
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

    // Part 2
    // play for free
    text[0] = 2;
    Value final_score = play_arkanoid(text);
    cout << "Last score was " << final_score << ".\n";

    return 0;
}

