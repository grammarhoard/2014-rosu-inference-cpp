#include "Apta.h"
#include "AptaVisualization.h"

#pragma once
class AptaBasedAlgorithm
{
public:
    AptaBasedAlgorithm(Apta apta, string visualizationPrefix);
    virtual void search() = 0;

    ~AptaBasedAlgorithm();
protected:
    Apta _apta;

    static const int _MAXSTRING = 1024;
    string _visualizationPrefix;
    size_t _visualizationId = 0;

    void _buildVisualization(string outputFileName);
};

