#include "Exbar.h"

Exbar::Exbar(Apta apta, string visualizationPrefix) : AptaBasedAlgorithm(apta, visualizationPrefix)
{
}

void Exbar::search()
{
    this->_maxRed += 1;

    Apta::Nodes redNodes = this->_apta.getRedNodes();

    // Limits the number of searches to the number of red nodes
    if (redNodes.size() > this->_maxRed) {
        printf("Limit exceeded: the number of redNodes (%i) is greater than maxRed (%i)!\n",
            redNodes.size(), this->_maxRed
        );
        return;
    }

    // No blue nodes exist
    if (this->_apta.getBlueNodes().size() == 0) {
        // Found a solution
        printf("Solution Found: No blue nodes exist!\n");
        return;
    }

    pair<string, string> pickedBlueNode = this->_pickBlueNode(); // (id, label)
    printf("Blue node picked: id='%s', label='%s'!\n",
        pickedBlueNode.first.c_str(), pickedBlueNode.second.c_str());

    // Try to merge with all red nodes that have the same label
    for (Apta::Nodes::iterator iterator = redNodes.begin(); iterator != redNodes.end(); ++iterator) {
        if (iterator->second != pickedBlueNode.second) {
            continue;
        }
        if (this->_tryMerge(iterator->first, pickedBlueNode.first)) {
            // Red node is successfully merged with blue node
            printf("R: '%s' merged successfully with B: '%s'!\n",
                iterator->first.c_str(), pickedBlueNode.first.c_str());

            // Continue to search
            this->search();
            return;
        }
    }

    // The blue node could not be merged with neither of the red nodes
    // Force promotion to red
    this->_colorNodeRed(pickedBlueNode.first);

    // Continue to search
    this->search();
}

pair<string, string> Exbar::_pickBlueNode() // (id, label)
{
    Apta::Nodes blueNodes = this->_apta.getBlueNodes();

    if (this->_noPossibleMerges == 0) {
        // 1. Look for blue nodes that can't be merged with any red node
        Apta::NodeLabels redNodesLabels = this->_apta.getRedNodesLabels();
        for (Apta::Nodes::iterator iterator = blueNodes.begin();
            iterator != blueNodes.end(); ++iterator) {

            if (this->_apta.redNodeLabelExists(iterator->second)) { // Found
                continue;
            }
            // The blue node's label does not exist in the red node's label set
            // Force promotion to red
            this->_colorNodeRed(iterator->first);

            //TODO maybe more criteria for finding this kind of nodes
        }

        this->_noPossibleMerges = 1;
    }

    // 2. Look for blue nodes that have only one or more possible merge(s)
    for (Apta::Nodes::iterator iterator = blueNodes.begin();
        iterator != blueNodes.end(); ++iterator) {

        // Get possible number of merges (number of red nodes with same label)
        size_t localNoPossibleMerges = this->_apta.getNumberRedNodesByLabel(iterator->second);

        if (localNoPossibleMerges > this->_noPossibleMerges) {
            continue;
        }

        if (localNoPossibleMerges == 0) {
            // Force promotion to red
            this->_colorNodeRed(iterator->first);
            continue;
        }

        // Found blue node
        return make_pair(iterator->first, iterator->second);
    }

    // Increment and try again
    this->_noPossibleMerges++;
    return this->_pickBlueNode();
}

bool Exbar::_tryMerge(string redNodeId, string blueNodeId)
{
    printf("Trying to merge red node '%s' with blue node '%s'!\n",
        redNodeId.c_str(), blueNodeId.c_str());

    Apta::NodeEdges & nodeEdges = this->_apta.getNodeEdges();
    Apta::NodeEdges & nodeEdges2 = this->_apta.getNodeEdges2();

    // If the nodes have transitions on a common symbol
    //     that lead to nodes which are not equivalent, merge is not allowed
    Apta::NodeEdges::iterator redNode = nodeEdges.find(redNodeId);
    Apta::NodeEdges::iterator blueNode = nodeEdges.find(blueNodeId);
    if (redNode != nodeEdges.end() &&  blueNode != nodeEdges.end()) { // Both found
        // Both nodes have children
        for (pair<char, string> redNodeChild : redNode->second) {
            for (pair<char, string> blueNodeChild : blueNode->second) {
                if (redNodeChild.first == blueNodeChild.first &&
                    this->_apta.getLabelByNodeId(redNodeChild.second) !=
                    this->_apta.getLabelByNodeId(blueNodeChild.second)
                ) {
                    printf("Merge failed! the nodes have children on a common %s",
                        "symbol that lead to nodes which are not equivalent!\n");
                    return false;
                }
            }
        }
    }

    // Get the nodes pointing to the blue node and make them point the red node
    this->_switchParents(blueNodeId, redNodeId);

    // Get the blue nodes' children and make them children of the red node
    this->_switchChildren(blueNodeId, redNodeId, false);

    // Remove blue node
    this->_apta.getBlueNodes().erase(blueNodeId);

    char outputFileName[_MAXSTRING];
    sprintf_s(outputFileName, _MAXSTRING, "%s_merged_with_%s.svg",
        redNodeId.c_str(), blueNodeId.c_str());
    this->_buildVisualization(outputFileName);

    return true;
}

void Exbar::_colorNodeRed(string nodeId)
{
    this->_apta.colorNodeRed(nodeId);

    printf("Blue node '%s' has been promoted to red!\n", nodeId.c_str());

    this->_maxRed++;

    char outputFileName[_MAXSTRING];
    sprintf_s(outputFileName, _MAXSTRING, "%s_colored_red.svg",
        nodeId.c_str());
    this->_buildVisualization(outputFileName);
}

Exbar::~Exbar()
{
}
