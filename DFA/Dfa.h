/**
 * Deterministic Finite Automata (DFA)
 * DFA A = <Q, Z, d, s, F> where
 *     Q = finite non-empty set of states
 *     Sigma Z = finite non-empty set of input symbols (alphabet)
 *     delta d : Q x Z -> Q = transition function
 *     s (element of) Q = start state (initial "reset state) <=> q0
 *     F (subset of) Q = set of final states or accepting states of A
 * Built on APTA
 */

#pragma once
class Dfa
{
public:
    Dfa();
    ~Dfa();
};
