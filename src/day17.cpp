#include <array>
#include <cassert>
#include <cmath>
#include <cctype>
#include <iostream>
#include <sstream>
#include <fstream>
#include <numeric>
#include <string>
#include <unordered_set>

#include "point.hpp"
#include "intcode.hpp"


using namespace std;

using PositionSet = unordered_set<Point, PointHasher>;
using Routine = string;

class Scaffold {

    public:
    Scaffold(IntcodeComputer &);
    int compute_sum_of_alignment_parameters();
    void generate_routines();
    size_t compressed_length(const Routine &) const;
    Routine compress(const Routine &) const;
    Routine MAIN;
    Routine A;
    Routine B;
    Routine C;
    Routine roadmap;

    private:
    void map_scaffold(IntcodeComputer &);
    inline bool is_intersection(const Point &);
    void build_roadmap();
    bool generate_main();
    bool turn();
    void forward();
    bool fit_b();
    void print() const;

    PositionSet field;
    Point current_position;
    Point current_direction;
    size_t roadmap_len;
};

Scaffold::Scaffold(IntcodeComputer &computer) {
    map_scaffold(computer);
    build_roadmap();
}

bool Scaffold::turn()
{
    // try left
    Point new_direction = current_direction.left();
    if (field.count(current_position + new_direction)) {
        current_direction = new_direction;
        roadmap.push_back('L');
        return true;
    }

    // try right
    new_direction = current_direction.right();
    if (field.count(current_position + new_direction)) {
        current_direction = new_direction;
        roadmap.push_back('R');
        return true;
    }

    return false;
}

void Scaffold::build_roadmap() {
    // build roadmap
    while (turn())
        forward();

    roadmap_len = roadmap.length();
}

size_t Scaffold::compressed_length(const Routine &r) const {
    return compress(r).length();
}

Routine Scaffold::compress(const Routine &r) const {
    size_t acc{};
    Routine compressed{};

    for (size_t i{}; i<r.length(); ++i) {
        if (r[i] == 'F')
            ++acc;
        else {
            // flush F sequence
            if (acc) {
                std::ostringstream oss;
                oss << acc;
                compressed.append(oss.str());
                acc = 0;
            }
            compressed.push_back(r[i]);
        }
    }

    // flush F sequence
    if (acc) {
        std::ostringstream oss;
        oss << acc;
        compressed.append(oss.str());
    }

    return compressed;
}

bool Scaffold::generate_main() {
    MAIN.clear();
    size_t i{};

    while (i<roadmap_len) {
         if (MAIN.size() > 10)
             return false;

        // try to fit A
        if (roadmap.compare(i, A.length(), A) == 0) {
            MAIN.push_back('A');
            i += A.length();
        }
        // try to fit B
        else if (roadmap.compare(i, B.length(), B) == 0) {
            MAIN.push_back('B');
            i += B.length();
        }
        // try to fit C
        else if (roadmap.compare(i, C.length(), C) == 0) {
            MAIN.push_back('C');
            i += C.length();
        }
        else
            return false;
    }

    return true;
}


bool Scaffold::fit_b() {
    size_t i{}, begin{};

    // apply A or C from beginning
    i = 0;
    while (i<roadmap_len) {
        // try to fit A
        if (roadmap.compare(i, A.length(), A) == 0) {
            i += A.length();
        }
        // try to fit C
        else if (roadmap.compare(i, C.length(), C) == 0) {
            i += C.length();
        }
        else
            break;
    }

    begin = i;
    assert(begin);

    i = 1;
    do {
        B = roadmap.substr(begin, i);
        ++i;

        if (generate_main())
            return true;
    }
    while (compressed_length(B) < 11);

    return false;
}

void Scaffold::print() const {
    cout << "\n---------\n";
    cout << "MAIN: " << MAIN << endl;
    cout << "A: " << compress(A) << endl;
    cout << "B: " << compress(B) << endl;
    cout << "C: " << compress(C) << endl;
}


void Scaffold::generate_routines() {
    // we assume the main routine begins with A, but don't know where it finishes
    // we assume the main routine ends with C, but don't know where it starts
    // we know their lengths are not longer than 10

    size_t i{1}, j{2};

    do {
        A = roadmap.substr(0,i);
        do {
            C = roadmap.substr(roadmap_len-j);
            // now try to come up with B
            if (fit_b()) {
                cout << "Found it! :)\n";
                return;
            }
            ++j;
        }
        while (compressed_length(C) < 11);
        ++i;
    }
    while (compressed_length(A) < 11);
}

void Scaffold::forward()
{
    int i{};
    while (field.count(current_position + current_direction)) {
        current_position += current_direction;
        ++i;
        roadmap.push_back('F');
    }
    // cout << i;
    assert(i);
}

void Scaffold::map_scaffold(IntcodeComputer &computer)
{
    // maps scaffolding to a set of points with side-effects on the computer

    int x{}, y{};

    while (!computer.has_terminated()) {
        computer.run();
        while (computer.output_size()) {
            Value v = computer.pop_output();
            char c = static_cast<char>(v);

            cout << c;

            // end of line
            if (c == '\n') {
                --y;
                x = 0;
            }
            // empty space
            else if (c == '.') {
                ++x;
            }
            // scaffold
            else {
                Point p{x, y};
                ++x;
                field.insert(p);

                // ^, >, v or <
                if (c == '^') {
                    current_direction = Point(0,1);
                    current_position = p;
                }
                else if (c == '>') {
                    current_direction = Point(1,0);
                    current_position = p;
                }
                else if (c == 'v') {
                    current_direction = Point(0,-1);
                    current_position = p;
                }
                else if (c == '<') {
                    current_direction = Point(-1,0);
                    current_position = p;
                }
                else
                    cout << "Dust " << v << endl;
            }
        }
    }
}


inline bool Scaffold::is_intersection(const Point &position)
{
    return (
        field.count(position + Point{0,1}) &&
        field.count(position + Point{0,-1}) &&
        field.count(position + Point{1,0}) &&
        field.count(position + Point{-1,0})
    );
}

int Scaffold::compute_sum_of_alignment_parameters()
{
    int r{};
    for (const auto &p: field)
        if (is_intersection(p))
            r += p.x * abs(p.y);

    return r;
}

void enter_routine(IntcodeComputer &computer, const Routine &routine)
{
    for (size_t i{}; i<routine.size(); ++i)
        computer.push_input(routine[i]);

    computer.push_input(10); // \n
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

int main(int argc, char **argv)
{
    Text text = parse_csv_ints(argv[argc - 1]);
    text[0] = 2;
    auto computer = IntcodeComputer(0, text);
    enter_routine(computer, "A,B,B,C,C,A,A,B,B,C");
    enter_routine(computer, "L,12,R,4,R,4");
    enter_routine(computer, "R,12,R,4,L,12");
    enter_routine(computer, "R,12,R,4,L,6,L,8,L,8");
    enter_routine(computer, "n");

    Scaffold scaffold{computer};

    while (computer.output_size())
        cout << int(computer.pop_output()) << endl;

    return 0;
}
