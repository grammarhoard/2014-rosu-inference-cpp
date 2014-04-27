#pragma once
#include <string>

using namespace std;

class NonTerminal
{
public:
    NonTerminal(const string name);
    ~NonTerminal();

    string getName() const;

    /*
     * We need this operator to add a NonTerminal to a set
     */
    bool operator < (const NonTerminal& nonTerminal) const;

private:
    string _name;
};
