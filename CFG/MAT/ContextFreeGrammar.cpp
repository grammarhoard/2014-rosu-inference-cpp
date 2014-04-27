#include "ContextFreeGrammar.h"

ContextFreeGrammar::ContextFreeGrammar()
{
}

ContextFreeGrammar::~ContextFreeGrammar()
{
}

NonTerminal ContextFreeGrammar::getStartSymbol()
{
    string numPart = this->_startSymbolIncrement == 0 ? "" : to_string(this->_startSymbolIncrement++);
    return NonTerminal(this->_startSymbol + numPart);
}

NonTerminal ContextFreeGrammar::getNonTerminalSymbol()
{
    if (string(1, this->_currentChar) == this->_startSymbol) {
        this->_currentChar = static_cast<char>(this->_currentChar + 1);
    }

    string numPart = this->_currentSymbolIncrement == 0 ? "" : to_string(this->_currentSymbolIncrement);
    string nonTerminalName = string(1, this->_currentChar) + numPart;

    if (this->_currentChar == 'Z') {
        this->_currentChar = 'A';
        this->_currentSymbolIncrement++;
    }
    this->_currentChar = static_cast<char>(this->_currentChar + 1);

    return NonTerminal(nonTerminalName);
}

bool ContextFreeGrammar::generates(const string w)
{
    for (NonTerminal nonTerminal : this->I) {
        if (this->yields(nonTerminal, w)) {
            return true;
        }
    }
    return false;
}

bool ContextFreeGrammar::yields(const NonTerminal& nonTerminal, const string w)
{
    return this->cykYields(nonTerminal, w);
}

bool ContextFreeGrammar::cykYields(const NonTerminal& nonTerminal, const string w)
{
    size_t i, l, r, t;
    string s; bool exists;
    const size_t length = w.size();
    vector<vector<set<NonTerminal>>> M(length, vector<set<NonTerminal>>(length));

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

    for (l = 1; l < length; l++) { // every substring length
        for (r = 0; r < length - l; r++) { // every starting location for a substring of length l
            for (t = 0; t < l; t++) { // every split of the substring at s[r:r+l]
                s = w.substr(r, r + t + 1);
                set<NonTerminal> & L = M[r][r + t]; // all non-terminals generating s[r:r+t]
                set<NonTerminal> & R = M[r + t + 1][r + l];

                // for all the pairs B in L, C in R do
                for (NonTerminal B : L) {
                    for (NonTerminal C : R) {
                        NonTerminalNonTerminal nTnT(make_pair(B, C));

                        for (Production production : this->P) {
                            if (production.second->equals(nTnT)) {
                                M[r][r + l].insert(production.first);
                            }
                        }
                    }
                }
            }
        }
    }

    set<NonTerminal> & start = M[0][length - 1];
    if (start.find(nonTerminal) != start.end()) { // Found
        return true;
    }

    return false;
}
