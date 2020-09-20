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


int get_gross_module_fuel_requiremet(const int mass) {
    return mass / 3 - 2;
}


int get_module_fuel_requirement(const int mass) {
    const auto gross = get_gross_module_fuel_requiremet(mass);
    return gross <= 0 ? 0 : gross + get_module_fuel_requirement(gross);
}


int get_total_fuel_requirements(const vector<string> &lines) {
    int total{};
    for (auto line: lines) {
        total += get_module_fuel_requirement(stoi(line));
    }

    return total;
}


int main(int argc, char **argv) {
    auto lines = get_lines(argv[1]);
    cout << "Sum of fuel requirements: " << get_total_fuel_requirements(lines) << "\n";

    return 0;
}
