#include <iostream>
#include <numeric>

#include "point.hpp"

using namespace std;


std::ostream &operator<<(std::ostream &out, const Point &p) {
    out << '(' << p.x << ',' << p.y << ')';
    return out;
}

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

inline bool Point::operator==(const Point &p) const {
    return (x == p.x && y == p.y);
}

inline bool Point::operator!=(const Point &p) const {
    return !(*this == p);
}

Point Point::direction_to(const Point &b) const {
    Point diff{b - *this};
    int gcd_xy{std::gcd(diff.x, diff.y)};

    if (gcd_xy== 0) {
        return diff;
    }
    else {
        return Point{diff/gcd_xy};
    }
}
