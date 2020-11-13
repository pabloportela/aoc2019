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
    bool operator<(const Point &) const;

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
    Value run(Value);
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

Value IntcodeComputer::run(Value input_signal) {
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


class HullBrush {

    public:

    HullBrush() : current_position({0, 0}), current_direction({0,1}) {}
    void step();
    void print();
    void paint(const bool);
    void turn(const bool);
    bool is_current_position_white() const;
    size_t count_painted_positions() const;

    private:

    unordered_map<Point, bool, PointHasher> painted_positions;
    Point current_position;
    Point current_direction;
};


void HullBrush::print() {
    const int width{5};
    const int height{5};
    for (int y{height}; y>-height; y--) {
        for (int x{-width}; x<=width; x++) {
            if (current_position == Point{x, y}) {
                if (current_direction == Point{0,1}) cout << '^';
                else if (current_direction == Point{0,-1}) cout << 'v';
                else if (current_direction == Point{-1,0}) cout << '<';
                else if (current_direction == Point{1,0}) cout << '>';
                else throw runtime_error("Invalid direction");
            }
            else {
                cout << (painted_positions.count(Point{x, y}) > 0 ? '#' : '.');
            }
        }
        cout << endl;
    }
}

inline void HullBrush::paint(const bool color) {
    painted_positions[current_position] = color;
}

inline void HullBrush::step() {
    current_position += current_direction;
}

void HullBrush::turn(const bool right) {
    // previously heading north
    if (current_direction == Point{0,1})
        current_direction = right ? Point{1,0} : Point{-1,0};

    // south
    else if (current_direction == Point{0,-1})
        current_direction = right ? Point{-1,0} : Point{1,0};

    // west
    else if (current_direction == Point{-1,0})
        current_direction = right ? Point{0,1} : Point{0,-1};

    // east
    else if (current_direction == Point{1,0})
        current_direction = right ? Point{0,-1} : Point{0,1};

    else
        throw runtime_error("invalid direction");
}

bool HullBrush::is_current_position_white() const {
    return painted_positions.count(current_position) && painted_positions.at(current_position) == true;
}

size_t HullBrush::count_painted_positions() const {
    return painted_positions.size();
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
    IntcodeComputer computer{0, text};
    HullBrush brush;

    // all black
    while (!computer.has_terminated()) {
        computer.run(brush.is_current_position_white() ? 1 : 0);
        assert(computer.output_size() >= 2);
        brush.paint(computer.pop_output() == 1);
        brush.turn(computer.pop_output() == 1);
        brush.step();
    }

    cout << "Our robot painted " << brush.count_painted_positions() << " panels" << endl;

    return 0;
}
