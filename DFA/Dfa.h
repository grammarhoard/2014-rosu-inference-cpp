/**
 * Deterministic Finite Automata (DFA)
 * DFA D = <Q, Z, d, s, F> where
 *     Q = finite non-empty set of states
 *     Sigma Z = finite non-empty set of input symbols (alphabet)
 *     delta d : Q x Z -> Q = transition function
 *     s (element of) Q = start state (initial "reset state) <=> q0
 *     F (subset of) Q = set of final states or accepting states of A
 * Built on APTA
 */

#include "Apta.h"

#pragma once
class Dfa
{
public:
    typedef set<string> F;

    class D
    {
    public:
        D(const Apta::NodeEdges * nodeEdges);

        Apta::Q Q; // set of states (node ids)
        Apta::Z Z; // set of input symbols

        /**
        * Returns the node id following the specified edge from a source node
        */
        string d(string nodeId, char edgeLabel);

        Apta::s s; // start state (root node's id)
        F F; // final states (final node ids)
    private:
        const Apta::NodeEdges * _nodeEdges;
    };

    Dfa();
    D get();
    void build(Apta apta);
    ~Dfa();
private:
    D _data;
    Apta::NodeEdges _nodeEdges; // Source -> Destination (all colors) = used to get children
};
