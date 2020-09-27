#include <cassert>
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;



bool has_no_downward_sequence(const char *p) {
    // check all digits are greater than or equal than last one
    char *c = const_cast<char *>(p) + 1;
    while (*c) {
        if (*(c-1) > *(c)) {
            return false;
        }
        c++;
    }
    return true;
}

bool has_exactly_two_consecutive_digits(const char *p) {
    char *c = const_cast<char *>(p) + 1;
    char state = 'a';

    while (*c) {

        // havent found a sequence of two same digits in a row yet
        if (state == 'a') {
            // char same as previous, enter sequence
            if (*(c-1) == *(c)) {
                // leaving in "valid password" state
                state = 'b';
            }
        }

        // just entered the sequence
        else if (state == 'b') {
            // third is different, found it
            if (*(c-1) != *(c)) {
                break;
            }
            // sequence contains more than two; not found
            else {
                state = 'c';
            }
        }

        // inside a failed sequence, continue until different
        else if (state == 'c') {
            // got out of the failed sequence
            if (*(c-1) != *(c)) {
                state = 'a';
            }
        }

        // ;)
        c++;
    }

    return state == 'b';
}


bool is_valid_password(const char *p) {
    return has_no_downward_sequence(p) && has_exactly_two_consecutive_digits(p);
}


unsigned int get_q_valid_passwords(int begin, int end) {
    unsigned int q_valid_passwords{};
    for (int i{begin}; i<=end; i++) {
        string s = to_string(i);
        if (is_valid_password(s.c_str())) {
            ++q_valid_passwords;
        }
    }

    return q_valid_passwords;
}


int main(int argc, char **argv) {
    assert(argc == 3);
    int begin = atoi(argv[1]);
    int end = atoi(argv[2]);
    assert(begin >= 100000);
    assert(end <= 999999);
    assert(begin < end);
    auto q_valid_passwords = get_q_valid_passwords(begin, end);

    cout << "There are " << q_valid_passwords << " valid passwords in such range\n";

    return 0;
}
