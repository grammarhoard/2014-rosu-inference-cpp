#include "ContextFreeGrammar.h"

ContextFreeGrammar::ContextFreeGrammar()
{
}

ContextFreeGrammar::~ContextFreeGrammar()
{
}

string ContextFreeGrammar::getStartSymbol()
{
    return this->_startSymbol;
}

string ContextFreeGrammar::getNonTerminalSymbol()
{
    //TODO find a way to get infinite unique non-terminal symbols (now only 25 letters are available)
    string nonTerminal = string(1, this->_currentChar);
    this->_currentChar = static_cast<char>(this->_currentChar + 1);
    if (nonTerminal != this->_startSymbol) {
        return nonTerminal;
    }
    return this->getNonTerminalSymbol();
}
