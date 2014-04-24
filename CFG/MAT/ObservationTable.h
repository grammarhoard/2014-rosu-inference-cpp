/*
 * Observation Table
 * ...
 */
//TODO write description for ObservationTable

#include <string>
#include <set>
#include <map>
#include <utility>   // std::make_pair

//TODO maybe you can get rid of algorithm (remove set_difference)
#include <algorithm> // std::set_difference

#include "MinimallyAdequateTeacher.h"
#include "ContextFreeLanguage.h"
#include "ContextFreeGrammar.h"

using namespace std;

#pragma once
class ObservationTable
{
public:
    typedef set<string> StringSet;
    typedef pair<string, string> Context;          // pair of strings (l, r)
    typedef set<Context> ContextSet;
    typedef set<string> GrammaticalStringSet;
    typedef map<string, map<Context, bool>> Table; // map(string k: (Context f, value))

    StringSet            K;  // non-empty finite set of strings
    ContextSet           F;  // non-empty finite set of contexts
    GrammaticalStringSet D;  // in the finite function mapping F \odot KK to {0, 1}

    StringSet            KK; // the Cartesian product between K and itself (K x K)

    ObservationTable(MinimallyAdequateTeacher mat);
    ~ObservationTable();

    /*
    * Compute the Cartesian product K x K
    */
    void computeKK();

    /*
     * The insertion or wrapping operation (combines a context f = (l, r) with a substring u)
     */
    void insert(Context f, string u);

    /*
    * Build table and populate D from K and F
    */
    void build();

    /*
    * Returns true if they appear in the same set of contexts F
    */
    bool equivalent(string u, string v);

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
    void addPositiveCounterExample(string w);

    /*
     * Returns the set of all substrings of a string w
     */
    StringSet getSub(string w);

    /*
     * Algorithm 3
     * ...
     */
    //TODO write ObservationTable::FindContext description
    Context FindContext(string X, Context f, string w);

    /*
     * Algorithm 4
     * ...
     */
    //TODO write ObservationTable::AddContexts description
    void AddContexts(ContextFreeGrammar G, string w);

private:
    MinimallyAdequateTeacher _mat;
    Alphabet _alphabet;
    string _lambda;
    Table _table;

    /*
     * Get the set of all contexts of a string k, such that lkr in L
     */
    ContextSet _getDistributionByK(string k);
};
