/**
 * Evidence Driven State Merging (EDSM)
 * Algorithm for DFA learning from given training data
 */

#include "AptaBasedAlgorithm.h"
#include "easylogging++.h"
#include <limits> // numeric_limits

#pragma once
class Edsm: public AptaBasedAlgorithm
{
public:
    Edsm(Apta apta, bool buildVisualizations, string visualizationPrefix = "");
    void search();
    ~Edsm();
private:
    int _minusInfinity;

    /*
     * Build merge score as the number of strings that end in the same state
     *    if that merge is done.
     * - conflicting labels: -infinity
     * - no labels: 0
     * - otherwise: number of labels - 1
     */
    int _buildMergeScore(string redNodeId, string blueNodeId);

    /*
    * Promote a blue node to red
    */
    void _colorNodeRed(string nodeId);

    void _merge(string redNodeId, string blueNodeId);
};
