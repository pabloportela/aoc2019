#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "point.hpp"
#include "intcode.hpp"


using namespace std;

using PositionSet = unordered_set<Point, PointHasher>;


PositionSet map_field(IntcodeComputer &computer) {
    // maps scaffolding to a set of points

    int x{}, y{};
    PositionSet field;

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

    return field;
}


inline bool is_intersection(const Point &position, const PositionSet &field) {
    return (
        field.count(position + Point{0,1}) &&
        field.count(position + Point{0,-1}) &&
        field.count(position + Point{1,0}) &&
        field.count(position + Point{-1,0})
    );
}

int compute_sum_of_alignment_parameters(const PositionSet &field) {
    int r{};
    for (const auto &p: field)
        if (is_intersection(p, field)) {
            r += p.x * p.y;
            cout << p << endl;
        }

    return r;
}

int main(int argc, char **argv) {
    auto computer = IntcodeComputer::from_file(0, argv[argc - 1]);
    PositionSet field = map_field(computer);

    // sum of alignment parameters
    int soap = compute_sum_of_alignment_parameters(field);

    cout << "soap: " << soap << endl;

    return 0;
}

