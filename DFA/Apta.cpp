#include "Apta.h"

Apta::Apta()
{
}

Apta::A Apta::get()
{
    return this->_data;
}

void Apta::build(TrainingSet trainingSet, bool useWhiteNodes)
{
    if (useWhiteNodes) {
        this->_useWhiteNodes = true;
    }

    // Start with the root of APTA colored red
    this->_addNode(true, this->_rootId, "", "", "");

    for (pair<string, bool> sample : trainingSet.get()) {
        this->_addPath(this->_rootId, sample.first, sample.second ? "Accepted" : "Rejected");
    }

    // Build _data

    Q nodeIds;
    Fp acceptedNodes;
    Fm rejectedNodes;
    
    for (pair<string, string> node : this->_redNodes) {
        if (node.second == "Accepted") {
            acceptedNodes.insert(node.first);
        } else {
            rejectedNodes.insert(node.first);
        }
        nodeIds.insert(node.first);
    }
    for (pair<string, string> node : this->_blueNodes) {
        if (node.second == "Accepted") {
            acceptedNodes.insert(node.first);
        } else {
            rejectedNodes.insert(node.first);
        }
        nodeIds.insert(node.first);
    }
    //TODO we don't need white nodes?

    this->_data = make_tuple(
        nodeIds,
        this->_alphabet,
        // this->_transitionFunction("", ""),
        this->_rootId,
        acceptedNodes,
        rejectedNodes
    );
}

string Apta::_getUniqueNodeId()
{
    this->_nodeIdAutoIncrement += 1;
    return "Node-" + this->_nodeIdAutoIncrement;
}

void Apta::_addNode(bool isRed, string id, string label, string parentId, string edgeLabel)
{
    //TODO rewrite this method
    if (isRed) {
        this->_redNodes.insert({ id, label });
        this->_redNodesLabels.push_back(label);
    } else {
        if ((this->_useWhiteNodes && parentId == this->_rootId) || !this->_useWhiteNodes) {
            this->_blueNodes.insert({ id, label });
        } else {
            this->_whiteNodes.insert({ id, label });
        }
    }

    if (parentId != "") {
        // If there is a parent node id, there should also be a node edge
        if (this->_nodeEdges.find(parentId) == this->_nodeEdges.end()) { // Not found
            NodeChildren nodeChildren;
            nodeChildren.insert({ edgeLabel, id });
            this->_nodeEdges.insert({ parentId, nodeChildren });
        } else {
            this->_nodeEdges.find(parentId)->second.insert({ edgeLabel, id });
        }
    }

    NodeChildren localNodeChildren;
    pair<string, string> edgeLabelDestinationId;
    if (edgeLabel != "") {
        edgeLabelDestinationId = { edgeLabel, parentId };
        localNodeChildren.insert(edgeLabelDestinationId);
    }

    if (this->_nodeEdges2.find(id) == this->_nodeEdges2.end()) { // Not found
        this->_nodeEdges2.insert({ id, localNodeChildren });
    } else {
        this->_nodeEdges2.find(id)->second.insert(edgeLabelDestinationId);
    }
}

string Apta::_addPath(string nodeId, string sample, string terminalNodeLabel)
{
    //TODO rewrite this method

    if (sample == "") { // Terminal Node
        if (this->_blueNodes.find(nodeId) != this->_blueNodes.end()) { // Found
            this->_blueNodes.find(nodeId)->second = terminalNodeLabel;
        } else {
            this->_whiteNodes.find(nodeId)->second = terminalNodeLabel;
        }
        return nodeId;
    }

    string sampleRest = sample.size() > 1 ? sample.substr(1) : "";
    string sampleFirst = sample.substr(0, 1);
    string localNodeLabel = sampleRest.size() == 0 ? terminalNodeLabel : "";

    string localNodeId;

    bool createPath = false;

    // Check if the first character of the sample exists or not as path
    if (this->_nodeEdges.find(nodeId) == this->_nodeEdges.end()) { // Not found
        createPath = true;
    } else { // Found
        NodeChildren nodeChildren = this->_nodeEdges.find(nodeId)->second;
        bool found = false;
        for (pair<string, string> edgeLabelDestinationId : nodeChildren) {
            if (edgeLabelDestinationId.second == sampleFirst) {
                found = true;
            }
        }
        if (!found) {
            createPath = true;
        }
    }

    if (createPath) {
        localNodeId = this->_getUniqueNodeId();
        this->_addNode(false, localNodeId, localNodeLabel, nodeId, sampleFirst);

        this->_alphabet.insert(sampleFirst);
    } else {
        // Update path
        // Get First Node Id By Label
        NodeChildren nodeChildren = this->_nodeEdges.find(nodeId)->second;
        for (pair<string, string> edgeLabelDestinationId : nodeChildren) {
            if (edgeLabelDestinationId.second == sampleFirst) {
                localNodeId = edgeLabelDestinationId.first;
            }
        }
    }

    return this->_addPath(localNodeId, sampleRest, terminalNodeLabel);
}

//TODO tie this to d
string &Apta::_transitionFunction(string nodeId, string edgeLabel)
{
    string localNodeId;
    NodeChildren nodeChildren = this->_nodeEdges.find(nodeId)->second;
    for (pair<string, string> edgeLabelDestinationId : nodeChildren) {
        if (edgeLabelDestinationId.first == edgeLabel) {
            localNodeId = edgeLabelDestinationId.second;
        }
    }

    return localNodeId; //TODO treat this
}

Apta::~Apta()
{
}
