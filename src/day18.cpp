#include <array>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <cmath>
#include <numeric>
#include <cctype>
#include <vector>
#include <map>
#include <algorithm>

#include "point.hpp"

using namespace std;

using PositionSet = unordered_set<Point, PointHasher>;
using Field = map<Point, char, decltype(cmp_point)>;
using Letters = set<char>;
using KeyPosition = map<char, Point>;

struct LettersHasher
{
    size_t operator()(const Letters& l) const {
        size_t hash{};
        for (const auto c: l)
            hash = ((std::hash<char>()(c) ^ (hash << 1)) >> 1);
        return hash;
    }
};

using Progress = unordered_map<char, unordered_map<Letters, size_t, LettersHasher>>;

struct Trace {
    char target;
    size_t q_steps{};
    Letters doors{};
    Letters keys{};

    inline bool can_go_through(const Letters &available_keys) const {
        return (doors.empty() || includes(available_keys.begin(), available_keys.end(), doors.begin(), doors.end()));
    }
};

using LetterTraces = unordered_map<char, vector<Trace>>;

class Tunnel {
    public:
    Tunnel(const char *);
    void q_steps_to_gather_keys(const char, const Letters &, const size_t);
    size_t min_so_far;

    private:
    void print(const Point &, const Letters &) const;
    Trace get_trace(const Point &, const Point &) const;

    Letters needed_keys;
    size_t needed_keys_size;
    // works like an adjacency matrix
    LetterTraces traces;
    KeyPosition key_position;
    Field field;
    Point start_position;

    Progress progress;

};


Tunnel::Tunnel(const char *filename) : field(cmp_point) {
    // parse file
    ifstream file{filename};
    assert(file.is_open());
    string line;
    int x{}, y{};
    while (getline(file, line)) {
        x = 0;
        for (auto &c: line) {
            Point position{x, y};
            cout << c;

            // start position
            if (c == '@') {
                start_position = position;
                c = '.';
            }

            // key
            if (islower(c)) {
                key_position[c] = position;
                needed_keys.insert(c);
            }

            field[position] = c;
            ++x;
        }
        cout << endl;
        ++y;
    }

    // trace paths
    for (const auto &[key_a, position_a]: key_position) {
        cout << "\nWill trace @ to " << key_a << endl;
        auto trace = get_trace(start_position, position_a);
        trace.target = key_a;
        traces['@'].push_back(trace);

        for (const auto &[key_b, position_b]: key_position)
            if (!(
                key_a == key_b ||
                (
                    traces.count(key_a) &&
                    find_if(
                        traces.at(key_a).begin(),
                        traces.at(key_a).end(),
                        [key_b] (Trace t) {return t.target == key_b;}
                    ) != traces.at(key_a).end()
                )
            )) {
                cout << "\nWill trace " << key_a << " to " << key_b << endl;
                trace = get_trace(position_a, position_b);
                trace.target = key_b;
                traces[key_a].push_back(trace);
                trace.target = key_a;
                traces[key_b].push_back(trace);
            }
    }

    // sort each vector by q_steps
    for (auto &t: traces)
        sort(
            t.second.begin(),
            t.second.end(),
            [](const Trace& lhs, const Trace& rhs) {return lhs.q_steps < rhs.q_steps;}
        );

    // other initializations
    min_so_far = numeric_limits<size_t>::max();
    needed_keys.insert('@');
    needed_keys_size = needed_keys.size();
}


Trace Tunnel::get_trace(const Point &start, const Point &target) const {
    // Breadth first search of shortest path between start and target
    // Returns number of steps, extra doors and keys gathered along

    // mart start point as expanded
    PositionSet expanded{};
    expanded.insert(start);

    // look around
    unordered_map<Point, Trace, PointHasher> to_expand;
    for (const auto &d: directions)
        if (field.at(start + d) != '#')
            to_expand[start + d] = Trace{};

    size_t i{1};
    while (!to_expand.empty()) {
        // copy because we will alter contents of the original inside the loop
        auto to_expand_copy{to_expand};
        for (auto &[position, acc_trace]: to_expand_copy) {
            acc_trace.q_steps = i;

            if (position == target) {
                cout << "Trace with this many steps: " << i << endl;
                cout << "And these doors: ";
                for (const auto &c: acc_trace.doors)
                    cout << (char) toupper(c);
                cout << "\nAnd these keys: ";
                for (const auto &c: acc_trace.keys)
                    cout << (char) c;
                cout << endl;

                return acc_trace;
            }

            expanded.insert(position);
            to_expand.erase(position);

            const auto c = field.at(position);

            if (c == '#')
                continue;

            if (islower(c))
                acc_trace.keys.insert(c);
            else if (isupper(c))
                acc_trace.doors.insert(tolower(c));
            else
                assert(c == '.');

            for (const auto &d: directions) {
                if (field.at(position + d) != '#')
                    to_expand[position + d] = Trace{acc_trace};
            }
        }
        ++i;
    }

    // empty trace, singnaling not found, should be an error
    return Trace();
}


void Tunnel::print(const Point &position, const Letters &available_keys) const {
    int y{};
    cout << endl;
    for (const auto &[k, v]: field) {
        if (k.y != y) {
            y = k.y;
            cout << endl;
        }

        if (available_keys.count(v))
            cout << '.';
        else if (isupper(v) && available_keys.count(tolower(v)))
            cout << '.';
        else
            cout << v;
    }
    cout << endl;
    cout << "current position:" << field.at(position) << endl;
    cout << endl;
}

void Tunnel::q_steps_to_gather_keys(const char current_key, const Letters &available_keys, const size_t acc_steps) {
    // already past a record, backtrack
    if (acc_steps >= min_so_far)
        return;

    // got all keys, done with the branch
    if (available_keys.size() == needed_keys_size) {
        cout << "Got a record solution: " << acc_steps << endl;
        min_so_far = acc_steps;
        return;
    }

    // we found a better path already
    if (
        progress.count(current_key)
        && progress.at(current_key).count(available_keys)
        && progress.at(current_key).at(available_keys) < acc_steps
    )
        return;

    progress[current_key][available_keys] = acc_steps;

    // filter out keys we already got and non-accessible ones
    Letters available_keys_aux;
    for (const auto &trace: traces[current_key]) {
        if (available_keys.count(trace.target) || !trace.can_go_through(available_keys))
            continue;

        available_keys_aux = available_keys;
        // insert the key before visiting
        available_keys_aux.insert(trace.target);
        // insert any keys found in the way
        available_keys_aux.insert(trace.keys.begin(), trace.keys.end());
        // visit
        q_steps_to_gather_keys(trace.target, available_keys_aux, acc_steps + trace.q_steps);
    }
}

int main(int argc, char **argv) {
    Tunnel t{argv[1]};

    t.q_steps_to_gather_keys('@', Letters({'@'}), 0);
    cout << "Need " << t.min_so_far << " steps\n";

    return 0;
}
