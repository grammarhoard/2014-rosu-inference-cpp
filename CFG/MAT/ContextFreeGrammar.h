/*
 * Objective Context Free Grammar (CFG)
 * A CFG is a tuple <Sigma, V, P, I> where:
 *     Sigma - set of terminal symbols
 *     V - finite non-empty set of non-terminals
 *     I - non-empty subset of V, the set of initial symbols
 *     P - finite set of productions of the form V x (V U Sigma)* in Chomsky Normal Form (CNF):
 *         N -> PQ or N -> a or N -> lambda
 * Note: it is objective because is not the standard version
 *     - now we allow multiple start symbols and we allow the alphabet Sigma to be empty
 */
#pragma once

#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include "Terminal.h"
#include "NonTerminal.h"
#include "NonTerminalNonTerminal.h"
#include "ProductionRight.h"

using namespace std;

class ContextFreeGrammar
{
public:
    typedef pair<NonTerminal, ProductionRight*> Production; // pair(left, right)

    typedef pair<pair<NonTerminal, NonTerminalNonTerminal>,
        pair<string, string>> Derivation; // pair(production, pair(left substring, right substring))
    typedef map<string, set<Derivation>> Derivations;

    typedef pair<NonTerminal, set<string>> EquivalenceClass;
    typedef map<NonTerminal, set<string>> EquivalenceClasses;

    set<Terminal>    Sigma; // finite non-empty alphabet of terminal symbols
    set<NonTerminal> V;     // set of non terminals
    set<Production>  P;     // set of productions
    set<NonTerminal> I;     // set of initial symbols
    EquivalenceClasses equivalenceClasses; // map(non-terminal, equivalence class)

    ContextFreeGrammar();
    ~ContextFreeGrammar();

    /*
     * Returns an unique start symbol (S, S1, S2, ...)
     * Note: We allow multiple start symbols
     */
    NonTerminal getStartSymbol();

    /*
     * Returns an unique non-terminal symbol (A, B, ..., Z, A1, B1, ...)
     */
    NonTerminal getNonTerminalSymbol();

    /*
     * Returns true if this grammar can generate the provided string,
     *     and false otherwise
     */
    bool generates(const string w);

    /*
     * CYK Algorithm - decide if a string is in a L(G) defined by this grammar G
     * Returns true if the string is in the language L(G),
     *     and false otherwise
     * If the return value is true, the derivations are available
     *     by calling getDerivationsByString(w)
     */
    bool cykYields(const NonTerminal& nonTerminal, const string w);

    /*
     * Get the derivations found by last cykYields(X, w) run
     */
    set<Derivation> getDerivationsByString(const string w);

private:
    const string _startSymbol        = "S";
    unsigned _startSymbolIncrement   = 0;

    char _currentChar                = 'A';
    unsigned _currentSymbolIncrement = 0;

    Derivations _derivations;
};
