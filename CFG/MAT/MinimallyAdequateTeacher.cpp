#include "MinimallyAdequateTeacher.h"

MinimallyAdequateTeacher::MinimallyAdequateTeacher(ContextFreeLanguage L): _L(L)
{
}

MinimallyAdequateTeacher::~MinimallyAdequateTeacher()
{
}

ContextFreeLanguage MinimallyAdequateTeacher::getLanguage()
{
    return this->_L;
}

bool MinimallyAdequateTeacher::Mem(string l, string u, string r)
{
    return this->_L.in(l + u + r);
}

bool MinimallyAdequateTeacher::Equiv(ContextFreeGrammar G)
{
    //TODO implement MinimallyAdequateTeacher::Equiv
    if (this->_counterExample == "") {
        this->_counterExample = "ab";
    } else if (this->_counterExample == "ab") {
        this->_counterExample = "aab";
    } else {

        this->_counterExample = "";
        return true;
    }
    return false;
}

string MinimallyAdequateTeacher::getCounterExample()
{
    return this->_counterExample;
}