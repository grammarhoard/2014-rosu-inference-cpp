#include <string>
#include <set>
#include <utility>

using namespace std;

#pragma once
class ObjectiveContextFreeGrammar
{
public:
    typedef pair<string, string> Production; // pair (left, right)

    set<string> Sigma; // finite non-empty alphabet of terminal symbols
    set<string> V; // set of non terminals
    set<Production> P; // set of productions
    set<string> I; // set of initial symbols

    ObjectiveContextFreeGrammar();
    ~ObjectiveContextFreeGrammar();

    /*
     * Returns "" if the Grammar is correct (//TODO what is correct?)
     *     or a string w otherwise
     */
    //TODO implement this
    // string Equiv();
};

