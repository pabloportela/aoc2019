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

    Image(vector<int> pixels, size_t width, size_t height) :
        pixels(pixels),
        width(width),
        height(height)
    {
        assert(pixels.size() % (width * height) == 0);
    }
    int get_answer1();
    void print();

    private:

    int count_digit_x_in_layer(int, int);
    inline size_t count_layers();
    inline size_t layer_size();
    tuple<vector<int>::iterator, vector<int>::iterator> get_layer_range(int);
    size_t get_layer_with_least_digits_x(int);
    char render_pixel(size_t, size_t);

    vector<int> pixels;
    size_t width;
    size_t height;
};

inline size_t Image::layer_size() {
    return width * height;
}

inline size_t Image::count_layers() {
    return pixels.size() / layer_size();
}

tuple<vector<int>::iterator, vector<int>::iterator> Image::get_layer_range(int layer) {
    int begin = layer * layer_size();
    int end = begin + layer_size();

    return tuple<vector<int>::iterator, vector<int>::iterator>{pixels.begin() + begin, pixels.begin() + end};
}

int Image::count_digit_x_in_layer(int x, int layer) {
    auto [begin, end] = get_layer_range(layer);
    return count(begin, end, x);
}

size_t Image::get_layer_with_least_digits_x(int x) {
    int min_so_far{numeric_limits<int>::max()};
    int layer_wmd{};
    for (size_t layer{}, j{count_layers()}; layer<j; layer++) {
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

char Image::render_pixel(size_t x, size_t y) {
    size_t layer_offset = y * width + x;
    for (size_t layer{}, q_layers{count_layers()}; layer<q_layers; layer++) {
        int p = pixels[layer * layer_size() + layer_offset];
        if (p == 0)
            return ' ';
        if (p == 1)
            return '1';
    }
    return ' ';
}

void Image::print() {
    // go down the 4th quadrant
    cout << endl;
    for (size_t y{}; y<height; y++) {
        for (size_t x{}; x<width; x++)
            cout << render_pixel(x, y) << " ";
        cout << endl;
    }
    cout << endl;
}

void test_answer1() {
    Image image{vector<int>{0, 1, 1, 1, 2, 0, 0, 1, 2, 2, 1, 1 }, 2, 2};
    assert(image.get_answer1() == 4);
}

void test_printing() {
    Image image2{vector<int>{0, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 2, 0, 0, 0, 0}, 2, 2};
    image2.print();
}

int main(int argc, char **argv) {
    vector<int> digits = parse_digits(argv[1]);

    // instantiate our image
    Image image{digits, 25, 6};

    // Part 1
    test_answer1();
    int answer1 = image.get_answer1();
    cout << "Amount of 1's times amount of 2's in layer with least 0's: " << answer1 << endl << endl;

    // Part 2
    test_printing();
    image.print();

    return 0;
}
