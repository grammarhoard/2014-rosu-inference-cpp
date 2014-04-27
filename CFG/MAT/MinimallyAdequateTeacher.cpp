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

bool MinimallyAdequateTeacher::Equiv(const ContextFreeGrammar& G)
{
    //TODO implement MinimallyAdequateTeacher::Equiv()
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
