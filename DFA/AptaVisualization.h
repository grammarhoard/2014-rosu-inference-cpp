#include <ogdf/basic/Graph.h>
#include <ogdf/basic/graph_generators.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/hypergraph/Hypergraph.h>

#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>

#include "Apta.h"

using namespace ogdf;

#pragma once
class AptaVisualization
{
public:
    AptaVisualization(const char * outputFileName);
    void build(Apta apta);
    void buildLegend();
    ~AptaVisualization();
private:
    const char * _outputFileName;
    map<string, node> _nodes; // id: node
    map<node, edge> _nodesReferencingThemselves; // used to redraw some edges

    double _width;
    double _height;

    //TODO maybe find a way to get rid of these parameters
    //TODO it is not that easy due to "Access violation reading location" exception
    void _createNodes(Apta apta, Graph & graph, GraphAttributes & graphAttributes);
    void _createNode(Graph & graph, GraphAttributes & graphAttributes,
        string id, string label, Color color);
    void _createEdges(Apta apta, Graph & graph, GraphAttributes & graphAttributes);
    void _draw(GraphAttributes & graphAttributes);

    void _drawEdgesForNodesReferencingThemselves(GraphAttributes & graphAttributes);
};

