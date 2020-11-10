#include <cstdio>
#include <numeric>
#include <cassert>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <map>

using namespace std;


class Point {

    public:

    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {}
    Point() : x(0), y(0) {}

    // operator overloads
    bool operator==(const Point &) const;
    bool operator!=(const Point &) const;
    Point& operator+=(const Point &);
    Point operator+(const Point &) const;
    Point operator-(const Point &) const;
    Point operator/(const int) const;

    // vector logic
    Point direction_to(const Point &) const;

    friend ostream &operator<<(ostream &out, const Point &p) {
        out << '(' << p.x << ',' << p.y << ')';
        return out;
    }
};

Point Point::operator+(const Point &p) const {
    return Point{x + p.x, y + p.y};
}

Point Point::operator-(const Point &p) const {
    return Point{x - p.x, y - p.y};
}

Point Point::operator/(const int divisor) const {
    return Point{x / divisor, y / divisor};
}

Point& Point::operator+=(const Point &p) {
    x += p.x;
    y += p.y;
    return *this;
}

bool Point::operator==(const Point &p) const {
    return (x == p.x && y == p.y);
}

bool Point::operator!=(const Point &p) const {
    return !(*this == p);
}

Point Point::direction_to(const Point &b) const {
    Point diff{b - *this};
    int gcd_xy{gcd(diff.x, diff.y)};

    if (gcd_xy== 0) {
        return diff;
    }
    else {
        return Point{diff/gcd_xy};
    }
}



// main class

class MonitoringStation {

    public:


    MonitoringStation(vector<char> &&, const size_t, const size_t);
    static MonitoringStation from_file(const char *);
    void compute_clockwise_directions();

    Point find_best_spot() const;

    void print() const;
    int asteroid_field_get(size_t, size_t) const;
    void asteroid_field_set(size_t, size_t, int);
    size_t count_asteroids_on_sight(const Point &) const;
    int detect(Point, const Point &) const;
    bool is_inside_field(const Point &) const;

    private:

    vector<Point> asteroids;
    vector<int> asteroid_field;
    vector<Point> search_directions;

    size_t width;
    size_t height;
};

void MonitoringStation::compute_clockwise_directions() {
    /*
    Computes clockwise order of every direction point an asteroid may have
    taking into account width and height of the field
    */

    // compute and sort weights for 1st quadrant
    map<double, Point> clockwise_directions;
    Point origin{};
    for (size_t y{1}; y < height; y++)
        for (size_t x{1}; x < width; x++)
            // weight is tan; op / adj
            // direction is the smallest fraction
            clockwise_directions[static_cast<double>(x)/static_cast<double>(y)] = origin.direction_to(Point{static_cast<int>(x), static_cast<int>(y)});

    // 12:00
    search_directions.emplace_back(0, 1);
    // 1st quadrant; (x,y)
    for (auto &pair: clockwise_directions) {
        search_directions.push_back(pair.second);
    }

    // 3:00
    search_directions.emplace_back(1, 0);
    // 2st quadrant; (y,-x)
    for (auto &pair: clockwise_directions) {
        search_directions.emplace_back(pair.second.y, -pair.second.x);
    }

    // 6:00
    search_directions.emplace_back(0, -1);
    // 3rd quadrant; (-x,-y)
    for (auto &pair: clockwise_directions) {
        search_directions.emplace_back(-pair.second.x, -pair.second.y);
    }

    // 9:00
    search_directions.emplace_back(-1, 0);
    // 4th quadrant; (-y,x)
    for (auto &pair: clockwise_directions) {
        search_directions.emplace_back(-pair.second.y, pair.second.x);
    }

    /*
    for (auto &point: search_directions)
        cout << point << endl;
    */
}

MonitoringStation::MonitoringStation(vector<char> &&char_input, const size_t width, const size_t height): width(width), height(height)
{
    cout << "input size " << char_input.size() << endl;

    // gather asteroids
    size_t asteroid_count{};

    for (size_t y{}; y<height; y++) {
        for (size_t x{}; x<width; x++) {
            char c = char_input[y * height + x];
            if (c == '#') {
                asteroid_field.push_back(asteroid_count);
                asteroids.emplace_back(static_cast<int>(x), static_cast<int>(y));
                asteroid_count++;
            }
            else {
                assert(c == '.');
                asteroid_field.push_back(-1);
            }
        }
    }
    assert(asteroids.size() > 1);
    assert(asteroid_field.size() == width * height);

    compute_clockwise_directions();
}

bool MonitoringStation::is_inside_field(const Point &p) const {
    return p.x >= 0 && p.x < static_cast<int>(width) && p.y >= 0 && p.y < static_cast<int>(height);
}

MonitoringStation MonitoringStation::from_file(const char *filename) {
    ifstream file{filename};
    assert(file.is_open());

    vector<char> char_input;
    string line;
    size_t height{}; // increment in the while
    size_t width;
    while (getline(file, line)) {
        // append to asteroid field
        for (auto &c: line) {
            char_input.push_back(c);
            width = line.size();
        }
        height++;
    }

    return MonitoringStation(move(char_input), width, height);
}


void MonitoringStation::print() const {
    for (size_t y{}; y<height; y++) {
        for (size_t x{}; x<width; x++) {
            int v = asteroid_field_get(x, y);
            if (v != -1)
                printf("%0*d|", 2, v);
            else
                printf("  |");
        }
        cout << endl;
    }
}

int MonitoringStation::asteroid_field_get(size_t x, size_t y) const {
    return asteroid_field.at(y * height + x);
}

void MonitoringStation::asteroid_field_set(size_t x, size_t y, int val) {
    asteroid_field.at(y * height + x) = val;
}


Point MonitoringStation::find_best_spot() const {
    Point best_spot{0, 0}; // bogus value
    size_t max_count{0};

    printf("There are %lu asteroids\n", asteroids.size());
    for (auto a: asteroids) {
        size_t count = count_asteroids_on_sight(a);
        if (count > max_count) {
            max_count = count;
            best_spot = a;
        }
    }

    assert(max_count > 0);
    return best_spot;
}

int MonitoringStation::detect(Point from, const Point &direction) const {
    while (true) {
        from += direction;
        if (!is_inside_field(from)) {
            return -1;
        }

        int asteroid_index = asteroid_field_get(
            static_cast<size_t>(from.x),
            static_cast<size_t>(from.y)
        );
        if (asteroid_index >= 0) {
            return asteroid_index;
        }
    }
}

size_t MonitoringStation::count_asteroids_on_sight(const Point &asteroid) const {
    size_t count{};

    for (auto &direction: search_directions)
        if (detect(asteroid, direction) >= 0)
            count++;

    return count;
}


void test_best_spot() {
    vector<char> f1{'.', '#', '.', '.', '#', '.', '.', '.', '.', '.', '#', '#', '#', '#', '#', '.', '.', '.', '.', '#', '.', '.', '.', '#', '#'};
    MonitoringStation s1{move(f1), 5, 5};
    s1.print();

    /*
      01234
    0 .0..1
    1 .....
    2 23456
    3 ....7
    4 ...89
    */

    bool t;

    t = (s1.detect(Point{1,0}, Point{-1,0}) == -1);
    assert(t);

    t = (s1.detect(Point{1,0}, Point{1,0}) == 1);
    assert(t);

    t = (s1.detect(Point{1,0}, Point{3,1}) == -1);
    assert(t);

    t = (s1.detect(Point{1,0}, Point{1,2}) == 4);
    assert(t);

    t = (s1.detect(Point{4,0}, Point{-1,4}) == 8);
    assert(t);

    t = (s1.detect(Point{4,0}, Point{-1,5}) == -1);
    assert(t);

    t = (s1.detect(Point{1,2}, Point{-1,0}) == 2);
    assert(t);

    t = (s1.detect(Point{3,4}, Point{-2,-2}) == 3);
    assert(t);

    t = (s1.detect(Point{3,4}, Point{-3,-2}) == 2);
    assert(t);

    auto b1 = s1.find_best_spot();
    cout << "best spot: " << b1 << endl;
    auto t1 = (b1 == Point{3, 4});
    assert(t1);

    cout << "Passed the monitoring station tests\n";
}



int main(int argc, char **argv) {

    // test_point();
    // test_best_spot();

    MonitoringStation m = MonitoringStation::from_file(argv[argc - 1]);
    auto best_spot = m.find_best_spot();
    cout << best_spot.x << ',' << best_spot.y << " is the best spot, it can detect " << m.count_asteroids_on_sight(best_spot) << " asteroids." << endl;

    return 0;
}
