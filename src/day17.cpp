#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_set>

#include "point.hpp"
#include "intcode.hpp"


using namespace std;

using PositionSet = unordered_set<Point, PointHasher>;

class Scaffold {

    public:
    Scaffold(IntcodeComputer &);
    int compute_sum_of_alignment_parameters();

    private:
    inline bool is_intersection(const Point &);
    void map_scaffold(IntcodeComputer &);

    PositionSet field;
    Point current_position;
    Point current_direction;
};

Scaffold::Scaffold(IntcodeComputer &computer) {
    map_scaffold(computer);
}

void Scaffold::map_scaffold(IntcodeComputer &computer) {
    // maps scaffolding to a set of points

    int x{}, y{};

    while (!computer.has_terminated()) {
        computer.run();
        while (computer.output_size()) {
            char c = static_cast<char>(computer.pop_output());
            cout << c;
            if (c == '\n') {
                ++y;
                x = 0;
            }
            else {
                if (c != '.')
                    // v, ^, <, >, #
                    field.insert(Point{x, y});
                ++x;
            }
        }
    }
}


inline bool Scaffold::is_intersection(const Point &position) {
    return (
        field.count(position + Point{0,1}) &&
        field.count(position + Point{0,-1}) &&
        field.count(position + Point{1,0}) &&
        field.count(position + Point{-1,0})
    );
}

int Scaffold::compute_sum_of_alignment_parameters() {
    int r{};
    for (const auto &p: field)
        if (is_intersection(p)) {
            r += p.x * p.y;
            cout << p << endl;
        }

    return r;
}

int main(int argc, char **argv) {
    auto computer = IntcodeComputer::from_file(0, argv[argc - 1]);

    Scaffold scaffold{computer};

    // sum of alignment parameters
    int soap = scaffold.compute_sum_of_alignment_parameters();

    cout << "soap: " << soap << endl;

    return 0;
}
