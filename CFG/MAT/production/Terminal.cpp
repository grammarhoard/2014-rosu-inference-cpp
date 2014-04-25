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
