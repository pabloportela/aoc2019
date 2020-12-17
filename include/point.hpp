#include <algorithm>

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
    Point operator*(const int) const;
    Point operator/(const int) const;

    // vector logic
    Point direction_to(const Point &) const;
    Point left() const;
    Point right() const;
};

struct PointHasher
{
    size_t operator()(const Point& p) const {
        return ((std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1)) >> 1);
    }
};

std::ostream &operator<<(std::ostream &, const Point &p);
