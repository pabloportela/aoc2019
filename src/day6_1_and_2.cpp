#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>
#include <stack>

using namespace std;

using ChildrenMap = map<string, vector<string>>;

struct Node {
    Node(string name, Node *parent) : name(name), parent(parent) {}

    int get_total_depth(int);
    bool trace_parent(string &, stack<string> &);
    int get_minimum_orbital_transfers();

    string name;
    Node *parent;
    vector<Node> children;
};

int Node::get_minimum_orbital_transfers() {
    // trace Santa's and your parents
    string san_str{"SAN"}, you_str{"YOU"};
    stack<string> san_trace, you_trace;
    assert(this->trace_parent(san_str, san_trace));
    assert(this->trace_parent(you_str, you_trace));

    // trim path in common
    while (!san_trace.empty() && san_trace.top() == you_trace.top()) {
        san_trace.pop();
        you_trace.pop();
    }

    // rest of the paths
    return san_trace.size() + you_trace.size();
}


bool Node::trace_parent(string &name, stack<string> &acc) {
    if (this->name == name) {
        return true;
    }

    for (auto &c: children) {
        if (c.trace_parent(name, acc)) {
            acc.push(this->name);
            return true;
        }
    }

    return false;
}


int Node::get_total_depth(int acc) {
    int children_depth{};
    for (auto &c: children) {
        children_depth += c.get_total_depth(acc + 1);
    }

    return children_depth + acc;
}


ostream& operator<<(ostream &s, Node &n) {
    s << n.name << " , with " << n.children.size() << " kids" << endl;
    return s;
}


ChildrenMap parse_children(const char *filename) {
    fstream file;
    file.open(filename);
    assert(file.is_open());
    string line;
    ChildrenMap children;
    while (getline(file, line)) {
        children[line.substr(0, 3)].push_back(line.substr(4));
    }
    file.close();

    return children;
}


void populate_tree(Node *parent, ChildrenMap &children_map) {
    for (auto name: children_map[parent->name]) {

        Node n{name, parent};

        // recurse
        populate_tree(&n, children_map);

        parent->children.push_back(move(n));
    }
}


int main(int argc, char **argv) {
    // create root
    Node root{"COM", nullptr};

    // build tree
    ChildrenMap children_map = parse_children(argv[1]);
    populate_tree(&root, children_map);

    // total depth
    int total_orbits = root.get_total_depth(0);
    printf("Total orbits %d\n", total_orbits);

    int minimum_orbital_transfers = root.get_minimum_orbital_transfers();
    printf("minium orbital transfers: %d\n", minimum_orbital_transfers);

    return 0;
}
