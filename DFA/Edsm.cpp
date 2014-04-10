#include "Edsm.h"

Edsm::Edsm(Apta apta, bool buildVisualizations, string visualizationPrefix) :
    AptaBasedAlgorithm(apta, buildVisualizations, visualizationPrefix)
{
    this->_minusInfinity = numeric_limits<int>::min();
    this->_maxScore = this->_minusInfinity;
}

void Edsm::search()
{
    MergePair mergePair;

    Apta::Nodes & blueNodes = this->_apta.getBlueNodes();
    Apta::Nodes & redNodes = this->_apta.getRedNodes();
    Apta::Nodes::iterator blueNode;
    Apta::Nodes::iterator redNode;

    list<string> blueNodesToPromote;

    // 1. Evaluate all red/blue merges
    // Build all merge scores
    for (blueNode = blueNodes.begin(); blueNode != blueNodes.end(); ++blueNode) {
        int localMaxMergeScore = this->_minusInfinity;
        for (redNode = redNodes.begin(); redNode != redNodes.end(); ++redNode) {
            int score = this->_buildMergeScore(redNode->first, blueNode->first);
            if (score > this->_maxScore) {
                this->_maxScore = score;
            }
            mergePair = make_pair(redNode->first, blueNode->first);
            this->_mergeScores.insert({mergePair, score});
        }

        if (localMaxMergeScore == this->_minusInfinity) {
            blueNodesToPromote.push_back(blueNode->first);
        }
    }

    // 2. If a blue node that cannot be merged
    //     with any red node exists, promote it to red and start over
    this->_promoteBlueNodes(blueNodesToPromote);

    this->_search();
}

void Edsm::_search()
{
    // 3. If no blue node is promotable, perform the highest scoring
    //     red/blue merge and then go to step 1
    MergeScores::iterator mergeScore;
    for (mergeScore = this->_mergeScores.begin(); mergeScore != this->_mergeScores.end(); ++mergeScore) {
        if (mergeScore->second != this->_maxScore) {
            continue;
        }

        string redNodeId = mergeScore->first.first;
        string blueNodeId = mergeScore->first.second;

        if (this->_maxScore == this->_minusInfinity) {
            LOG(DEBUG) << "Blue node '" << blueNodeId << "' cannot be merged with any red node, so it is promoted to red";
            this->_colorNodeRed(blueNodeId);
            this->_search();
            return;
        }

        LOG(DEBUG) << "Merging red node '" << redNodeId << "' with blue node '" << blueNodeId << "'";
        this->_merge(redNodeId, blueNodeId);

        this->_search();
        return;
    }

    // 4. Halt
}

void Edsm::_promoteBlueNodes(list<string> blueNodesToPromote)
{
    string firstBlueNode = blueNodesToPromote.front();
    blueNodesToPromote.pop_front();
    LOG(DEBUG) << "Blue node '" << firstBlueNode << "' cannot be merged with any red node, so it is promoted to red";
    this->_colorNodeRed(firstBlueNode);

    // Check the rest of the blue nodes to see if something changed
    for (string secondBlueNode : blueNodesToPromote) {
        int score = this->_buildMergeScore(firstBlueNode, secondBlueNode);
        if (score == this->_minusInfinity) {
            // Color it
            this->_promoteBlueNodes(blueNodesToPromote);
            return;
        }
    }
}

int Edsm::_buildMergeScore(string redNodeId, string blueNodeId)
{
    if (this->_apta.getLabelByNodeId(redNodeId) != this->_apta.getLabelByNodeId(blueNodeId)) {
        return this->_minusInfinity; // conflicting labels
    }

    int numberOfLabels = 0;
    Apta::NodeEdges & nodeEdges = this->_apta.getNodeEdges();

    Apta::NodeEdges::iterator redNode = nodeEdges.find(redNodeId);
    Apta::NodeEdges::iterator blueNode = nodeEdges.find(blueNodeId);

    // Check determination rule (the children of equivalent nodes must be equivalent)
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

    int score = numberOfLabels == 0 ? 0 : numberOfLabels - 1;

    if (score > this->_maxScore) {
        this->_maxScore = score;
    }
    return numberOfLabels == 0 ? 0 : numberOfLabels - 1;
}

void Edsm::_colorNodeRed(string nodeId)
{
    this->_apta.colorNodeRed(nodeId);
    LOG(DEBUG) << "Blue node '" << nodeId << "' has been promoted to red";

    // Color its primary children to blue
    Apta::Nodes & redNodes = this->_apta.getRedNodes();
    Apta::Nodes & blueNodes = this->_apta.getBlueNodes();
    Apta::Nodes::iterator iterator;
    Apta::NodeEdges & nodeEdges = this->_apta.getNodeEdges();
    Apta::NodeEdges::iterator nodeChildren = nodeEdges.find(nodeId);
    MergePair mergePair;

    // Rebuild the merge scores involving this node id
    for (iterator = redNodes.begin(); iterator != redNodes.end(); ++iterator) {
        mergePair = make_pair(iterator->first, nodeId);
        this->_mergeScores.erase(mergePair);
    }
    for (iterator = blueNodes.begin(); iterator != blueNodes.end(); ++iterator) {
        int score = this->_buildMergeScore(nodeId, iterator->first);
        mergePair = make_pair(nodeId, iterator->first);
        this->_mergeScores.insert({ mergePair, score });
    }

    if (nodeChildren != nodeEdges.end()) { // Found
        Apta::Nodes & whiteNodes = this->_apta.getWhiteNodes();
        for (pair<char, string> nodeChild : nodeChildren->second) {
            string label = this->_apta.getLabelByNodeId(nodeChild.second);
            blueNodes.insert({ nodeChild.second, label });
            whiteNodes.erase(nodeChild.second);

            for (iterator = redNodes.begin(); iterator != redNodes.end(); ++iterator) {
                int score = this->_buildMergeScore(iterator->first, nodeChild.second);
                mergePair = make_pair(iterator->first, nodeChild.second);
                this->_mergeScores.insert({ mergePair, score });
            }
        }
    }

    // Build Visualization
    if (this->_buildVisualizations) {
        char outputFileName[_MAXSTRING];
        sprintf_s(outputFileName, _MAXSTRING, "%s_colored_red.svg",
            nodeId.c_str());
        this->_buildVisualization(outputFileName);
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

    // Rebuild the merge scores involving these node ids
    Apta::Nodes & redNodes = this->_apta.getRedNodes();
    Apta::Nodes & blueNodes = this->_apta.getBlueNodes();
    Apta::Nodes::iterator iterator;
    MergePair mergePair;

    for (iterator = redNodes.begin(); iterator != redNodes.end(); ++iterator) {
        mergePair = make_pair(iterator->first, blueNodeId);
        this->_mergeScores.erase(mergePair);
    }
    for (iterator = blueNodes.begin(); iterator != blueNodes.end(); ++iterator) {
        int score = this->_buildMergeScore(redNodeId, iterator->first);
        mergePair = make_pair(redNodeId, iterator->first);
        this->_mergeScores.insert({ mergePair, score });
    }

    // Calculate max score
    MergeScores::iterator mergeScore;
    int localMaxScore = this->_minusInfinity;
    for (mergeScore = this->_mergeScores.begin(); mergeScore != this->_mergeScores.end(); ++mergeScore) {
        if (mergeScore->second > localMaxScore) {
            localMaxScore = mergeScore->second;
        }
    }
    this->_maxScore = localMaxScore;

    if (this->_buildVisualizations) {
        char outputFileName[_MAXSTRING];
        sprintf_s(outputFileName, _MAXSTRING, "%s_merged_with_%s.svg",
            redNodeId.c_str(), blueNodeId.c_str());
        this->_buildVisualization(outputFileName);
    }
}

void Edsm::_switchChildren(string blueNodeId, string redNodeId, bool colorBlue)
{
    Apta::Nodes & redNodes = this->_apta.getRedNodes();
    Apta::Nodes & blueNodes = this->_apta.getBlueNodes();
    Apta::Nodes & whiteNodes = this->_apta.getWhiteNodes();

    Apta::NodeEdges & nodeEdges = this->_apta.getNodeEdges();
    Apta::NodeEdges & nodeEdges2 = this->_apta.getNodeEdges2();

    Apta::NodeEdges::iterator & blueNodesChildrenIterator = nodeEdges.find(blueNodeId);
    if (blueNodesChildrenIterator == nodeEdges.end()) { // Not found
        return;
    }

    Apta::NodeChildren localRedNodesChildren;

    Apta::NodeChildren & blueNodesChildren = blueNodesChildrenIterator->second;
    for (Apta::NodeChildren::iterator & blueNodesChild = blueNodesChildren.begin();
        blueNodesChild != blueNodesChildren.end();
        ) {

        // Color them to blue
        if (colorBlue == true) {
            string blueNodeLabel = this->_apta.getLabelByNodeId(blueNodesChild->second);
            blueNodes.insert({ blueNodesChild->second, blueNodeLabel });
            whiteNodes.erase(blueNodesChild->second);
        }

        // Create merges for the newly colored into blue nodes
        Apta::Nodes::iterator iterator;
        MergePair mergePair;

        for (iterator = redNodes.begin(); iterator != redNodes.end(); ++iterator) {
            int score = this->_buildMergeScore(iterator->first, blueNodesChild->second);
            mergePair = make_pair(iterator->first, blueNodesChild->second);
            this->_mergeScores.insert({ mergePair, score });
        }

        Apta::NodeEdges::iterator & redNodesChildren = nodeEdges.find(redNodeId);

        if (redNodesChildren != nodeEdges.end()) { // Found
            pair<char, string> newChild = make_pair(blueNodesChild->first, blueNodesChild->second);
            redNodesChildren->second.insert(newChild);
        } else {
            localRedNodesChildren.clear();
            localRedNodesChildren.insert({ blueNodesChild->first, blueNodesChild->second });
            nodeEdges.insert({ redNodeId, localRedNodesChildren });
        }

        Apta::NodeChildren & localBlueNodesParent = nodeEdges2.find(blueNodesChild->second)->second;
        localBlueNodesParent.insert({ blueNodesChild->first, redNodeId });
        localBlueNodesParent.erase({ blueNodesChild->first, blueNodeId });

        blueNodesChildren.erase(blueNodesChild++);
    }
}

Edsm::~Edsm()
{
}
