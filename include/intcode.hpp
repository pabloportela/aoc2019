#include <vector>
#include <map>
#include <queue>

using namespace std;


using Value = int64_t;
using Address = int64_t;
using Text = vector<Value>;
using Heap = map<Value, Value>;



class Memory {

    public:

    Memory(Text& text);
    Value get(Value);
    void set(Address, Value);
    void print();

    private:

    Text text;
    Heap heap;
};


class IntcodeComputer {

    public:

    IntcodeComputer(int, Text&);
    void push_input(Value);
    Value run();
    Value pop_output();
    size_t output_size();
    bool has_terminated();

    private:

    inline int parse_opcode(int);
    int parse_parameter_mode(int, int);

    Value get_read_param(int);
    Address get_write_address(int);
    void log(const char *);

    int id;
    queue<Value> input;
    queue<Value> output;
    bool terminated;

    Memory memory;
    Address ip;
    Address relative_base;
};
