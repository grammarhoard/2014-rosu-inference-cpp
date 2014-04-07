#include "Edsm.h"

Edsm::Edsm(Apta apta, string visualizationPrefix) : AptaBasedAlgorithm(apta, visualizationPrefix)
{
    this->_minusInfinity = numeric_limits<int>::min();
}

void Edsm::search()
{
    string a = "";
}

Edsm::~Edsm()
{
}
