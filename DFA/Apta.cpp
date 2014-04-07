#include "Apta.h"

const string Apta::ACCEPTED = "Accepted";
const string Apta::REJECTED = "Rejected";

Apta::A::A(const NodeEdges * nodeEdges)
{
    this->_nodeEdges = nodeEdges;
}

Apta::Apta() : _data(& _nodeEdges)
{
}

Apta::A Apta::get()
{
    return this->_data;
}

Apta::Nodes & Apta::getRedNodes()
{
    return this->_redNodes;
}

Apta::Nodes & Apta::getBlueNodes()
{
    return this->_blueNodes;
}

Apta::Nodes & Apta::getWhiteNodes()
{
    return this->_whiteNodes;
}

Apta::NodeEdges & Apta::getNodeEdges()
{
    return this->_nodeEdges;
}

Apta::NodeEdges & Apta::getNodeEdges2()
{
    return this->_nodeEdges2;
}

Apta::NodeLabels & Apta::getRedNodesLabels()
{
    return this->_redNodesLabels;
}

void Apta::build(TrainingSet trainingSet, bool useWhiteNodes)
{
    if (useWhiteNodes) {
        this->_useWhiteNodes = true;
    }

    // Start with the root of APTA colored red
    this->_addNode(true, this->_rootId, "", "", '\0');

    for (pair<string, bool> sample : trainingSet.get()) {
        this->_addPath(this->_rootId, sample.first,
            sample.second ? Apta::ACCEPTED : Apta::REJECTED);
    }

    // Build _data

    Q nodeIds;
    Fp acceptedNodes;
    Fm rejectedNodes;
    
    Nodes allNodes;
    allNodes.insert(this->_redNodes.begin(), this->_redNodes.end());
    allNodes.insert(this->_blueNodes.begin(), this->_blueNodes.end());
    allNodes.insert(this->_whiteNodes.begin(), this->_whiteNodes.end());

    for (pair<string, string> node : allNodes) {
        if (node.second == Apta::ACCEPTED) {
            acceptedNodes.insert(node.first);
        } else if (node.second == Apta::REJECTED) {
            rejectedNodes.insert(node.first);
        }
        nodeIds.insert(node.first);
    }

    this->_data.Q  = nodeIds;
    this->_data.Z  = this->_alphabet;
    this->_data.s  = this->_rootId;
    this->_data.Fp = acceptedNodes;
    this->_data.Fm = rejectedNodes;
}

string Apta::getLabelByNodeId(string nodeId)
{
    Apta::Nodes::iterator redNode = this->_redNodes.find(nodeId);
    if (redNode != this->_redNodes.end()) { // Found
        return redNode->second;
    }

    Apta::Nodes::iterator blueNode = this->_blueNodes.find(nodeId);
    if (blueNode != this->_blueNodes.end()) { // Found
        return blueNode->second;
    }

    Apta::Nodes::iterator whiteNode = this->_whiteNodes.find(nodeId);
    if (whiteNode != this->_whiteNodes.end()) { // Found
        return whiteNode->second;
    }

    assert(false && "This should never happen! Node not found!");
    return NULL;
}

void Apta::colorNodeRed(string nodeId)
{
    Apta::Nodes::iterator blueNode = this->_blueNodes.find(nodeId);
    if (blueNode == this->_blueNodes.end()) { // Not found
        assert(false && "This should never happen! Blue node does not exist!");
    }

    this->_redNodes.insert({ blueNode->first, blueNode->second });
    this->_redNodesLabels.push_back(blueNode->second);
    this->_blueNodes.erase(blueNode);
}

bool Apta::redNodeLabelExists(string nodeLabel)
{
    if (std::find(this->_redNodesLabels.begin(), this->_redNodesLabels.end(), nodeLabel) !=
        this->_redNodesLabels.end()) { // Found

        return true;
    }
    return false;
}

size_t Apta::getNumberRedNodesByLabel(string nodeLabel)
{
    size_t result = 0;
    for (string localNodeLabel : this->_redNodesLabels) {
        if (localNodeLabel == nodeLabel) {
            result++;
        }
    }
    return result;
}

string Apta::_getUniqueNodeId()
{
    this->_nodeIdAutoIncrement += 1;
    return /*"Node-" + */to_string(this->_nodeIdAutoIncrement);
}

void Apta::_addNode(bool isRed, string id, string label, string parentId, char edgeLabel)
{
    // Populate nodes map (red, blue or white)
    if (isRed) {
        this->_redNodes.insert({ id, label });
        this->_redNodesLabels.push_back(label);
    } else if (this->_useWhiteNodes && parentId != this->_rootId) {
        this->_whiteNodes.insert({ id, label });
    } else {
        this->_blueNodes.insert({ id, label });
    }

    // Populate node edges map
    if (parentId != "") {
        // If there is a parent node id, there should also be a node edge
        NodeEdges::iterator parentNodeEdges = this->_nodeEdges.find(parentId);
        if (parentNodeEdges != this->_nodeEdges.end()) { // Found
            parentNodeEdges->second.insert({ edgeLabel, id });
        } else { // Not found
            NodeChildren nodeChildren;
            nodeChildren.insert({ edgeLabel, id });
            this->_nodeEdges.insert({ parentId, nodeChildren });
        }
    }

    // Populate node edges 2 map
    NodeEdges::iterator nodeEdges2 = this->_nodeEdges2.find(id);
    if (nodeEdges2 != this->_nodeEdges2.end()) { // Found
        if (edgeLabel != '\0') {
            nodeEdges2->second.insert({ edgeLabel, parentId });
        }
    } else { // Not found
        NodeChildren localNodeChildren;
        if (edgeLabel != '\0') {
            localNodeChildren.insert({ edgeLabel, parentId });
        }
        this->_nodeEdges2.insert({ id, localNodeChildren });
    }
}

string Apta::_addPath(string nodeId, string sample, string terminalNodeLabel)
{
    // Terminal Node
    if (sample == "") {
        Nodes::iterator blueNodes = this->_blueNodes.find(nodeId);
        if (blueNodes != this->_blueNodes.end()) { // Found
            blueNodes->second = terminalNodeLabel;
        } else { // Not found
            this->_whiteNodes.find(nodeId)->second = terminalNodeLabel;
        }
        return nodeId;
    }

    char sampleFirst = sample[0];
    string sampleRest = sample.size() > 1 ? sample.substr(1) : "";
    string localNodeLabel = sampleRest.size() == 0 ? terminalNodeLabel : "";

    // Check if the first character of the sample exists or not as path
    NodeEdges::iterator nodeEdges = this->_nodeEdges.find(nodeId);
    if (nodeEdges != this->_nodeEdges.end()) { // Found
        NodeChildren nodeChildren = nodeEdges->second;
        for (pair<char, string> edgeLabelDestinationId : nodeChildren) {
            if (edgeLabelDestinationId.first != sampleFirst) {
                continue;
            }

            // Update path
            return this->_addPath(edgeLabelDestinationId.second, sampleRest, terminalNodeLabel);
        }
    }

    // Create path
    string localNodeId = this->_getUniqueNodeId();
    this->_addNode(false, localNodeId, localNodeLabel, nodeId, sampleFirst);

    this->_alphabet.insert(sampleFirst);
    return this->_addPath(localNodeId, sampleRest, terminalNodeLabel);
}

string Apta::A::d(string nodeId, char edgeLabel)
{
    NodeChildren nodeChildren = this->_nodeEdges->find(nodeId)->second;
    for (pair<char, string> edgeLabelDestinationId : nodeChildren) {
        if (edgeLabelDestinationId.first != edgeLabel) {
            continue;
        }
        return edgeLabelDestinationId.second;
    }

    return NULL;
}

Apta::~Apta()
{
}
