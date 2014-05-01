#include "ContextFreeGrammar.h"

ContextFreeGrammar::ContextFreeGrammar()
{
}

ContextFreeGrammar::~ContextFreeGrammar()
{
}

NonTerminal ContextFreeGrammar::getStartSymbol()
{
    // Compute the number part
    string numberPart = "";
    if (this->_startSymbolIncrement != 0) {
        numberPart = to_string(this->_startSymbolIncrement);
    }
    this->_startSymbolIncrement++;

    return NonTerminal(this->_startSymbol + numberPart);
}

NonTerminal ContextFreeGrammar::getNonTerminalSymbol()
{
    string currentChar = string(1, this->_currentChar);

    // If the current char is the char used as the start symbol, increment it
    if (currentChar == this->_startSymbol) {
        this->_currentChar = static_cast<char>(this->_currentChar + 1);
        currentChar = string(1, this->_currentChar);
    }

    // Compute the number part
    string numberPart = "";
    if (this->_currentSymbolIncrement != 0) {
        numberPart = to_string(this->_currentSymbolIncrement);
    }

    // If the current char is Z, reset it to A and increment the number part
    if (this->_currentChar == 'Z') {
        this->_currentChar = 'A';
        this->_currentSymbolIncrement++;
    }
    this->_currentChar = static_cast<char>(this->_currentChar + 1);

    return NonTerminal(currentChar + numberPart);
}

bool ContextFreeGrammar::generates(const string w)
{
    Terminal terminal(w);

    // Check lexical rules (terminals)
    for (Production production : this->P) {
        if (production.right.equals(terminal)) {
            return true;
        }
    }

    // Check binary rules (non-terminals)
    for (NonTerminal nonTerminal : this->I) {
        if (this->cykYields(nonTerminal, w)) {
            return true;
        }
    }

    return false;
}

bool ContextFreeGrammar::cykYields(const NonTerminal& nonTerminal, const string w)
{
    this->_clearOldDerivations(w);

    // The CYK algorithm is implemented according to http://pages.cs.wisc.edu/~agorenst/cyk.pdf
    size_t i, l, r, t;
    string s; bool exists;
    const size_t length = w.size();
    vector<vector<set<NonTerminal>>> M(length, vector<set<NonTerminal>>(length));

    // Populate M with terminals
    for (i = 0; i < length; i++) {
        exists = false;
        s = w.substr(i, 1);

        for (Production production : this->P) {
            if (production.right.equals(Terminal(s))) {
                exists = true;
                M[i][i].insert(production.left);
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
                this->_cykPopulateMwithNonTerminals(nonTerminal, w, M, l, r, t);
            }
        }
    }

    // Final check
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

NonTerminalNonTerminal* ContextFreeGrammar::getNonTerminalPairForString(const string s)
{
    set<pair<string, string>> alternatives = this->_getSplitAlternatives(s);
    string s1, s2;

    for (pair<string, string> alternative : alternatives) {
        s1 = alternative.first;
        s2 = alternative.second;

        map<string, NonTerminal>::iterator elementL1 = this->_lexicalRules.find(s1);
        map<string, NonTerminal>::iterator elementL2 = this->_lexicalRules.find(s2);

        if (elementL1 != this->_lexicalRules.end() && elementL2 != this->_lexicalRules.end()) {
            return new NonTerminalNonTerminal(make_pair(elementL1->second, elementL2->second));
        }
    }

    // If no viable split was found, take the first one and create what you need
    auto alternative = alternatives.begin();
    s1 = alternative->first;
    s2 = alternative->second;

    NonTerminal nt1 = this->_getNonTerminalByString(s1);
    NonTerminal nt2 = this->_getNonTerminalByString(s2);

    return new NonTerminalNonTerminal(make_pair(nt1, nt2));
}

ContextFreeGrammar::EquivalenceClasses& ContextFreeGrammar::getEquivalenceClasses()
{
    return this->_equivalenceClasses;
}

ContextFreeGrammar::LexicalRules& ContextFreeGrammar::getLexicalRules()
{
    return this->_lexicalRules;
}

void ContextFreeGrammar::saveToLaTeX(const string fileName, const string prefix, const int step)
{
    string fullFileName = fileName + prefix + "_CFG_Step" + to_string(step) + ".tex";
    ofstream myfile(fullFileName); bool hasKK = false;
    if (!myfile.is_open()) {
        string message = "File '" + fullFileName + "' could not be opened";
        throw exception(message.c_str());
    }

    size_t i;
    size_t SigmaSize = this->Sigma.size();
    size_t VSize = this->V.size();
    size_t ISize = this->I.size();
    size_t PSize = this->P.size();

    // Sigma (terminals)
    myfile << "\\textbf{$\\Sigma$} = $\\{";
    i = 0;
    for (Terminal terminal: this->Sigma) {
        myfile << this->getLaTeXString(terminal.getName());
        if (i != SigmaSize - 1) {
            myfile << ", ";
        }
        i++;
    }
    myfile << "\\}$" << endl << endl;

    // V (non-terminals)
    myfile << "\\textbf{$V$} = $\\{";
    i = 0;
    for (NonTerminal nonTerminal: this->V) {
        myfile << this->getLaTeXString(nonTerminal.getName());
        if (i != VSize- 1) {
            myfile << ", ";
        }
        i++;
    }
    myfile << "\\}$" << endl << endl;

    // I (start symbols)
    myfile << "\\textbf{$I$} = $\\{";
    i = 0;
    for (NonTerminal nonTerminal : this->I) {
        myfile << this->getLaTeXString(nonTerminal.getName());
        if (i != ISize - 1) {
            myfile << ", ";
        }
        i++;
    }
    myfile << "\\}$" << endl << endl;

    // P (productions)
    myfile << "\\textbf{$P$} = $\\{$";
    i = 0;
    for (Production production: this->P) {
        myfile << "$" << this->getLaTeXString(production.left.getName())
            << "\\to\\ " << this->getLaTeXString(production.right.getName()) << "$";
        if (i != PSize - 1) {
            myfile << ", ";
        }
        i++;
    }
    myfile << "$\\}$" << endl << endl;

    myfile << endl
        << "\\caption{Step " << step << "}" << endl
        << "\\label{figure:CFG" << prefix << "_step" << step << "}" << endl;

    myfile.close();

}

string ContextFreeGrammar::getLaTeXString(const string s)
{
    return s == "" ? "\\lambda" : s;
}

void ContextFreeGrammar::_clearOldDerivations(const string w)
{
    auto oldDerivations = this->_derivations.find(w);
    if (oldDerivations == this->_derivations.end()) { // Not found
        set<Derivation> derivations;
        this->_derivations.insert({ w, derivations });
    } else {
        oldDerivations->second.clear();
    }
}

void ContextFreeGrammar::_cykPopulateMwithNonTerminals( const NonTerminal& nonTerminal, const string w,
    vector<vector<set<NonTerminal>>>& M, const size_t l, const size_t r, const size_t t)
{
    const size_t length = w.size();
    set<NonTerminal>& L = M[r][r + t]; // all non-terminals generating s[r:r+t]
    set<NonTerminal>& R = M[r + t + 1][r + l];

    // for all the pairs B in L, C in R do
    for (NonTerminal B : L) {
        for (NonTerminal C : R) {
            NonTerminalNonTerminal nTnT(make_pair(B, C));

            for (Production production : this->P) {
                if (!production.right.equals(nTnT)) {
                    continue;
                }

                M[r][r + l].insert(production.left);

                // Save the derivation
                if (r == 0 && r + l == length - 1 && production.left.equals(nonTerminal)) {
                    string sL = w.substr(r, r + t - (r)+1);
                    string sR = w.substr(r + t + 1, r + l - (r + t + 1) + 1);

                    this->_derivations.find(w)->second.insert(
                        make_pair(make_pair(production.left, make_pair(B, C)),
                        make_pair(sL, sR))
                    );
                }
            }
        } // end for (NonTerminal C : R)
    } // end for (NonTerminal B : L)
}

set<pair<string, string>> ContextFreeGrammar::_getSplitAlternatives(const string s)
{
    set<pair<string, string>> alternatives;
    size_t length = s.length();
    size_t len = 1; // first substring length
    string l, r;

    if (length < 2) {
        string message = "String '" + s + "' has las than 2 characters";
        throw exception(message.c_str());
    }

    while (len < length) {
        l = s.substr(0, len);
        r = s.substr(len);

        alternatives.insert(make_pair(l, r));
        len++;
    }

    return alternatives;
}

NonTerminal ContextFreeGrammar::_getNonTerminalByString(const string s)
{
    map<string, NonTerminal>::iterator element = this->_lexicalRules.find(s);
    NonTerminal nonTerminal = element != this->_lexicalRules.end() ? element->second :
        this->getNonTerminalSymbol();

    if (this->V.find(nonTerminal) == this->V.end()) { // Not found
        this->Sigma.insert(Terminal(s));
        this->P.insert(Production(nonTerminal, new Terminal(s)));
        this->_lexicalRules.insert({ s, nonTerminal });
    }

    return nonTerminal;
}
