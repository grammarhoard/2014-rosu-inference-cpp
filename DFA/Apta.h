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

#include <string>  // std::string
#include <set>     // std::set
#include <tuple>   // std::tuple, std::get, std::tie, std::ignore
#include <map>     // std::map
#include <list>    // std::list
#include <utility> // std::pair, std::make_pair

using namespace std;

#pragma once
class Apta
{
public:
    // Aliases
    typedef set<string> Q; // set of node ids
    typedef set<string> Z; // set of input symbols
    typedef string(d)(string, string); // transition function (nodeId, edgeLabel)
    typedef string s; // root node's id
    typedef set<string> Fp; // final nodes of strings in S+
    typedef set<string> Fm; // final nodes of strings in S-
    typedef tuple<Q, Z, /*string(string, string), */s, Fp, Fm> A;

    typedef map<string, string> Nodes; // map(id, label)
    typedef set<pair<string, string>> NodeChildren; // set[(edgeLabel, destinationId)]
    typedef map<string, NodeChildren> NodeEdges; // sourceId: NodeChildren

    Apta();
    A get();
    void build(TrainingSet trainingSet, bool useWhiteNodes); // Build APTA from Training Set starting with the Root Node
    ~Apta();
private:
    A _data;

    Nodes _whiteNodes; // Only used by EDSM algorithm
    Nodes _blueNodes;
    Nodes _redNodes;
    list<string> _redNodesLabels; // Only used by Exbar algorithm (pickBlueNode)
    NodeEdges _nodeEdges; // Source -> Destination (all colors) = used to get children
    NodeEdges _nodeEdges2; // Destination -> Source (all colors) = used to get parents
    set<string> _alphabet; // Set of input symbols
    string _rootId;
    int _nodeIdAutoIncrement = 0;

    // If this parameter is true, the root node is red,
    //     its first level children are blue and the rest of the nodes are white
    bool _useWhiteNodes = false;

    string _getUniqueNodeId();
    void _addNode(bool isRed, string id, string label, string parentId, string edgeLabel); //TODO rewrite this method

    // Add path to the tree (it goes recursively until the sample is consumed)
    //TODO rewrite this method
    string _addPath(string nodeId, string sample, string terminalNodeLabel);

    string &_transitionFunction(string nodeId, string edgeLabel); // Returns the node id following the specified edge from a source node
};

