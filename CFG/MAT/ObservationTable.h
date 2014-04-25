/*
 * Observation Table
 * The basic data structure used for learning the CFG
 * We write it as a tuple <K, D, F>, where:
 *    K - a non-empty finite set of strings
 *    F - a non-empty finite set of contexts
 *    D - a set of grammatical strings in F \odot KK (finite function mapping to {false, true})
 */

#include <string>
#include <set>
#include <map>
#include <utility>

#include "MinimallyAdequateTeacher.h"
#include "ContextFreeLanguage.h"
#include "ContextFreeGrammar.h"
#include "Terminal.h"
#include "NonTerminal.h"
#include "ProductionRight.h"
#include "NonTerminalNonTerminal.h"

using namespace std;

#pragma once
class ObservationTable
{
public:
    typedef set<string> StringSet;
    typedef set<string> GrammaticalStringSet;

    typedef pair<string, string> Context;          // pair of strings (l, r)
    typedef set<Context> ContextSet;

    typedef map<string, map<Context, bool>> Table; // map(string k: (Context f, value))

    typedef pair<ContextSet, StringSet> EquivalenceClass;  // pair(distribution, set of strings)
    typedef map<ContextSet, StringSet> EquivalenceClasses; // map(distribution: set of strings)

    StringSet            K;  // non-empty finite set of strings
    ContextSet           F;  // non-empty finite set of contexts
    GrammaticalStringSet D;  // in the finite function mapping F \odot KK to {0, 1}

    StringSet            KK; // the Cartesian product between K and itself (K x K)

    ObservationTable(MinimallyAdequateTeacher& mat);
    ~ObservationTable();

    /*
    * Compute the Cartesian product K x K
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
     * Returns true if they appear in the same set of contexts F
     */
    bool equivalent(const string u, const string v);

    /*
     * Algorithm 1
     * Selects appropriate contexts to make sure that the table is consistent
     */
    void MakeConsistent();

    /*
     * Algorithm 2
     * Constructs a grammar from the observation table in polynomial time
     *     returns a valid Context Free Grammar
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
     * Algorithm 3
     * Returns a context that splits some category X of grammar G
     */
    Context FindContext(const string X, const Context f, const string w);

    /*
     * Algorithm 4
     * Add contexts until the derived grammar no longer generates string w (string that is not in L)
     */
    void AddContexts(const string w);

private:
    MinimallyAdequateTeacher& _mat;
    Alphabet& _alphabet;
    const string _lambda;
    Table _table;

    /*
     * Add Context f, increase D and fill in the observation table
     */
    void _addContext(Context f);

    /*
     * Get the set of all contexts of a string k, such that lkr in L
     */
    ContextSet _getDistributionByK(const string k);

    EquivalenceClasses _getEquivalenceClasses();
};
