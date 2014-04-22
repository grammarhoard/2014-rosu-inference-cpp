#include <set>
#include <map>
#include <string>
#include <utility>   // std::make_pair
#include <vector>
#include <algorithm> // std::next_permutation

#include "ContextFreeLanguage.h"
#include "ObjectiveContextFreeGrammar.h"

using namespace std;

#pragma once
class ObservationTable
{
public:
    typedef set<string> StringSet;
    typedef pair<string, string> Context; // pair of strings (l, r)
    typedef set<Context> ContextSet;
    typedef set<string> GrammaticalStringSet;
    typedef map<string, map<Context, bool>> Table; // map(string k: (Context f, value))

    StringSet            K; // non-empty finite set of strings
    ContextSet           F; // non-empty finite set of contexts
    GrammaticalStringSet D; // in the finite function mapping F \odot KK to {0, 1}

    ObservationTable(ContextFreeLanguage language);
    ~ObservationTable();

    /*
    * Membership Oracle
    * Used to fill in the cell of the table corresponding to context (l, r) and substring u
    */
    bool Mem(string l, string u, string r);

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
     * Returns true if for all u1, u2, v1, v2 \in K,
     * if u1 equivalent with u2 and v1 equivalent with v2 then u1u2 equivalent with v1v2
     */
    bool isConsistent();

    /*
     * Algorithm 1
     * Selects appropriate contexts to make sure that the table is consistent
     *     returns a set of features that is consistent
     */
    ContextSet MakeConsistent();

    /*
     * Algorithm 2
     * Constructs a grammar from the observation table in polynomial time
     *     returns a valid Context Free Grammar
     */
    ObjectiveContextFreeGrammar MakeGrammar();

    //TODO are you sure?
    void addCounterExample(string w, bool positive);

    /*
     * Returns the set of all substrings of a string w
     */
    StringSet getSub(string w);

    /*
     * Returns the Cartesian product K x K
     */
    StringSet getKK();

private:
    ContextFreeLanguage _language;
    Table _table;
};

