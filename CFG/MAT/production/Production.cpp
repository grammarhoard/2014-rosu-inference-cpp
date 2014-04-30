#include "Production.h"

Production::Production(NonTerminal nonTerminal, ProductionRight* productionRight):
    left(nonTerminal), right(*productionRight)
{
}

Production::~Production()
{
}

bool Production::operator < (const Production& production) const
{
    if (this->left < production.left) {
        return true;
    }

    if (production.left < this->left) {
        return false;
    }

    // this->left == production.left
    return this->right < production.right;
}
