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

Apta::Nodes Apta::getRedNodes()
{
    return this->_redNodes;
}

Apta::Nodes Apta::getBlueNodes()
{
    return this->_blueNodes;
}

Apta::Nodes Apta::getWhiteNodes()
{
    return this->_whiteNodes;
}

Apta::NodeEdges Apta::getNodeEdges()
{
    return this->_nodeEdges;
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
        if (label != "") {
            this->_redNodesLabels.push_back(label);
        }
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
