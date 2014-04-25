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

#include <string>
#include <list>
#include <set>
#include <vector>
#include <utility>

#include "ContextFreeLanguage.h"
#include "Terminal.h"
#include "NonTerminal.h"
#include "ProductionRight.h"

using namespace std;

#pragma once
class ContextFreeGrammar
{
public:
    //TODO maybe use objects instead of strings for terminals / non-terminals
    typedef pair<NonTerminal, ProductionRight> Production; // pair (left, right)

    set<Terminal>    Sigma; // finite non-empty alphabet of terminal symbols
    set<NonTerminal> V;     // set of non terminals
    set<Production>  P;     // set of productions
    set<NonTerminal> I;     // set of initial symbols

    ContextFreeGrammar();
    ~ContextFreeGrammar();

    /*
     * Returns the start symbol (S)
     * Note: We allow multiple start symbols
     */
    NonTerminal getStartSymbol();

    /*
     * Returns an unique non-terminal symbol
     */
    NonTerminal getNonTerminalSymbol();

    /*
     * Returns true if this grammar can generate the provided string,
     *     and false otherwise
     */
    bool generates(const string w);

    /*
     * Returns true if the non-terminal can lead to generating w,
     *     and false otherwise
     */
    bool yields(NonTerminal nonTerminal, const string w);

    /*
     * CYK Algorithm - decide if a string is in a L(G) defined by this grammar G
     * Returns true if the string is in the language,
     *     and false otherwise
     */
    bool cykYields(NonTerminal nonTerminal, const string w);

private:
    const string _startSymbol      = "S";
    unsigned _startSymbolIncrement = 0;

    char _currentChar                = 'A';
    unsigned _currentSymbolIncrement = 0;
};
