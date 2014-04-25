#include <string>

#include "ProductionRight.h"

using namespace std;

#pragma once
class Terminal : public ProductionRight
{
public:
    Terminal(const string name);
    ~Terminal();

    string getName() const;

private:
    string _name;
};
