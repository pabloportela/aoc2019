#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


vector<string> get_lines(const char *filename) {
    fstream file;
    file.open(filename);
    string tp;
    vector<string> lines;
    while (getline(file, tp)) {
        lines.push_back(tp);
    }
    file.close();

    return lines;
}


int get_module_fuel_requirement(const int mass) {
    return mass / 3 - 2;
}


int get_total_fuel_requirements(const vector<string> &lines) {
    int total{};
    for (auto line: lines) {
        total += get_module_fuel_requirement(stoi(line));
    }

    return total;
}


int main() {
    auto lines = get_lines("day1_input.txt");
    cout << "Sum of fuel requirements: " << get_total_fuel_requirements(lines) << "\n";

    return 0;
}
