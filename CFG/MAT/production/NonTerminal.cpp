#include "NonTerminal.h"

NonTerminal::NonTerminal(const string name) : _name(name)
{
}

NonTerminal::~NonTerminal()
{
}

string NonTerminal::getName() const
{
    return this->_name;
}

bool NonTerminal::operator < (const NonTerminal& nonTerminal) const
{
    return this->_name < nonTerminal.getName();
}

bool NonTerminal::equals(const NonTerminal& nonTerminal) const
{
    return this->_name == nonTerminal.getName();
}
