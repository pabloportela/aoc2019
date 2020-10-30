#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <tuple>
#include <limits>

using namespace std;


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


class Image {

    public:

    Image(vector<int> pixels, size_t layer_size) : pixels(pixels), layer_size(layer_size) {
        assert(q_layers() * layer_size == pixels.size());
    }
    int get_answer1();

    private:

    int count_digit_x_in_layer(int, int);
    inline size_t q_layers() {return pixels.size() / layer_size;}
    tuple<vector<int>::iterator, vector<int>::iterator> get_layer_range(int);
    size_t get_layer_with_least_digits_x(int);

    vector<int> pixels;
    size_t layer_size;
};

tuple<vector<int>::iterator, vector<int>::iterator> Image::get_layer_range(int layer) {
    int begin = layer * layer_size;
    int end = begin + layer_size;

    return tuple<vector<int>::iterator, vector<int>::iterator>{pixels.begin() + begin, pixels.begin() + end};
}

int Image::count_digit_x_in_layer(int x, int layer) {
    auto [begin, end] = get_layer_range(layer);
    return count(begin, end, x);
}

size_t Image::get_layer_with_least_digits_x(int x) {
    int min_so_far{numeric_limits<int>::max()};
    int layer_wmd{};
    for (size_t layer{}, j{q_layers()}; layer<j; layer++) {
        int q_x_digits = count_digit_x_in_layer(x, layer);
        if (q_x_digits < min_so_far) {
            min_so_far = q_x_digits;
            layer_wmd = layer;
        }
    }

    assert(min_so_far != -1);
    return layer_wmd;
}

int Image::get_answer1() {
    int layer = get_layer_with_least_digits_x(0);
    return count_digit_x_in_layer(1, layer) * count_digit_x_in_layer(2, layer);
}

void test() {
    Image image{vector<int>{0, 1, 1, 1, 2, 0, 0, 1, 1, 2, 1, 2, 2, 1, 1 }, 5};
    assert(image.get_answer1() == 6);
}

int main(int argc, char **argv) {
    vector<int> digits = parse_digits(argv[1]);

    test();

    // instantiate our image
    size_t layer_size = 25 * 6;
    Image image{digits, layer_size};

    int answer1 = image.get_answer1();
    cout << "Amount of 1's times amount of 2's in layer with least 0's: " << answer1 << endl;

    return 0;
}
