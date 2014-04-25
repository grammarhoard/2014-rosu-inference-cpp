#include "ProductionRight.h"

ProductionRight::ProductionRight(const string name) : _name(name)
{
}

ProductionRight::~ProductionRight()
{
}

string ProductionRight::getName() const
{
    return this->_name;
}

bool ProductionRight::operator < (const ProductionRight& productionRight) const
{
    return this->_name < productionRight.getName();
}

bool ProductionRight::operator == (const ProductionRight& productionRight) const
{
    return this->getName() == productionRight.getName();
}
