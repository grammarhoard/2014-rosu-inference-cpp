#include "ObservationTable.h"

ObservationTable::ObservationTable(MinimallyAdequateTeacher mat) :
_mat(mat), _alphabet(this->_mat.getLanguage().getAlphabet()), _lambda(this->_mat.getLanguage().lambda)
{
}

ObservationTable::~ObservationTable()
{
}

void ObservationTable::computeKK()
{
    this->KK.clear();
    for (string k1 : this->K) {
        for (string k2 : this->K) {
            this->KK.insert(k1 + k2);
        }
    }
}

void ObservationTable::insert(Context f, string u)
{
    this->_table[u][f] = this->_mat.Mem(f.first, u, f.second);

    if (this->_table[u][f]) {
        this->D.insert(f.first + u + f.second);
    }
}

void ObservationTable::build()
{
    //ATTENTION!!! you may want to compute KK before calling this method
    for (string k : this->KK) {
        for (Context f : this->F) {
            this->insert(f, k);
        }
    }
}

bool ObservationTable::equivalent(string u, string v)
{
    for (Context context : this->F) {
        if (this->_table[u][context] != this->_table[v][context]) {
            return false;
        }
    }
    return true;
}

void ObservationTable::MakeConsistent()
{
    for (Context context : this->F) {
        string l = context.first;
        string r = context.second;

        //TODO find a better way to get the four strings (ObservationTable::MakeConsistent)
        for (string u1 : this->K) {
            for (string u2 : this->K) {
                for (string v1 : this->K) {
                    for (string v2 : this->K) {

                        if (this->equivalent(u1, u2) && this->equivalent(v1, v2) &&
                            this->D.find(l + u1 + v1 + r) != this->D.end() && // lu1v1r in D
                            this->D.find(l + u2 + v2 + r) == this->D.end() // lu2v2r not in D
                        ) {

                            // Add context
                            Context f;
                            if (this->_mat.Mem(l, u1 + v2, r)) {
                                f = make_pair(l, v2 + r);
                            } else {
                                f = make_pair(l + u1, r);
                            }
                            this->F.insert(f);

                            // Increase D and fill in the observation table;
                            for (string k : this->KK) {
                                this->insert(f, k);
                            }

                        } // end if
                    }
                }
            }
        } // end for (string u1 ...)
    } // end for (Context ...)
}

ContextFreeGrammar ObservationTable::MakeGrammar()
{
    map<ContextSet, set<string>> equivalenceClasses; // map(distribution: set of strings)

    // Divide K into equivalence classes according to \equiv F
    for (string k : this->K) {

        ContextSet distribution = this->_getDistributionByK(k);
        auto equivalenceClass = equivalenceClasses.find(distribution);

        if (equivalenceClass != equivalenceClasses.end()) { // Found
            equivalenceClass->second.insert(k);
        } else { // Not found
            StringSet ks; // set of k strings
            ks.insert(k);
            equivalenceClasses.insert({ distribution, ks });
        }
    }


    map<ContextSet, string> distributionNonTerminal; // map(distribution: non terminal)
    map<string, string> lexicalRulesData; // map(string: non-terminal(s))
    map<string, string> binaryRulesData; // map(non-terminal: string w)
    ContextFreeGrammar G;

    // Let V be the set of these equivalence classes - set of non terminals
    for (pair<ContextSet, set<string>> equivalenceClass : equivalenceClasses) {
        string nonTerminal;

        // Check if this equivalence class can generate a start symbol
        bool isStart = true;
        for (string w : equivalenceClass.second) {
            if (this->D.find(w) == this->D.end()) { // Not found
                isStart = false;
            }
        }

        // Create the non-terminal for this equivalence class
        if (isStart) {
            nonTerminal = G.getStartSymbol();
            G.I.insert(nonTerminal);
        } else {
            nonTerminal = G.getNonTerminalSymbol();
        }
        G.V.insert(nonTerminal);

        // Compute lexical rules
        for (string w : equivalenceClass.second) {

            // Remember distribution - non terminal mapping
            distributionNonTerminal.insert({ equivalenceClass.first, nonTerminal });

            if (w == this->_lambda || this->_alphabet.in(w)) {
                G.P.insert({ nonTerminal, w }); // N -> a
                G.Sigma.insert(w);
                lexicalRulesData.insert({ w, nonTerminal });
                continue;
            }

            // Remember the strings that will become binary rules
            binaryRulesData.insert({ nonTerminal, w });
        }
    }

    // Compute binary rules
    if (binaryRulesData.size() > 0) {
        for (pair<string, string> binaryRuleData : binaryRulesData) {
            string rightSide;
            for (char c : binaryRuleData.second) {
                string s(1, c);
                //TODO throw error message if no lexical rule is found having this string
                rightSide += lexicalRulesData[s];
            }
            G.P.insert({ binaryRuleData.first, rightSide });
            lexicalRulesData.insert({ binaryRuleData.second, binaryRuleData.first });
        }
    }

    // Compute binary rules from K K \ K
    for (string k1 : this->K) {
        for (string k2 : this->K) {
            string kk = k1 + k2;

            if (this->K.find(kk) != this->K.end()) { // Found in K
                continue;
            }

            // Get distribution of this string
            ContextSet distribution = this->_getDistributionByK(kk);

            // Get the non-terminal based on this distribution
            auto nonTerminalPair = distributionNonTerminal.find(distribution);
            if (nonTerminalPair == distributionNonTerminal.end()) { // Not found
                continue;
            }

            string nonTerminal = nonTerminalPair->second;

            //TODO throw error message if no lexical rule is found having this string
            G.P.insert({ nonTerminal, lexicalRulesData[k1] + lexicalRulesData[k2] });
        }
    }

    return G;
}

void ObservationTable::addPositiveCounterExample(string w)
{
    // Add Sub(w) to K
    StringSet substrings = this->getSub(w);
    this->K.insert(substrings.begin(), substrings.end());
}

ObservationTable::StringSet ObservationTable::getSub(string w)
{
    int c, i;
    int size = w.size();
    StringSet stringSet;

    for (c = 0; c < size; c++) {
        for (i = 1; i <= size - c; i++) {
            stringSet.insert(w.substr(c, c + i));
        }
    }

    return stringSet;
}

ObservationTable::Context ObservationTable::FindContext(string X, Context f, string w)
{
    //TODO implement ObservationTable::FindContext
    return make_pair("", "b");
}

void ObservationTable::AddContexts(ContextFreeGrammar G, string w)
{
    //TODO implement ObservationTable::AddContexts
    Context f = this->FindContext("S", make_pair("", ""), w);
    this->F.insert(f);

    // Increase D and fill in the observation table;
    for (string k : this->KK) {
        this->insert(f, k);
    }
}

ObservationTable::ContextSet ObservationTable::_getDistributionByK(string k)
{
    ContextSet distribution;
    for (Context context : this->F) {
        if (this->_mat.Mem(context.first, k, context.second)) {
            distribution.insert(context);
        }
    }
    return distribution;
}
