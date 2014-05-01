/*
 * Observation Table
 * The basic data structure used for learning the CFG
 * We write it as a tuple <K, D, F>, where:
 *    K - a non-empty finite set of strings
 *    F - a non-empty finite set of contexts
 *    D - a set of grammatical strings in F \odot KK (finite function mapping to {false, true})
 */
#pragma once

#include <string>
#include <set>
#include <map>
#include <utility>
#include <fstream>

#include "MinimallyAdequateTeacher.h"
#include "ContextFreeGrammar.h"

#include "Terminal.h"
#include "NonTerminal.h"
#include "NonTerminalNonTerminal.h"
#include "ProductionRight.h"

using namespace std;

class ObservationTable
{
public:
    typedef set<string> StringSet;

    typedef pair<string, string> Context; // pair of strings (l, r)
    typedef set<Context> ContextSet;

    typedef map<string, map<Context, bool>> Table; // map(string k: (Context f, true or false))

    typedef pair<ContextSet, StringSet> EquivalenceClass;  // pair(distribution, set of strings)
    typedef map<ContextSet, StringSet> EquivalenceClasses; // map(distribution: set of strings)

    StringSet  K;  // non-empty finite set of strings
    ContextSet F;  // non-empty finite set of contexts
    StringSet  D;  // in the finite function mapping F \odot KK to {0, 1}

    ObservationTable(MinimallyAdequateTeacher& mat);
    ~ObservationTable();

    /*
     * Compute the Cartesian product K x K and save it in KK
     */
    void computeKK();

    /*
     * The insertion or wrapping operation (combines a context f = (l, r) with a substring u)
     */
    void insert(const Context f, const string u);

    /*
     * Build table and populate D from K and F
     */
    void build();

    /*
     * Returns true if they appear in the same set of contexts F,
     *     and false otherwise
     */
    bool equivalent(const string u, const string v);

    /*
     * Algorithm 1
     * Selects appropriate contexts to make sure that the table is consistent
     */
    void MakeConsistent();

    /*
     * Algorithm 2
     * Constructs a grammar from the observation table in polynomial time and
     *     returns it
     */
    ContextFreeGrammar MakeGrammar();

    /*
     * Add positive counter-example
     * Add Sub(w) to K, where Sub(w) is a set of all possible substrings of w
     */
    void addPositiveCounterExample(const string w);

    /*
     * Returns the set of all substrings of a string w
     */
    StringSet getSub(const string w);

    /*
     * Returns true if context f can split category X
     *     (there are u, v in X such that lux in L and lvx not in L), and
     *     false otherwise
     */
    bool contextSplitsCategory(ContextFreeGrammar& G,
        const Context f, const NonTerminal X);

    /*
     * Algorithm 3
     * Returns a context that splits some category X of grammar G
     */
    Context FindContext(ContextFreeGrammar& G,
        const NonTerminal X, const Context f, const string w);

    /*
     * Algorithm 4
     * Add contexts until the derived grammar no longer generates string w
     * w is not in L
     */
    void AddContexts(ContextFreeGrammar& G, const string w);

    /*
     * Save the observation table to a LaTeX file
     */
    void saveToLaTeX(const string fileName, const string prefix, const int step);
    string getLaTeXString(const string s);
    string getLaTeXString(const bool b);
    string getLaTeXString(const int i);

private:
    MinimallyAdequateTeacher& _mat;
    Alphabet& _alphabet;
    const string _lambda;
    StringSet _KK; // the Cartesian product between K and itself (K x K)
    Table _table;

    /*
     * Add Context f, increase D and fill in the observation table
     */
    void _addContext(Context f);

    /*
     * Get the set of all contexts of a string k, such that lkr in L
     */
    ContextSet _getDistributionByK(const string k);

    /*
     * Returns the equivalence classes of K under \odot F
     *     if the parameter is false, it will return the equivalence classes of KK
     */
    EquivalenceClasses _getEquivalenceClasses(bool ofK = true);

    /*
     * Returns a pair of strings u', v' in K such that u' in Y, v' in Z and u'v' in X
     */
    pair<string, string> _getStringPair(ContextFreeGrammar& G,
        NonTerminal Y, NonTerminal Z, NonTerminal X);

    /*
     * Build lexical rules and populate the maps in the parameters
     * binaryRulesData (non-terminal: a string that needs to be processed)
     * distributionNonTerminal (relation between a distribution and a non-terminal)
     */
    void _buildLexicalRules(ContextFreeGrammar& G,
        map<NonTerminal, string>& binaryRulesData,
        map<ContextSet, NonTerminal>& distributionNonTerminal);

    /*
     * Build the binary rules from the map binaryRulesData (populated by the _buildLexicalRules)
     */
    void _buildBinaryRules(ContextFreeGrammar& G,
        map<NonTerminal, string>& binaryRulesData);

    /*
     * Build the binary rules from K
     */
    void _buildBinaryRulesFromK(ContextFreeGrammar& G,
        map<ContextSet, NonTerminal>& distributionNonTerminal);
};
