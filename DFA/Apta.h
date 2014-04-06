/**
 * Augmented Prefix Tree Acceptor (APTA)
 *     tuple with 6 elements
 * APTA(S+, S-) A = <Q, Z, d, s, F+, F-> where
 *     S+ = set of positive samples
 *     S- = set of negative samples
 *     Q = finite non-empty set of nodes
 *     Sigma Z = finite non-empty set of input symbols (alphabet)
 *     delta d : Q x Z -> Q = transition function
 *     s (element of) Q = start (root node)
 *     F+ (subset of) Q = final nodes of strings in S+
 *     F- (subset of) Q = final nodes of strings in S-
 * size(A) = |Q| (total number of elements in Q / total number of nodes)
 * Built on Training Set
 * APTA is built from the beginning in the red-blue framework
 */

#include "TrainingSet.h"

#include <string>
#include <set>
#include <tuple>
#include <map>
#include <list>
#include <utility> // std::pair

using namespace std;

#pragma once
class Apta
{
public:
    static const string ACCEPTED;
    static const string REJECTED;

    typedef set<string> Q; // set of node ids
    typedef set<char> Z; // set of input symbols
    typedef string s; // root node's id
    typedef set<string> Fp; // final nodes of strings in S+
    typedef set<string> Fm; // final nodes of strings in S-

    typedef list<string> NodeLabels; // list because it can have duplicates
    typedef map<string, string> Nodes; // id: label
    typedef set<pair<char, string>> NodeChildren; // set[(edgeLabel, destinationId)]
    typedef map<string, NodeChildren> NodeEdges; // sourceId: NodeChildren

    class A
    {
    public:
        A(const NodeEdges * nodeEdges);

        Q Q;
        Z Z;

        /**
         * Returns the node id following the specified edge from a source node
         */
        string d(string nodeId, char edgeLabel);

        s s;
        Fp Fp;
        Fm Fm;
    private:
        const NodeEdges * _nodeEdges;
    };

    Apta();
    A get();
    Nodes getRedNodes();
    Nodes getBlueNodes();
    Nodes getWhiteNodes();

    NodeEdges getNodeEdges();
    NodeEdges getNodeEdges2();

    NodeLabels getRedNodesLabels();

    /*
     * Build APTA from Training Set starting with the Root Node
     */
    void build(TrainingSet trainingSet, bool useWhiteNodes);

    ~Apta();
private:
    A _data;

    Nodes _whiteNodes; // Only used by EDSM algorithm
    Nodes _blueNodes;
    Nodes _redNodes;
    NodeLabels _redNodesLabels; // Only used by Exbar algorithm (pickBlueNode)
    NodeEdges _nodeEdges; // Source -> Destination (all colors) = used to get children
    NodeEdges _nodeEdges2; // Destination -> Source (all colors) = used to get parents
    Z _alphabet; // Set of input symbols
    string _rootId = "R";
    int _nodeIdAutoIncrement = 0;

    /*
     * If this parameter is true, the root node is red,
     *     its first level children are blue and the rest of the nodes are white
     * If this parameter is false, the root node is red and
     *     its children are blue
     */
    bool _useWhiteNodes = false;

    string _getUniqueNodeId();
    void _addNode(bool isRed, string id, string label, string parentId, char edgeLabel);

    /*
     * Add path to the tree (it goes recursively until the sample is consumed)
     */
    string _addPath(string nodeId, string sample, string terminalNodeLabel);
};
