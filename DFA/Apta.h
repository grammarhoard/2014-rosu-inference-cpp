/**
 * Augmented Prefix Tree Acceptor (APTA)
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

#pragma once
class Apta
{
public:
    Apta();
    void build(TrainingSet trainingSet, bool useWhiteNodes);
    ~Apta();
};

