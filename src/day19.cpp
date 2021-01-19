#include <array>
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "point.hpp"
#include "intcode.hpp"


using namespace std;


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


Value get_point_value(const Value x, const Value y, Text &text) {
    auto computer = IntcodeComputer(0, text);
    computer.push_input(x); // x
    computer.push_input(y); // y
    computer.run();
    return computer.pop_output();
}


inline bool is_lower_left_corner(const Value side, const Value x, const Value y, Text &text) {
    return get_point_value(x, y, text) && get_point_value(x + side - 1, y - side + 1, text);
}


int main(int argc, char **argv)
{
    Text text = parse_csv_ints(argv[argc - 1]);

    // make sure the functions work
    assert(is_lower_left_corner(1, 0, 0, text) == true);
    assert(is_lower_left_corner(2, 0, 0, text) == false);
    assert(is_lower_left_corner(2, 11, 10, text) == true);
    assert(is_lower_left_corner(2, 10, 9, text) == false);
    assert(is_lower_left_corner(2, 11, 8, text) == false);
    assert(is_lower_left_corner(2, 0, 8, text) == false);
    assert(is_lower_left_corner(4, 33, 30, text) == true);
    assert(is_lower_left_corner(4, 32, 30, text) == false);

    // part 1
    {
        Value q_affected{};
        for (Value x{}; x<50; ++x)
            for (Value y{}; y<50; ++y)
                q_affected += get_point_value(x, y, text);

        cout << q_affected << " points affected\n";
    }

    // part 2
    {
        // skip second, third lines which don't have data
        Value x{3}, y{4}, side{100};

        // trace the lower edge of the beam until it fits the square
        while (!is_lower_left_corner(side, x, y, text)) {
            // cout << "Trying " << x << ',' << y << endl;
            ++y;
            while (!get_point_value(x, y, text))
                ++x;
        }
        // adjust y to yield the upper left corner
        y += (-side + 1);

        cout << "Closest square of " << side << 'x' << side << " is at " << x << ',' << y << endl;
        cout << "answer is " << x * 10000 + y << endl;
    }

    return 0;
}
