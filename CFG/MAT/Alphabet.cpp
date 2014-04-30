#include "Alphabet.h"

Alphabet::Alphabet()
{
}

Alphabet::~Alphabet()
{
}

void Alphabet::insert(const string element)
{
    this->_data.insert(element);
}

set<string>& Alphabet::get()
{
    return this->_data;
}

bool Alphabet::in(const string element)
{
    return this->_data.find(element) != this->_data.end();
}
