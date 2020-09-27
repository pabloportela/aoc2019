#include <cassert>
#include <iostream>
#include "day4_2_lib.hpp"

using namespace std;


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
