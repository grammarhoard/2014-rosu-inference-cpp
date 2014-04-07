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

    /*
     * Get the nodes pointing to the blue node and make them point the red node
     */
    void _switchParents(string blueNodeId, string redNodeId);

    /*
     * Get the blue nodes' children and make them children of the red node
     */
    void _switchChildren(string blueNodeId, string redNodeId, bool colorBlue);
};

