#include "AptaVisualization.h"

AptaVisualization::AptaVisualization()
{
}

void AptaVisualization::build(Apta apta)
{
    Graph graph;
    GraphAttributes graphAttributes(graph,
        GraphAttributes::nodeGraphics |
        GraphAttributes::nodeLabel |
        GraphAttributes::nodeStyle |

        GraphAttributes::edgeGraphics |
        GraphAttributes::edgeLabel |
        GraphAttributes::edgeStyle |
        GraphAttributes::edgeType |
        GraphAttributes::edgeArrow
    );

    this->_createNodes(apta, graph, graphAttributes);
    this->_createEdges(apta, graph, graphAttributes);
    this->_draw(graphAttributes);
}

void AptaVisualization::_createNodes(Apta apta,
    Graph & graph, GraphAttributes & graphAttributes)
{
    Apta::Nodes::iterator iterator;

    Apta::Nodes redNodes = apta.getRedNodes();
    for (iterator = redNodes.begin(); iterator != redNodes.end(); ++iterator) {
        this->_createNode(graph, graphAttributes, iterator->first,
            iterator->second, Color("#FF0000"));
    }

    Apta::Nodes blueNodes = apta.getBlueNodes();
    for (iterator = blueNodes.begin(); iterator != blueNodes.end(); ++iterator) {
        this->_createNode(graph, graphAttributes, iterator->first,
            iterator->second, Color("#00FFFF"));
    }

    Apta::Nodes whiteNodes = apta.getWhiteNodes();
    for (iterator = whiteNodes.begin(); iterator != whiteNodes.end(); ++iterator) {
        this->_createNode(graph, graphAttributes, iterator->first,
            iterator->second, Color("#FFFFFF"));
    }
}

void AptaVisualization::_createNode(Graph & graph, GraphAttributes & graphAttributes,
    string id, string label, Color color)
{
    node node = graph.newNode();
    this->_nodes.insert({ id, node });
    graphAttributes.fillColor(node) = color;
    graphAttributes.label(node) = id;

    if (label == Apta::ACCEPTED) {
        graphAttributes.shape(node) = Shape::shEllipse;
    } else if (label == Apta::REJECTED) {
        graphAttributes.shape(node) = Shape::shHexagon;
    } else { // No label
        graphAttributes.shape(node) = Shape::shRect;
    }
}

void AptaVisualization::_createEdges(Apta apta, Graph & graph, GraphAttributes & graphAttributes)
{
    Apta::NodeEdges::iterator iterator2;
    Apta::NodeEdges nodeEdges = apta.getNodeEdges();

    for (iterator2 = nodeEdges.begin(); iterator2 != nodeEdges.end(); ++iterator2) {
        node node1 = this->_nodes.find(iterator2->first)->second;
        Apta::NodeChildren nodeChildren = iterator2->second;
        for (pair<char, string> edgeLabelDestinationId : nodeChildren) {
            node node2 = this->_nodes.find(edgeLabelDestinationId.second)->second;
            edge edge = graph.newEdge(node1, node2);
            graphAttributes.bends(edge);
            string label(&edgeLabelDestinationId.first, 1);
            graphAttributes.label(edge) = label;
            graphAttributes.strokeColor(edge) = Color("#CCCCCC");
        }
    }
}

void AptaVisualization::_draw(GraphAttributes graphAttributes)
{
    // Compute a hierarchical drawing of the graph (using SugiyamaLayout)
    SugiyamaLayout sugiyamaLayout;
    sugiyamaLayout.setRanking(new OptimalRanking);
    sugiyamaLayout.setCrossMin(new MedianHeuristic);

    OptimalHierarchyLayout *optimalHierarchyLayout = new OptimalHierarchyLayout;

    sugiyamaLayout.setLayout(optimalHierarchyLayout);
    sugiyamaLayout.call(graphAttributes);

    GraphIO::drawSVG(graphAttributes, "Output.svg");
}

AptaVisualization::~AptaVisualization()
{
}
