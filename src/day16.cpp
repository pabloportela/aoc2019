#include <cassert>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <array>
#include <vector>
#include <algorithm>

using namespace std;


vector<int> subset(const vector<int> &v, const size_t begin_offset, const size_t end_offset) {
    vector<int>::const_iterator begin = v.cbegin() + begin_offset;
    vector<int>::const_iterator end = v.cbegin() + end_offset;
    return vector<int>{begin, end};
}


void print(const vector<int> &v) {
    for (const auto &e: v)
        cout << e;
    cout << endl;
}


inline int acc_diff(const vector<int> &acc, const size_t position, const size_t length) {
    int acc_begin = position == 0 ? 0 : acc[position-1];
    int acc_end = acc[min(position + length - 1, acc.size() - 1)];
    return acc_end - acc_begin;
}


int process_digit(const size_t digit_offset, const vector<int> &acc) {
    auto width{digit_offset + 1};
    auto i{digit_offset};
    int result{};
    int sign{1};

    while (i < acc.size()) {
        result += sign * acc_diff(acc, i, width);
        sign *= -1;
        // jump to next 1, or -1 section
        i += width * 2;
    }

    return abs(result % 10);
}


vector<int> compute_accumulated(const vector<int> &input) {
    vector<int> acc(input.size());
    acc[0] = input[0];
    for (size_t i{1}; i<input.size(); ++i)
        acc[i] = acc[i-1] + input[i];

    return acc;
}


vector<int> phase(const vector<int> &input) {
    // compute acccumulated to reuse in every digit
    auto acc = compute_accumulated(input);

    // process each digit
    vector<int> output(input.size());
    for (size_t i{}; i<output.size(); i++)
       output[i] = process_digit(i, acc);

    return output;
}


vector<int> fft(const vector<int> &digits, const size_t q_phases) {
    vector<int> result{digits};
    for (size_t i{}; i<q_phases; ++i)
        result = phase(result);

    return result;
}


vector<int> parse_digits(const char *filename) {
    ifstream file{filename};
    assert(file.is_open());
    char digit;
    vector<int> digits;

    while (file >> digit) {
        digits.push_back(digit - '0');
    }

    return digits;
}

inline vector<int> repeat(const vector<int> &input, const size_t n) {
    vector<int> output(input.size() * n);
    for (size_t i{}; i<n; ++i)
        copy(input.cbegin(), input.cend(), output.begin() + (i * input.size()));

    return output;
}


void test_acc() {
    vector<int> input{1,2,3,4,5,6,7,8};
    auto acc = compute_accumulated(input);
    print(input);
    print(acc);
    assert(acc_diff(acc,0,1) == 1);
    assert(acc_diff(acc,0,2) == 3);
    assert(acc_diff(acc,0,8) == 36);
    assert(acc_diff(acc,1,1) == 2);
    assert(acc_diff(acc,1,2) == 5);
    assert(acc_diff(acc,1,5) == 20);
    assert(acc_diff(acc,2,3) == 12);
    assert(acc_diff(acc,5,3) == 21);
    assert(acc_diff(acc,5,4) == 21);
    assert(acc_diff(acc,5,48) == 21);
    cout << "Acc diff should work\n";
}


int main(int argc, char **argv) {
    vector<int> input = parse_digits(argv[argc - 1]);

    // part 1
    {
        vector<int> result = fft(input, 100);
        print(subset(result, 0, 8));
    }


    // part 2
    {
        vector<int> repeated_input = repeat(input, 10000);
        vector<int> result = fft(repeated_input, 100);
        size_t msg_offset{5971313};
        print(subset(result, msg_offset, msg_offset + 8));
    }

    return 0;
}
