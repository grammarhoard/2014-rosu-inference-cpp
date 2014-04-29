#include "MinimallyAdequateTeacher.h"

MinimallyAdequateTeacher::MinimallyAdequateTeacher(Language& L): _L(L)
{
}

MinimallyAdequateTeacher::~MinimallyAdequateTeacher()
{
}

Language& MinimallyAdequateTeacher::getLanguage()
{
    return this->_L;
}

bool MinimallyAdequateTeacher::Mem(const string l, const string u, const string r)
{
    return this->_L.in(l + u + r);
}
