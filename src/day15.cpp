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

using PositionSet = unordered_set<Point, PointHasher>;


// evil global variables
static const array<Point, 4> directions{Point{1, 0}, Point{0, 1}, Point{-1, 0}, Point{0, -1}};
PositionSet known_walls;


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

size_t get_shortest_path_size(PositionSet visited, const IntcodeComputer &computer, const Point &position, size_t acc) {
    // recursively try all four directions from position in the computer
    // keep track of visited positions and accumulated movement count

    size_t min_so_far{numeric_limits<size_t>::max()};

    // try all four directions
    for (const auto &direction: directions) {
        const auto landing_position = position + direction;
        size_t path_size;

        // visited, or wall don't go further
        if (visited.count(landing_position) || known_walls.count(landing_position))
            continue;

        // not visited, copy computer and see what she says
        auto computer_copy = computer;
        switch (try_direction(computer_copy, direction)) {
            // wall, don't go further
            case 0:
                known_walls.insert(landing_position);
                continue;

            // unvisited empty space
            case 1:
                // mark as visited
                visited.insert(position + direction);

                // recurse
                path_size = get_shortest_path_size(visited, computer_copy, landing_position, acc + 1);

                // is it the shortest?
                if (path_size < min_so_far)
                    min_so_far = path_size;
                break;

            // found the tank
            case 2:
                return acc;
        }
    }

    return min_so_far;
}


int main(int argc, char **argv) {
    Text text = parse_csv_ints(argv[argc - 1]);

    // state to be passed as mutable copy along the recursion
    IntcodeComputer computer{0, text};
    PositionSet visited_positions;

    // call recursive function
    size_t shortest_path_size = get_shortest_path_size(visited_positions, computer, Point{0, 0}, 1);
    assert(shortest_path_size != numeric_limits<size_t>::max());

    cout << "Shortest path to tank requires " << shortest_path_size << " movements." << endl;

    return 0;
}

