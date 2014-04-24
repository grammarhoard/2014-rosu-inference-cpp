/*
 * Objective Context Free Grammar (CFG)
 * ...
 */
//TODO write the description for Objective Context Free Grammar

#include <string>
#include <set>
#include <utility>

using namespace std;

#pragma once
class ContextFreeGrammar
{
public:
    //TODO maybe use objects instead of strings for terminals / non-terminals
    typedef pair<string, string> Production; // pair (left, right)

    set<string>     Sigma; // finite non-empty alphabet of terminal symbols
    set<string>     V;     // set of non terminals
    set<Production> P;     // set of productions
    set<string>     I;     // set of initial symbols

    ContextFreeGrammar();
    ~ContextFreeGrammar();

    /*
     * Returns the start symbol (S)
     */
    string getStartSymbol();

    /*
     * Returns an unique non-terminal symbol
     */
    string getNonTerminalSymbol();

private:
    char _currentChar = 'A';
    const string _startSymbol = "S";
};
