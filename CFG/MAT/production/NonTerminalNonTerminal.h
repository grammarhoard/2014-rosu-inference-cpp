#include <utility>

#include "ProductionRight.h"
#include "NonTerminal.h"

using namespace std;

#pragma once
class NonTerminalNonTerminal: public ProductionRight
{
public:
    typedef pair<NonTerminal, NonTerminal> NonTerminalPair;

    NonTerminalNonTerminal(NonTerminalPair pair);
    ~NonTerminalNonTerminal();

    NonTerminalPair getPair();

private:
    NonTerminalPair _pair;
};
