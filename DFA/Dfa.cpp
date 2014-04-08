#include "Dfa.h"

Dfa::D::D(const Apta::NodeEdges * nodeEdges)
{
    this->_nodeEdges = nodeEdges;
}

Dfa::Dfa() : _data(&_nodeEdges)
{
}

Dfa::D Dfa::get()
{
    return this->_data;
}

void Dfa::build(Apta apta)
{
    apta.generateData();
    Apta::A a = apta.get();
    this->_nodeEdges = apta.getNodeEdges();
    this->_data.Q = a.Q;
    this->_data.Z = a.Z;
    this->_data.s = a.s;

    Dfa::F allNodes;
    allNodes.insert(a.Fp.begin(), a.Fp.end());
    allNodes.insert(a.Fm.begin(), a.Fm.end());

    this->_data.F = allNodes;
}

string Dfa::D::d(string nodeId, char edgeLabel)
{
    //TODO this is the same as for APTA (see if you can get rid of this duplication)
    Apta::NodeChildren nodeChildren = this->_nodeEdges->find(nodeId)->second;
    for (pair<char, string> edgeLabelDestinationId : nodeChildren) {
        if (edgeLabelDestinationId.first != edgeLabel) {
            continue;
        }
        return edgeLabelDestinationId.second;
    }

    return NULL;
}

Dfa::~Dfa()
{
}
