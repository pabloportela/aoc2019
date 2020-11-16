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
#include <thread>
#include <chrono>

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

auto cmp_point = [](const Point& a, const Point& b){
    return a.y < b.y || (a.y == b.y && a.x < b.x);
};

using Field = map<Point, char, decltype(cmp_point)>;


class Arkanoid {
    public:

    Arkanoid(Text &text) : computer(0, text), field(cmp_point) {}
    void auto_play();

    private:

    void process_computer_output();
    void refresh_screen();

    IntcodeComputer computer;
    Field field;
    size_t q_blocks{};
    Point ball_position;
    Point pad_position;
    Value current_score;
};


void Arkanoid::refresh_screen() {

    // some black magic to clear the screen
    cout << "\033[2J\033[1;1H";

    // go through the points sorted by row and col
    int row{}, col{};
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
    cout << "Amount of blocks: " << q_blocks << endl;
    cout << "Current score: " << current_score << endl;
}

void Arkanoid::process_computer_output() {
    Value x, y, output;

    while (computer.output_size()) {
        // gather three-outputs sequences
        assert(computer.output_size() >= 3);
        x = computer.pop_output();
        y = computer.pop_output();
        output = computer.pop_output();

        // special coords for score
        if (x == -1 && y == 0) {
            current_score = output;
        }
        // coords representing field state
        else {
            Point point{static_cast<int>(x), static_cast<int>(y)};
            char c;
            switch (output) {
                case 0: c = ' '; break; // space
                case 1: c = '#'; break; // wall

                // block
                case 2:
                    c = '*';
                    q_blocks++;
                    break;

                // paddle
                case 3:
                    c = '@';
                    pad_position = point;
                    break;

                // ball
                case 4:
                    c = 'o';
                    ball_position = point;
                    break; // ball

                default: throw runtime_error("Invalid object");
            }

            field[point] = c;
        }
    }
}

void Arkanoid::auto_play() {
    while (true) {
        // ball on intcode side
        computer.run();

        // analyze what she says
        process_computer_output();

        // tell it to the user
        refresh_screen();

        // delay so you can see it
        this_thread::sleep_for(chrono::milliseconds{10});

        // game over?
        if (computer.has_terminated())
            break;

        // move joystick to follow the ball
        if (pad_position.x < ball_position.x)
            computer.push_input(1);
        else if (pad_position.x > ball_position.x)
            computer.push_input(-1);
        else
            computer.push_input(0);
    }
}


int main(int argc, char **argv) {
    Text text = parse_csv_ints(argv[argc - 1]);

    // play for free
    text[0] = 2;

    Arkanoid arkanoid{text};
    arkanoid.auto_play();

    return 0;
}

