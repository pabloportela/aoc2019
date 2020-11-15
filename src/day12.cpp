#include <array>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

struct Point {

    int x;
    int y;
    int z;

    Point(int x, int y, int z) : x(x), y(y), z(z) {}
    Point() : x(0), y(0), z(0) {}

    // operator overloads
    bool operator==(const Point &) const;
    bool operator!=(const Point &) const;
    Point& operator+=(const Point &);
    Point operator+(const Point &) const;
    Point operator-(const Point &) const;
    Point operator/(const int) const;
    bool operator<(const Point &) const;

    // vector logic
    Point direction_to(const Point &) const;


    friend ostream &operator<<(ostream &out, const Point &p) {
        out << '(' << p.x << ',' << p.y << ',' << p.z << ')';
        return out;
    }
};

Point Point::operator+(const Point &p) const {
    return Point{x + p.x, y + p.y, z + p.z};
}

Point Point::operator-(const Point &p) const {
    return Point{x - p.x, y - p.y, z - p.z};
}

Point Point::operator/(const int divisor) const {
    assert(divisor != 0);
    return Point{x / divisor, y / divisor, z / divisor};
}

Point& Point::operator+=(const Point &p) {
    x += p.x;
    y += p.y;
    z += p.z;
    return *this;
}

inline bool Point::operator==(const Point &p) const {
    return (x == p.x && y == p.y && z == p.z);
}

inline bool Point::operator!=(const Point &p) const {
    return !(*this == p);
}


Point Point::direction_to(const Point &b) const {
    Point diff{b - *this};
    int gcd_xyz{gcd(gcd(diff.x, diff.y), diff.z)};

    if (gcd_xyz == 0) {
        return diff;
    }
    else {
        return Point{diff/gcd_xyz};
    }
}

struct PointHasher
{
    size_t operator()(const Point& p) const {
        return ((hash<int>()(p.x) ^ (hash<int>()(p.y) << 1) ^ (hash<int>()(p.z) << 1)) >> 1);
    }
};


using Positions = vector<Point>;
using Velocities = vector<Point>;

class MoonSystem {

    public:

    MoonSystem(Positions positions, Velocities velocities);
    void walk(size_t q_steps);
    size_t compute_total_energy() const;
    void print() const;
    size_t count_steps_for_lap();

    private:

    void step();
    inline int compute_energy(const Point &) const;

    Positions positions;
    Velocities velocities;
    const size_t length;
};

MoonSystem::MoonSystem(Positions positions, Velocities velocities) : positions(positions), velocities(velocities), length(positions.size()) {
    assert(positions.size() == velocities.size());
}

void MoonSystem::print() const {
    cout << "got " << positions.size() << " positions\n";
    for (auto &p: positions)
        cout << p << endl;

    cout << "got " << velocities.size() << " velocities\n";
    for (auto &p: velocities)
        cout << p << endl;
}

void MoonSystem::step() {
    // apply gravity to velocities
    for (size_t i{}; i<length; i++) {
        for (size_t j{}; j<length; j++) {
            Point delta{positions[j] - positions[i]};
            if (delta.x != 0)
                velocities[i].x += delta.x > 0 ? 1 : -1;
            if (delta.y != 0)
                velocities[i].y += delta.y > 0 ? 1 : -1;
            if (delta.z != 0)
                velocities[i].z += delta.z > 0 ? 1 : -1;
        }
    }

    // apply velocity to positions
    for (size_t i{}; i<length; i++) {
        positions[i] += velocities[i];
    }
}

void MoonSystem::walk(size_t q_steps) {
    for (size_t i{}; i<q_steps; i++) {
        step();
    }
}

inline int MoonSystem::compute_energy(const Point &p) const {
    return abs(p.x) + abs(p.y) + abs(p.z);
}

inline size_t MoonSystem::compute_total_energy() const {
    size_t total_energy{};

    for (size_t i{}; i<length; i++)
        total_energy += static_cast<size_t>(compute_energy(positions[i])) * static_cast<size_t>(compute_energy(velocities[i]));

    return total_energy;
}

size_t MoonSystem::count_steps_for_lap() {
    auto initial_positions = positions;
    auto initial_velocities = velocities;
    size_t q_steps{}, x{}, y{}, z{};

    Point cycles{};

    while (!x || !y || !z) {
        step();
        q_steps++;

        if (q_steps % 1000000 == 0)
            cout << q_steps << endl;



        // match individual positions and velocities for individual x, y, and z's.
        // on the verge of making a lambda but desisted
        if (!x) {
            size_t i{};
            while (i<length)
                if (positions[i].x != initial_positions[i].x || velocities[i].x != initial_velocities[i].x)
                    break;
                else
                    i++;
            if (i == length)
                x = q_steps;
        }

        if (!y) {
            size_t i{};
            while (i<length)
                if (positions[i].y != initial_positions[i].y || velocities[i].y != initial_velocities[i].y)
                    break;
                else
                    i++;
            if (i == length)
                y = q_steps;
        }

        if (!z) {
            size_t i{};
            while (i<length)
                if (positions[i].z != initial_positions[i].z || velocities[i].z != initial_velocities[i].z)
                    break;
                else
                    i++;
            if (i == length)
                z = q_steps;
        }
    }

    return lcm(x, lcm(y, z));
}

void test() {

    vector<Point> positions = {
        Point{-1,0,2},
        Point{2,-10,-7},
        Point{4,-8,8},
        Point{3,5,-1}
    };

    vector<Point> velocities = {
        Point{0, 0, 0},
        Point{0, 0, 0},
        Point{0, 0, 0},
        Point{0, 0, 0}
    };

    MoonSystem ms{positions, velocities};
    ms.walk(10);

    ms.print();

    auto total_energy = ms.compute_total_energy();
    assert(total_energy == 179);
    cout << "we're good!\n";
}

int main() {

    test();

    vector<Point> positions = {
        Point{17, -9, 4  },
        Point{ 2,  2, -13},
        Point{-1,  5, -1 },
        Point{ 4,  7, -7 }
    };

    vector<Point> velocities = {
        Point{0, 0, 0},
        Point{0, 0, 0},
        Point{0, 0, 0},
        Point{0, 0, 0}
    };

    // Part 1
    MoonSystem ms{positions, velocities};
    ms.walk(1000);
    auto total_energy = ms.compute_total_energy();
    cout << "Total energy after 1000 steps: " << total_energy << endl;

    // Part 2
    MoonSystem ms2{positions, velocities};
    size_t q_steps = ms2.count_steps_for_lap();
    cout << "Total laps before coming back to initial state: " << q_steps << endl;

    return 0;
}
