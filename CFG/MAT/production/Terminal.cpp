#include "Terminal.h"

Terminal::Terminal(const string name) : _name(name), ProductionRight(name)
{
}

Terminal::~Terminal()
{
}

string Terminal::getName() const
{
    return this->_name;
}

bool Terminal::equals(const Terminal& terminal) const
{
    return this->_name == terminal.getName();
}

bool Terminal::equals(const NonTerminalNonTerminal& nonTerminalNonTerminal) const
{
    return false;
}
