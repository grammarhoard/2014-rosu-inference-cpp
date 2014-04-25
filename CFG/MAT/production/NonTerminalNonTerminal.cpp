#include "NonTerminalNonTerminal.h"

NonTerminalNonTerminal::NonTerminalNonTerminal(NonTerminalPair pair) : _pair(pair),
    ProductionRight(pair.first.getName() + pair.second.getName())
{
}

NonTerminalNonTerminal::~NonTerminalNonTerminal()
{
}

NonTerminalNonTerminal::NonTerminalPair NonTerminalNonTerminal::getPair()
{
    return this->_pair;
}
