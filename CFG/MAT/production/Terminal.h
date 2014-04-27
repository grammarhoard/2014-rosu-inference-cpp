#pragma once
#include <string>

#include "ProductionRight.h"
#include "NonTerminalNonTerminal.h"

using namespace std;

class Terminal : public ProductionRight
{
public:
    Terminal(const string name);
    ~Terminal();

    string getName() const;

    /*
     * Compares the names of the two terminals
     */
    bool equals(const Terminal& terminal) const;

    /*
     * Always returns false
     */
    bool equals(const NonTerminalNonTerminal& nonTerminalNonTerminal) const;

private:
    string _name;
};
