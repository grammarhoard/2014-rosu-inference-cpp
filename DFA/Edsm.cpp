#include "Edsm.h"

Edsm::Edsm(Apta apta, bool buildVisualizations, string visualizationPrefix) :
    AptaBasedAlgorithm(apta, buildVisualizations, visualizationPrefix)
{
    this->_minusInfinity = numeric_limits<int>::min();
}

void Edsm::search()
{
    set<tuple<string, string, int>> merges; // set(tuple(redNodeId, blueNodeId, mergingScore))
    int maxMergeScore = this->_minusInfinity;

    Apta::Nodes & blueNodes = this->_apta.getBlueNodes();
    Apta::Nodes::iterator blueNode;
    Apta::Nodes & redNodes = this->_apta.getRedNodes();
    Apta::Nodes::iterator redNode;

    // 1. Evaluate all red/blue merges
    for (blueNode = blueNodes.begin(); blueNode != blueNodes.end(); ++blueNode) {
        for (redNode = redNodes.begin(); redNode != redNodes.end(); ++redNode) {
            int mergeScore = this->_buildMergeScore(redNode->first, blueNode->first);
            merges.insert(make_tuple(redNode->first, blueNode->first, mergeScore));
        }

        int localMaxMergeScore = this->_minusInfinity;

        for (tuple<string, string, int> merge : merges) {
            int mergeScore = get<2>(merge);
            if (localMaxMergeScore < mergeScore) {
                localMaxMergeScore = mergeScore;
            }
            if (maxMergeScore < mergeScore) {
                maxMergeScore = mergeScore;
            }
        }

        // 2. If there exists a blue node that cannot be merged
        //     with any red node, promote it to red and go to step 1
        if (localMaxMergeScore == this->_minusInfinity) {
            LOG(DEBUG) << "Blue node '" << blueNode->first
                << "' cannot be merged with any red node, so it is promoted to red";

            this->_colorNodeRed(blueNode->first);
            this->search();
            return;
        }
    }

    // 3. If no blue node is promotable, perform the highest scoring
    //     red/blue merge and then go to step 1
    for (tuple<string, string, int> merge : merges) {
        if (get<2>(merge) == maxMergeScore) {
            string redNodeId = get<0>(merge);
            string blueNodeId = get<1>(merge);

            LOG(DEBUG) << "Merging red node '" << redNodeId
                << "' with blue node '" << blueNodeId << "'";

            this->_merge(redNodeId, blueNodeId);
            this->search();
            return;
        }
    }

    // 4. Halt
}

int Edsm::_buildMergeScore(string redNodeId, string blueNodeId)
{
    if (this->_apta.getLabelByNodeId(redNodeId) != this->_apta.getLabelByNodeId(blueNodeId)) {
        return this->_minusInfinity; // conflicting labels
    }

    int numberOfLabels = 0;
    Apta::NodeEdges nodeEdges = this->_apta.getNodeEdges();

    // Check determination rule (the children of equivalent nodes must be equivalent)
    Apta::NodeEdges::iterator redNode = nodeEdges.find(redNodeId);
    Apta::NodeEdges::iterator blueNode = nodeEdges.find(blueNodeId);
    if (redNode != nodeEdges.end() && blueNode != nodeEdges.end()) { // Both found
        // Both nodes have children
        for (pair<char, string> redNodeChild : redNode->second) {
            for (pair<char, string> blueNodeChild : blueNode->second) {
                if (redNodeChild.first == blueNodeChild.first &&
                    this->_apta.getLabelByNodeId(redNodeChild.second) !=
                    this->_apta.getLabelByNodeId(blueNodeChild.second)
                ) {
                    return this->_minusInfinity;
                } else {
                    numberOfLabels++;
                }
            }
        }
    }

    return numberOfLabels == 0 ? 0 : numberOfLabels - 1;
}

void Edsm::_colorNodeRed(string nodeId)
{
    this->_apta.colorNodeRed(nodeId);

    LOG(DEBUG) << "Blue node '" << nodeId << "' has been promoted to red";

    if (this->_buildVisualizations) {
        char outputFileName[_MAXSTRING];
        sprintf_s(outputFileName, _MAXSTRING, "%s_colored_red.svg",
            nodeId.c_str());
        this->_buildVisualization(outputFileName);
    }

    // Color its primary children to blue
    Apta::NodeEdges nodeEdges = this->_apta.getNodeEdges();
    Apta::NodeEdges::iterator nodeChildren = nodeEdges.find(nodeId);

    if (nodeChildren == nodeEdges.end()) { // Not found
        return;
    }

    Apta::Nodes & blueNodes = this->_apta.getBlueNodes();
    Apta::Nodes & whiteNodes = this->_apta.getWhiteNodes();
    for (pair<char, string> nodeChild : nodeChildren->second) {
        blueNodes.insert({ nodeChild.second, this->_apta.getLabelByNodeId(nodeChild.second) });
        whiteNodes.erase(nodeChild.second);
    }
}

void Edsm::_merge(string redNodeId, string blueNodeId)
{
    // Get the nodes pointing to the blue node and make them point the red node
    this->_switchParents(blueNodeId, redNodeId);

    // Get the blue nodes' children and make them children of the red node
    this->_switchChildren(blueNodeId, redNodeId, true);

    // Remove blue node
    this->_apta.getBlueNodes().erase(blueNodeId);

    if (this->_buildVisualizations) {
        char outputFileName[_MAXSTRING];
        sprintf_s(outputFileName, _MAXSTRING, "%s_merged_with_%s.svg",
            redNodeId.c_str(), blueNodeId.c_str());
        this->_buildVisualization(outputFileName);
    }
}

Edsm::~Edsm()
{
}
