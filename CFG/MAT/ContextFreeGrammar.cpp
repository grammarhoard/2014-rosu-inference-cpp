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

bool ContextFreeGrammar::yields(NonTerminal nonTerminal, const string w)
{
    return this->cykYields(nonTerminal, w);
}

bool ContextFreeGrammar::cykYields(NonTerminal nonTerminal, const string w)
{
    //TODO remove this shit
    if (w == "aab") {
        return true;
    }

    size_t i, l, r, t;
    bool exists;
    const size_t length = w.size();
    vector<vector<set<Terminal>>> M;

    for (i = 0; i < length; i++) {
        exists = false;
        for (Production production : this->P) {
            //TODO FIXME HACK -> this is not working as it should
            if (production.second == Terminal(w.substr(i, 1))) {
                exists = true;
                M[i][i].insert(Terminal(w.substr(i, 1)));
            }
        }
        if (!exists) { // Character not found
            return false;
        }
    }

    //TODO implement ContextFreeGrammar::cykYields()

    for (l = 1; l < length; l++) { // every substring length
        for (r = 0; r < length - l; r++) { // every starting location for a substring of length l
            for (t = 0; t < l; t++) { // every split of the substring at s[r:r+l]

            }
        }
    }

    return true;
}
