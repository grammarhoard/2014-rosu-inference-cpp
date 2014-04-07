/**
 * Evidence Driven State Merging (EDSM)
 * Algorithm for DFA learning from given training data
 */

#include "AptaBasedAlgorithm.h"
#include <limits> // numeric_limits

#pragma once
class Edsm: public AptaBasedAlgorithm
{
public:
    Edsm(Apta apta, string visualizationPrefix);
    void search();
    ~Edsm();
private:
    int _minusInfinity;
};
