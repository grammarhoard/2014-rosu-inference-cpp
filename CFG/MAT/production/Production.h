#pragma once

#include "NonTerminal.h"
#include "ProductionRight.h"

class Production
{
public:
    NonTerminal left;
    ProductionRight& right;

    Production(NonTerminal nonTerminal, ProductionRight* productionRight);
    ~Production();

    /*
     * We need this operator to add a Production to a set
     */
    bool operator < (const Production& production) const;
};

