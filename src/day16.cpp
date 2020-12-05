#include <iostream>
#include <cstdlib>
#include <array>
#include <vector>

#define PATTERN_LENGTH 4

using namespace std;
using Digits = vector<int>;

const array<int, PATTERN_LENGTH> base_pattern{0, 1, 0, -1};


void print(const Digits &v) {
    for (const auto &i: v)
        cout << i;
    cout << endl;
}


// with 2 if pattern is 0, 0, 1, 1, 0, 0, -1, -1
// term goes from 1 to 8

inline int get_pattern(const int width, const int term) {
    return base_pattern[(term / width) % PATTERN_LENGTH];
}


int process_digit(const Digits &input, const size_t digit_offset) {
    int result{};
    int pattern_offset{};
    for (const auto &digit: input) {
        result += digit * get_pattern(digit_offset + 1, pattern_offset + 1);
        ++pattern_offset;
    }

    return abs(result % 10);
}


Digits phase(const Digits &input) {
    Digits output{input};
    for (size_t i{}; i<input.size(); i++)
       output[i] = process_digit(input, i);

    return output;
}


Digits fft(const Digits &digits, const size_t q_phases) {
    Digits result{digits};
    for (size_t i{}; i<q_phases; i++)
        result = phase(result);

    return result;
}


int main() {
    // Digits input{1,2,3,4,5,6,7,8};
    // Digits input{8,0,8,7,1,2,2,4,5,8,5,9,1,4,5,4,6,6,1,9,0,8,3,2,1,8,6,4,5,5,9,5};
    // Digits input{1,9,6,1,7,8,0,4,2,0,7,2,0,2,2,0,9,1,4,4,9,1,6,0,4,4,1,8,9,9,1,7};
    Digits input{5,9,7,1,3,1,3,7,2,6,9,8,0,1,0,9,9,6,3,2,6,5,4,1,8,1,2,8,6,2,3,3,9,3,5,2,1,9,8,1,1,7,5,5,5,0,0,4,5,5,3,8,0,9,3,4,7,7,0,7,6,5,5,6,9,1,3,1,7,3,4,5,9,6,7,6,3,6,9,5,5,0,9,2,7,9,5,6,1,6,8,5,7,8,8,8,5,6,4,7,1,4,2,0,0,6,0,1,1,8,7,3,8,3,0,7,7,1,2,1,8,4,6,6,6,9,7,9,7,2,7,7,0,5,7,9,9,2,0,2,1,6,4,3,9,0,6,3,5,6,8,8,4,3,9,7,0,1,7,6,3,2,8,8,5,3,5,5,7,4,1,1,3,2,8,3,9,7,5,6,1,3,4,3,0,0,5,8,3,3,2,8,9,0,2,1,5,6,8,5,1,0,2,6,5,6,1,9,3,0,5,6,9,3,9,7,6,5,5,9,0,4,7,3,2,3,7,0,3,1,5,8,4,3,2,6,0,2,8,1,6,2,8,3,1,8,7,2,6,9,4,7,4,2,4,7,3,0,9,4,4,9,8,6,9,2,6,9,0,9,2,6,3,7,8,5,6,0,2,1,5,0,6,5,1,1,2,0,5,5,2,7,7,0,4,2,9,5,7,1,9,2,8,8,4,4,8,4,7,3,6,8,8,5,0,8,5,7,7,6,0,9,5,6,0,1,2,5,8,1,3,8,8,2,7,4,0,7,4,7,9,8,6,4,9,6,6,5,9,5,8,0,5,6,8,4,2,8,3,7,3,6,1,1,4,1,0,4,3,6,1,2,0,0,5,1,1,1,4,9,4,0,3,4,1,5,2,6,4,0,0,5,2,4,2,8,0,2,5,5,2,2,2,0,9,3,0,5,1,4,4,8,6,1,8,8,6,6,1,2,8,2,6,9,1,4,4,7,2,6,7,0,7,9,8,6,9,7,4,6,2,2,2,1,9,3,5,6,3,3,5,2,3,7,4,5,4,1,2,6,9,4,3,1,5,3,1,6,6,6,9,0,3,1,2,7,4,9,2,4,6,7,4,4,6,1,0,0,1,8,4,4,4,7,6,5,8,3,5,7,5,7,9,1,8,9,0,7,0,6,9,8,7,0,7,5,4,0,7,2,1,9,5,9,5,2,7,6,9,2,4,6,6,4,1,4,2,9,0,6,2,6,6,3,3,0,1,7,1,6,4,8,1,0,6,2,7,0,9,9,2,4,3,2,8,1,6,5,3,1,3,9,9,9,6,0,2,5,6,6,1,9,9,3,6,1,0,7,6,3,9,4,7,9,8,7,9,4,2,7,4,1,8,3,1,1,8,5,0,0,2,7,5,6,3,6,4,2,4,9,9,9,2,0,2,8,0,5,0,3,1,5,7,0,4,5,3,1,5,6,7,9,1,6,8,2,1,9,4,4};
    Digits output = fft(input, 100);
    print(output);

    return 0;
}
