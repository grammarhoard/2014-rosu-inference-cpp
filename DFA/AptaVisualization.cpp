#include "AptaVisualization.h"

AptaVisualization::AptaVisualization()
{
}

void AptaVisualization::build(Apta apta)
{
    ogdf::Graph graph;
    ogdf::GraphAttributes graphAttributes(graph,
        ogdf::GraphAttributes::nodeGraphics |
        ogdf::GraphAttributes::nodeLabel |
        ogdf::GraphAttributes::nodeStyle |

        ogdf::GraphAttributes::edgeGraphics |
        ogdf::GraphAttributes::edgeLabel |
        ogdf::GraphAttributes::edgeStyle |
        ogdf::GraphAttributes::edgeType |
        ogdf::GraphAttributes::edgeArrow
    );

    this->_createNodes(apta, graph, graphAttributes);
    this->_createEdges(apta, graph, graphAttributes);
    this->_draw(graphAttributes);
}

void AptaVisualization::_createNodes(Apta apta,
    ogdf::Graph & graph, ogdf::GraphAttributes & graphAttributes)
{
    Apta::Nodes::iterator iterator;

    Apta::Nodes redNodes = apta.getRedNodes();
    for (iterator = redNodes.begin(); iterator != redNodes.end(); ++iterator) {
        this->_createNode(graph, graphAttributes, iterator->first,
            iterator->second, ogdf::Color("#FF0000"));
    }

    Apta::Nodes blueNodes = apta.getBlueNodes();
    for (iterator = blueNodes.begin(); iterator != blueNodes.end(); ++iterator) {
        this->_createNode(graph, graphAttributes, iterator->first,
            iterator->second, ogdf::Color("#00FFFF"));
    }

    Apta::Nodes whiteNodes = apta.getWhiteNodes();
    for (iterator = whiteNodes.begin(); iterator != whiteNodes.end(); ++iterator) {
        this->_createNode(graph, graphAttributes, iterator->first,
            iterator->second, ogdf::Color("#FFFFFF"));
    }
}

void AptaVisualization::_createNode(ogdf::Graph & graph, ogdf::GraphAttributes & graphAttributes,
    string id, string label, ogdf::Color color)
{
    ogdf::node node = graph.newNode();
    this->_nodes.insert({ id, node });
    graphAttributes.fillColor(node) = color;
    graphAttributes.label(node) = id;

    if (label == Apta::ACCEPTED) {
        graphAttributes.shape(node) = ogdf::Shape::shEllipse;
    } else if (label == Apta::REJECTED) {
        graphAttributes.shape(node) = ogdf::Shape::shHexagon;
    } else { // No label
        graphAttributes.shape(node) = ogdf::Shape::shRect;
    }
}

void AptaVisualization::_createEdges(Apta apta, ogdf::Graph & graph, ogdf::GraphAttributes & graphAttributes)
{
    Apta::NodeEdges::iterator iterator2;
    Apta::NodeEdges nodeEdges = apta.getNodeEdges();

    for (iterator2 = nodeEdges.begin(); iterator2 != nodeEdges.end(); ++iterator2) {
        ogdf::node node1 = this->_nodes.find(iterator2->first)->second;
        Apta::NodeChildren nodeChildren = iterator2->second;
        for (pair<char, string> edgeLabelDestinationId : nodeChildren) {
            ogdf::node node2 = this->_nodes.find(edgeLabelDestinationId.second)->second;
            ogdf::edge edge = graph.newEdge(node1, node2);
            graphAttributes.bends(edge);
            string label(&edgeLabelDestinationId.first, 1);
            graphAttributes.label(edge) = label;
            graphAttributes.strokeColor(edge) = ogdf::Color("#CCCCCC");
        }
    }
}

void AptaVisualization::_draw(ogdf::GraphAttributes graphAttributes)
{
    // Compute a hierarchical drawing of the graph (using SugiyamaLayout)
    ogdf::SugiyamaLayout sugiyamaLayout;
    sugiyamaLayout.setRanking(new ogdf::OptimalRanking);
    sugiyamaLayout.setCrossMin(new ogdf::MedianHeuristic);

    ogdf::OptimalHierarchyLayout *optimalHierarchyLayout = new ogdf::OptimalHierarchyLayout;

    sugiyamaLayout.setLayout(optimalHierarchyLayout);
    sugiyamaLayout.call(graphAttributes);

    ogdf::GraphIO::drawSVG(graphAttributes, "Output.svg");
}

AptaVisualization::~AptaVisualization()
{
}
