#include <ogdf/basic/Graph.h>
#include <ogdf/basic/graph_generators.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/hypergraph/Hypergraph.h>

#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>

#include "Apta.h"

using namespace std;

#pragma once
class AptaVisualization
{
public:
    AptaVisualization();
    void build(Apta apta);
    ~AptaVisualization();
private:
    map<string, ogdf::node> _nodes; // id: node

    void _createNodes(Apta apta, ogdf::Graph & graph, ogdf::GraphAttributes & graphAttributes);
    void _createNode(ogdf::Graph & graph, ogdf::GraphAttributes & graphAttributes,
        string id, string label, ogdf::Color color);
    void _createEdges(Apta apta, ogdf::Graph & graph, ogdf::GraphAttributes & graphAttributes);
    void _draw(ogdf::GraphAttributes graphAttributes);
};

