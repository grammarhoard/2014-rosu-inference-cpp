#include "AptaVisualization.h"

AptaVisualization::AptaVisualization(const char * outputFileName)
{
    this->_outputFileName = outputFileName;
    this->_width = 20;
    this->_height = 20;
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
    graphAttributes.setDirected(true);
    graphAttributes.setAllWidth(this->_width);
    graphAttributes.setAllHeight(this->_height);

    this->_createNodes(apta, graph, graphAttributes);
    this->_createEdges(apta, graph, graphAttributes);
    this->_draw(graphAttributes);
}

void AptaVisualization::buildLegend()
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

    graphAttributes.setAllWidth(this->_width);
    graphAttributes.setAllHeight(this->_height);

    // Accepted node
    node acceptedNode = graph.newNode();
    graphAttributes.shape(acceptedNode) = Shape::shEllipse;
    graphAttributes.x(acceptedNode) = 5;
    graphAttributes.y(acceptedNode) = 5;

    node acceptedNodeLabel = graph.newNode();
    graphAttributes.x(acceptedNodeLabel) = 75;
    graphAttributes.y(acceptedNodeLabel) = 10;
    graphAttributes.width(acceptedNodeLabel) = 70;
    graphAttributes.strokeColor(acceptedNodeLabel) = Color("#FFFFFF");
    graphAttributes.label(acceptedNodeLabel) = "Accepted Node";

    // Rejected node
    node rejectedNode = graph.newNode();
    graphAttributes.shape(rejectedNode) = Shape::shHexagon;
    graphAttributes.x(rejectedNode) = 5;
    graphAttributes.y(rejectedNode) = 30;

    node rejectedNodeLabel = graph.newNode();
    graphAttributes.x(rejectedNodeLabel) = 73;
    graphAttributes.y(rejectedNodeLabel) = 38;
    graphAttributes.width(rejectedNodeLabel) = 70;
    graphAttributes.strokeColor(rejectedNodeLabel) = Color("#FFFFFF");
    graphAttributes.label(rejectedNodeLabel) = "Rejected Node";

    // Unknown node
    node unknownNode = graph.newNode();
    graphAttributes.shape(unknownNode) = Shape::shRect;
    graphAttributes.x(unknownNode) = 5;
    graphAttributes.y(unknownNode) = 55;

    node unknownNodeLabel = graph.newNode();
    graphAttributes.x(unknownNodeLabel) = 75;
    graphAttributes.y(unknownNodeLabel) = 63;
    graphAttributes.width(unknownNodeLabel) = 70;
    graphAttributes.strokeColor(unknownNodeLabel) = Color("#FFFFFF");
    graphAttributes.label(unknownNodeLabel) = "Unknown Node";


    GraphIO::drawSVG(graphAttributes, this->_outputFileName);
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
    } else { // Unknown node
        graphAttributes.shape(node) = Shape::shRect;
    }
}

void AptaVisualization::_createEdges(Apta apta, Graph & graph, GraphAttributes & graphAttributes)
{
    Apta::NodeEdges::iterator iterator2;
    Apta::NodeEdges nodeEdges = apta.getNodeEdges();

    for (iterator2 = nodeEdges.begin(); iterator2 != nodeEdges.end(); ++iterator2) {
        string node1Id = iterator2->first;
        map<string, node>::iterator nodes = this->_nodes.find(node1Id);
        if (nodes == this->_nodes.end()) {
            continue;
        }
        node node1 = nodes->second;

        Apta::NodeChildren nodeChildren = iterator2->second;
        for (pair<char, string> edgeLabelDestinationId : nodeChildren) {
            string node2Id = edgeLabelDestinationId.second;
            node node2 = this->_nodes.find(node2Id)->second;
            edge edge = graph.newEdge(node1, node2);
            graphAttributes.bends(edge);
            string label(&edgeLabelDestinationId.first, 1);
            graphAttributes.label(edge) = label;
            graphAttributes.strokeColor(edge) = Color("#CCCCCC");

            if (node1Id == node2Id) {
                this->_nodesReferencingThemselves.insert({ node1, edge });
            }
        }
    }
}

void AptaVisualization::_draw(GraphAttributes & graphAttributes)
{
    // Compute a hierarchical drawing of the graph (using SugiyamaLayout)
    SugiyamaLayout sugiyamaLayout;
    sugiyamaLayout.setRanking(new OptimalRanking);
    sugiyamaLayout.setCrossMin(new MedianHeuristic);

    OptimalHierarchyLayout *optimalHierarchyLayout = new OptimalHierarchyLayout;

    sugiyamaLayout.setLayout(optimalHierarchyLayout);
    sugiyamaLayout.call(graphAttributes);

    this->_drawEdgesForNodesReferencingThemselves(graphAttributes);

    GraphIO::drawSVG(graphAttributes, this->_outputFileName);
}

void AptaVisualization::_drawEdgesForNodesReferencingThemselves(GraphAttributes & graphAttributes)
{
    map<node, edge>::iterator nodes;
    for (nodes = this->_nodesReferencingThemselves.begin();
        nodes != this->_nodesReferencingThemselves.end(); ++nodes
    ) {
        double x = graphAttributes.x(nodes->first);
        double y = graphAttributes.y(nodes->first);

        DPolyline &p = graphAttributes.bends(nodes->second);
        p.pushBack(DPoint(x + this->_width - 5, y + this->_height - 10));
        p.pushBack(DPoint(x + this->_width - 10, y + this->_height));
    }
}

AptaVisualization::~AptaVisualization()
{
}
