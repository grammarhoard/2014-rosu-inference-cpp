#include "Exbar.h"

Exbar::Exbar(Apta apta, bool buildVisualizations, string visualizationPrefix) :
    AptaBasedAlgorithm(apta, buildVisualizations, visualizationPrefix)
{
}

void Exbar::search()
{
    this->_maxRed += 1;

    Apta::Nodes & redNodes = this->_apta.getRedNodes();

    // Limits the number of searches to the number of red nodes
    if (redNodes.size() > this->_maxRed) {
        LOG(DEBUG) << "Limit exceeded: the number of redNodes (" << redNodes.size() << ") is greater than maxRed (" << this->_maxRed << ")";
        return;
    }

    // No blue nodes exist
    if (this->_apta.getBlueNodes().size() == 0) {
        // Found a solution
        LOG(DEBUG) << "Solution Found: No blue nodes exist";
        return;
    }

    pair<string, string> pickedBlueNode = this->_pickBlueNode(); // (id, label)
    LOG(DEBUG) << "Blue node picked: id = '" << pickedBlueNode.first << "', label = '" << pickedBlueNode.second << "'";

    // Try to merge with all red nodes that have the same label
    for (Apta::Nodes::iterator iterator = redNodes.begin(); iterator != redNodes.end(); ++iterator) {
        if (iterator->second != pickedBlueNode.second) {
            continue;
        }
        if (this->_tryMerge(iterator->first, pickedBlueNode.first)) {
            // Red node is successfully merged with blue node
            LOG(DEBUG) << "R: '" << iterator->first << "' merged successfully with B: '" << pickedBlueNode.first << "'";

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
    Apta::Nodes & blueNodes = this->_apta.getBlueNodes();
    Apta::Nodes::iterator iterator;

    if (this->_noPossibleMerges == 0) {
        // 1. Look for blue nodes that can't be merged with any red node
        Apta::NodeLabels & redNodesLabels = this->_apta.getRedNodesLabels();
        for (iterator = blueNodes.begin(); iterator != blueNodes.end(); ++iterator) {

            if (this->_apta.redNodeLabelExists(iterator->second)) { // Found
                continue;
            }
            // The blue node's label does not exist in the red node's label set
            // Force promotion to red
            this->_colorNodeRed(iterator->first);
            iterator = blueNodes.begin();

            //TODO maybe more criteria for finding this kind of nodes
        }

        this->_noPossibleMerges = 1;
    }

    // 2. Look for blue nodes that have only one or more possible merge(s)
    for (iterator = blueNodes.begin(); iterator != blueNodes.end(); iterator++) {

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
    LOG(DEBUG) << "Trying to merge red node '" << redNodeId << "' with blue node '" << blueNodeId << "'";

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
                    LOG(DEBUG) << "Merge failed! the nodes have children on a common " << "symbol that lead to nodes which are not equivalent";
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

    if (this->_buildVisualizations) {
        char outputFileName[_MAXSTRING];
        sprintf_s(outputFileName, _MAXSTRING, "%s_merged_with_%s.svg",
            redNodeId.c_str(), blueNodeId.c_str());
        this->_buildVisualization(outputFileName);
    }

    return true;
}

void Exbar::_colorNodeRed(string nodeId)
{
    this->_apta.colorNodeRed(nodeId);

    LOG(DEBUG) << "Blue node '" << nodeId << "' has been promoted to red";

    this->_maxRed++;

    if (this->_buildVisualizations) {
        char outputFileName[_MAXSTRING];
        sprintf_s(outputFileName, _MAXSTRING, "%s_colored_red.svg",
            nodeId.c_str());
        this->_buildVisualization(outputFileName);
    }
}

Exbar::~Exbar()
{
}
