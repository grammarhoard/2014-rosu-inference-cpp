#include "Alphabet.h"

Alphabet::Alphabet()
{
}

Alphabet::~Alphabet()
{
}

void Alphabet::insert(string element)
{
    this->_data.insert(element);
}

set<string> Alphabet::get()
{
    return this->_data;
}