#include "NonTerminalNonTerminal.h"

NonTerminalNonTerminal::NonTerminalNonTerminal(NonTerminalPair pair) : _pair(pair),
    ProductionRight(pair.first.getName() + pair.second.getName())
{
}

NonTerminalNonTerminal::~NonTerminalNonTerminal()
{
}

NonTerminalNonTerminal::NonTerminalPair NonTerminalNonTerminal::getPair() const
{
    return this->_pair;
}

bool NonTerminalNonTerminal::equals(const Terminal& terminal) const
{
    return false;
}

bool NonTerminalNonTerminal::equals(const NonTerminalNonTerminal& nonTerminalNonTerminal) const
{
    return this->_pair.first.getName() == nonTerminalNonTerminal.getPair().first.getName() &&
        this->_pair.second.getName() == nonTerminalNonTerminal.getPair().second.getName();
}
