/**
 * Algorithm for the exact inference of minimal DFA
 */

#include "AptaBasedAlgorithm.h"
#include <assert.h>

#pragma once
class Exbar: public AptaBasedAlgorithm
{
public:
    Exbar(Apta apta, string visualizationPrefix);
    void search();
    ~Exbar();
private:
    size_t _noPossibleMerges = 0; // Current number of possible merges
    size_t _maxRed = 0; // Limits the number of searches to the number of red nodes

    /*
     * Picks a blue node that can be disposed of in the fewest ways:
     *     the preferred kind of blue node can't be merged with any red node,
     *         which means force promotion to red
     *     the next best kind of blue node has only one possible merge,
     *         resulting in two choices to be searched
     *     etc.
     */
    pair<string, string> _pickBlueNode();

    /*
     * Return true on success and false on failure
     */
    bool _tryMerge(string redNodeId, string blueNodeId);

    /*
     * Promote a blue node to red
     */
    void _colorNodeRed(string nodeId);
};
