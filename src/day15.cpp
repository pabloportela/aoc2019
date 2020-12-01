#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <array>
#include <cmath>
#include <numeric>

#include "point.hpp"
#include "intcode.hpp"


using namespace std;

using PositionMap = unordered_map<Point, char, PointHasher>;
using PositionSet = unordered_set<Point, PointHasher>;


static const array<Point, 4> directions{Point{1, 0}, Point{0, 1}, Point{-1, 0}, Point{0, -1}};

// evil global variables
PositionMap section;
Point tank_position;
size_t distance_to_tank;
size_t minutes_to_spread;


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


Value get_direction_code(const Point &point) {
    if (point == Point{0,1}) return 1;
    if (point == Point{-1,0}) return 3;
    if (point == Point{0,-1}) return 2;
    if (point == Point{1,0}) return 4;
    throw runtime_error("Invalid direction");
};


Value try_direction(IntcodeComputer &computer, const Point &direction) {
    // try movement in a copy of the computer
    computer.push_input(get_direction_code(direction));
    computer.run();
    assert(computer.output_size() == 1);
    return computer.pop_output();
}

void map_section(const IntcodeComputer &computer, const Point &position) {
    // recursively try all four directions from position in the computer, without repeating positions
    // keep track of positions in section and tank as a desirable side-effect

    // try all four directions
    for (const auto &direction: directions) {
        const auto landing_position = position + direction;

        // visited, don't go further
        if (section.count(landing_position))
            continue;

        // not visited, copy computer and see what she says
        auto computer_copy = computer;
        switch (try_direction(computer_copy, direction)) {
            // wall, don't go further
            case 0:
                section[landing_position] = '#';
                continue;

            // found the tank
            case 2:
                tank_position = landing_position;
                // spillover to case 1

            case 1:
                // mark as visited (yes, also the tank)
                section[landing_position] = '.';

                // recurse
                map_section(computer_copy, landing_position);
        }
    }
}


void compute_movements_and_minutes() {
    PositionSet expanded;
    PositionSet to_expand{tank_position};
    size_t i{};

    while (!to_expand.empty()) {
        PositionSet to_expand_copy{to_expand};
        for (const auto position: to_expand_copy) {
            // don't come here again
            expanded.insert(position);
            to_expand.erase(position);

            // check for distance to tank (note we are going from tank to origin)
            if (position == Point{})
                distance_to_tank = i;

            // add adjacent, unexpanded positions to expand set
            for (const auto &direction: directions) {
                const Point landing_position{position + direction};
                if (!expanded.count(landing_position) && section.at(position) == '.')
                    to_expand.insert(landing_position);
            }
        }
        ++i;
    }

    minutes_to_spread = i-1;
}

int main(int argc, char **argv) {
    Text text = parse_csv_ints(argv[argc - 1]);

    // state to be passed as mutable copy along the recursion
    IntcodeComputer computer{0, text};

    // map all positions
    map_section(computer, Point{});

    // calculate what we're asked for
    compute_movements_and_minutes();

    cout << "Tank is in " << tank_position << endl;
    cout << "Distance is " << distance_to_tank << endl;
    cout << "Minutes to spread " << minutes_to_spread << endl;

    return 0;
}

