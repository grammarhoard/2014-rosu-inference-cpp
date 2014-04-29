#include "ContextFreeGrammar.h"

ContextFreeGrammar::ContextFreeGrammar()
{
}

ContextFreeGrammar::~ContextFreeGrammar()
{
}

NonTerminal ContextFreeGrammar::getStartSymbol()
{
    string numberPart = this->_startSymbolIncrement == 0 ? "" :
        to_string(this->_startSymbolIncrement++);
    return NonTerminal(this->_startSymbol + numberPart);
}

NonTerminal ContextFreeGrammar::getNonTerminalSymbol()
{
    if (string(1, this->_currentChar) == this->_startSymbol) {
        this->_currentChar = static_cast<char>(this->_currentChar + 1);
    }

    string numberPart = this->_currentSymbolIncrement == 0 ? "" :
        to_string(this->_currentSymbolIncrement);
    string nonTerminalName = string(1, this->_currentChar) + numberPart;

    if (this->_currentChar == 'Z') {
        this->_currentChar = 'A';
        this->_currentSymbolIncrement++;
    }
    this->_currentChar = static_cast<char>(this->_currentChar + 1);

    return NonTerminal(nonTerminalName);
}

bool ContextFreeGrammar::generates(const string w)
{
    Terminal terminal(w);

    // Check terminals
    for (Production production : this->P) {
        if (production.second->equals(terminal)) {
            return true;
        }
    }

    for (NonTerminal nonTerminal : this->I) {
        if (this->cykYields(nonTerminal, w)) {
            return true;
        }
    }
    return false;
}

bool ContextFreeGrammar::cykYields(const NonTerminal& nonTerminal, const string w)
{
    // The CYK algorithm is implemented according to http://pages.cs.wisc.edu/~agorenst/cyk.pdf

    size_t i, l, r, t;
    string s; bool exists;
    const size_t length = w.size();
    vector<vector<set<NonTerminal>>> M(length, vector<set<NonTerminal>>(length));

    // Clear old derivations
    auto oldDerivations = this->_derivations.find(w);
    if (oldDerivations == this->_derivations.end()) { // Not found
        set<Derivation> derivations;
        this->_derivations.insert({ w, derivations });
    } else {
        oldDerivations->second.clear();
    }

    // Populate M with terminals
    for (i = 0; i < length; i++) {
        exists = false;
        s = w.substr(i, 1);

        for (Production production : this->P) {
            if (production.second->equals(Terminal(s))) {
                exists = true;
                M[i][i].insert(production.first);
            }
        }
        if (!exists) { // Character not found
            return false;
        }
    }

    // Populate M with non-terminals
    for (l = 1; l < length; l++) { // every substring length
        for (r = 0; r < length - l; r++) { // every starting location for a substring of length l
            for (t = 0; t < l; t++) { // every split of the substring at s[r:r+l]
                set<NonTerminal> & L = M[r][r + t]; // all non-terminals generating s[r:r+t]
                set<NonTerminal> & R = M[r + t + 1][r + l];

                // for all the pairs B in L, C in R do
                for (NonTerminal B : L) {
                    for (NonTerminal C : R) {
                        NonTerminalNonTerminal nTnT(make_pair(B, C));

                        for (Production production : this->P) {
                            if (production.second->equals(nTnT)) {
                                M[r][r + l].insert(production.first);

                                // Save the derivation
                                if (r == 0 && r + l == length - 1 && production.first.equals(nonTerminal)) {
                                    string sL = w.substr(r, r + t - (r)+1);
                                    string sR = w.substr(r + t + 1, r + l - (r + t + 1) + 1);

                                    this->_derivations.find(w)->second.insert(
                                        make_pair(make_pair(production.first, make_pair(B, C)),
                                        make_pair(sL, sR)));
                                }
                            }
                        }
                    } // end for (NonTerminal C : R)
                } // end for (NonTerminal B : L)
            } // end for (t = 0; ...)
        } // end for (r = 0; ...)
    } // end for (l = 1; ...)

    set<NonTerminal> & start = M[0][length - 1];
    if (start.find(nonTerminal) != start.end()) { // Found
        return true;
    }

    return false;
}

set<ContextFreeGrammar::Derivation> ContextFreeGrammar::getDerivationsByString(const string w)
{
    return this->_derivations.find(w)->second;
}
