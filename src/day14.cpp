#include <algorithm>
#include <fstream>
#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>

#include <boost/algorithm/string.hpp>

using namespace std;

using Chem = string;
using ChemQty = pair<Chem, size_t>;
using Reaction = pair<vector<ChemQty>, ChemQty>;


struct NotEnoughOreException : public exception {};

class NanoFactory {

    public:
    NanoFactory(const vector<Reaction> &);

    size_t get_available_chem(const Chem &);

    size_t compute_cost_in_ore(const Chem &, const size_t);
    size_t compute_produced_with_budget(const Chem &, const size_t);

    private:
    bool can_produce(const Chem &, const size_t, const size_t);
    void produce(const Chem &, const size_t);
    void consume(const Chem &, size_t);
    size_t redeem_from_stock(const Chem &, size_t);

    unordered_map<Chem, Reaction> reaction_map;

    // state variables
    unordered_map<Chem, size_t> chem_stock;
    size_t q_ore_spent;
    bool unlimited_ore;
};

NanoFactory::NanoFactory(const vector<Reaction> &reactions) {
    // Lookup per material.
    for (const auto &reaction: reactions)
        reaction_map[reaction.second.first] = reaction;
}

size_t NanoFactory::get_available_chem(const Chem &chem) {
    return chem_stock[chem];
}

size_t NanoFactory::compute_produced_with_budget(const Chem &chem, const size_t budget) {

    // find bounds
    size_t upper{1};
    while (can_produce(chem, upper, budget))
        upper *= 2;

    size_t lower = upper / 2;

    while (true) {
        auto middle = (upper + lower) / 2;
        cout <<  "Trying with " << middle << endl;
        auto a = can_produce(chem, middle, budget);
        auto b = can_produce(chem, middle+1, budget);

        if (a && !b)
            return middle;

        if (a && b)
            lower = middle + 1;
        else
            upper = middle - 1;
    }
}

bool NanoFactory::can_produce(const Chem &chem, const size_t qty, const size_t budget) {
    chem_stock.clear();
    chem_stock["ORE"] = budget;
    q_ore_spent = 0;
    unlimited_ore = false;

    try {
        produce(chem, qty);
        return true;
    }
    catch (NotEnoughOreException) {
        return false;
    }
}

size_t NanoFactory::compute_cost_in_ore(const Chem &chem, const size_t qty) {
    // set object state
    chem_stock.clear();
    q_ore_spent = 0;
    unlimited_ore = true;

    // produce
    produce(chem, qty);

    return q_ore_spent;
}

void NanoFactory::produce(const Chem &output_chem, const size_t qty_to_produce) {
    // produces, at least qty_produce
    // or more if the qty_to_produce is not a multiple of output_qty
    //

    const auto &reaction = reaction_map.at(output_chem);
    const size_t output_qty = reaction.second.second;
    const size_t q_needed_reactions = (qty_to_produce + output_qty - 1) / output_qty;
    for (const auto&[input_chem, input_qty]: reaction.first)
        consume(input_chem, input_qty * q_needed_reactions);

    // required materials were consumed, we got the right to increment
    // note maybe there were leftovers in the process
    chem_stock[output_chem] += reaction.second.second * qty_to_produce;
}


// heart of the program
void NanoFactory::consume(const Chem &chem, size_t qty_to_consume) {

    // always try stock first
    qty_to_consume -= redeem_from_stock(chem, qty_to_consume);
    // we got enough from stock
    if (qty_to_consume == 0) {
        return;
    }

    assert(qty_to_consume > 0);

    // recurse
    const auto &reaction = reaction_map.at(chem);
    const size_t &output_qty = reaction.second.second;
    const size_t q_needed_reactions = (qty_to_consume + output_qty - 1) / output_qty;
    // double check logic
    assert(q_needed_reactions * output_qty >= qty_to_consume);

    // consume required
    for (const auto&[input_chem, input_qty]: reaction.first) {
        const auto qty_to_consume = input_qty * q_needed_reactions;
        // ORE production has custom logic
        if (input_chem == "ORE") {
            // part 1
            if (unlimited_ore)
                q_ore_spent += qty_to_consume;
            // part 2
            else {
                if (qty_to_consume >= chem_stock["ORE"])
                    throw NotEnoughOreException();
                else
                    chem_stock["ORE"] -= qty_to_consume;
            }
        }
        else {
            // this is how much we need to consume
            consume(input_chem, qty_to_consume);
        }
    }

    chem_stock[chem] += q_needed_reactions * output_qty - qty_to_consume;
}

size_t NanoFactory::redeem_from_stock(const Chem &chem, const size_t qty) {
    // redeem from stock as much as possible, update stock and returns much was actually redeemed

    // fetch stock of that chem
    auto &available_qty = chem_stock[chem];

    // got something stored
    if (available_qty) {
        // full redemption
        if (available_qty >= qty) {
            available_qty -= qty;
            return qty;
        }
        // partial redemption
        else {
            auto aux = available_qty;
            available_qty = 0;
            return aux;
        }
    }
    // the nothing box
    else {
        return 0;
    }
}


ChemQty parse_chem_qty(string &chem_qty_str) {
    boost::trim(chem_qty_str);
    string qty_str = chem_qty_str.substr(0, chem_qty_str.find(" "));
    stringstream sstream(qty_str);
    size_t qty;
    sstream >> qty;
    Chem chem = chem_qty_str.substr(chem_qty_str.find(" ") + 1);

    return ChemQty{chem, qty};
}

vector<Reaction> parse_reactions_from_file(const char *filename) {
    fstream file;
    file.open(filename);
    assert(file.is_open());

    string line;
    vector<Reaction> reactions;
    while (getline(file, line)) {
        // differentiate input and output
        string input_str = line.substr(0, line.find("=>"));
        string output_str = line.substr(line.find("=>") + 2);

        vector<string> chem_qty_str_vec;
        boost::split(chem_qty_str_vec, input_str, [](char c){return c == ',';});

        vector<ChemQty> input{};
        for (auto &chem_qty_str: chem_qty_str_vec)
            input.push_back(parse_chem_qty(chem_qty_str));

        ChemQty output{parse_chem_qty(output_str)};

        reactions.emplace_back(input, output);
    }

    file.close();

    return reactions;
}

int main(int argc, char **argv) {
    vector<Reaction> reactions = parse_reactions_from_file(argv[argc-1]);
    NanoFactory nf{reactions};

    // Part 1
    auto q_ore_needed = nf.compute_cost_in_ore("FUEL", 1);

    cout << "To produce 1 FUEL, we need " << q_ore_needed << " ORE." << endl;
    cout << "Si no tienen, que lloren." << endl << endl;

    // Part 2
    auto q_fuel_produced = nf.compute_produced_with_budget("FUEL", 1000000000000);

    cout << "Will a trillion ORE we can produce " << q_fuel_produced << " FUEL." << endl;
    cout << "Si no tenés, ya fuel." << endl;

    return 0;
}
