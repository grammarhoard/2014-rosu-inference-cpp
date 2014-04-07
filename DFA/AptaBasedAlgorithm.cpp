#include "AptaBasedAlgorithm.h"

AptaBasedAlgorithm::AptaBasedAlgorithm(Apta apta, string visualizationPrefix)
{
    this->_apta = apta;
    this->_visualizationPrefix = visualizationPrefix;
}

void AptaBasedAlgorithm::_buildVisualization(string outputFileName)
{
    string outputName = this->_visualizationPrefix +
        to_string(this->_visualizationId) + "_" + outputFileName;
    AptaVisualization aptaVisualization(outputName.c_str());
    aptaVisualization.build(this->_apta);
    this->_visualizationId++;
}

AptaBasedAlgorithm::~AptaBasedAlgorithm()
{
}
