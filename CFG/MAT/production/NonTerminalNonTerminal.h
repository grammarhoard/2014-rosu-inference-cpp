#pragma once
#include <utility>

#include "ProductionRight.h"
#include "NonTerminal.h"
#include "Terminal.h"

using namespace std;

class NonTerminalNonTerminal: public ProductionRight
{
public:
    typedef pair<NonTerminal, NonTerminal> NonTerminalPair;

    NonTerminalNonTerminal(NonTerminalPair pair);
    ~NonTerminalNonTerminal();

    NonTerminalPair getPair() const;

    /*
     * Always returns false
     */
    bool equals(const Terminal& terminal) const;

    /*
     * Compares the names of the non-terminals in the pairs
     */
    bool equals(const NonTerminalNonTerminal& nonTerminalNonTerminal) const;

private:
    NonTerminalPair _pair;
};
