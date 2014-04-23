#include "ObservationTable.h"

ObservationTable::ObservationTable(ContextFreeLanguage language) : _language(language)
{
    this->K.insert(this->_language.lambda);

    //TODO not sure yet
    // set<string> alphabet = this->_language.getAlphabet().get();
    // this->K.insert(alphabet.begin(), alphabet.end());

    this->F.insert({ this->_language.lambda, this->_language.lambda });
}

ObservationTable::~ObservationTable()
{
}

bool ObservationTable::Mem(string l, string u, string r)
{
    return this->_language.in(l + u + r);
}

void ObservationTable::insert(Context f, string u)
{
    this->_table[u][f] = this->Mem(f.first, u, f.second);

    if (this->_table[u][f]) {
        this->D.insert(f.first + u + f.second);
    }
}

void ObservationTable::build()
{
    //TODO maybe also keep KK from the start
    StringSet KK;
    KK.insert(this->K.begin(), this->K.end());
    KK.insert(this->KKmK.begin(), this->KKmK.end());

    for (string k : KK) {
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

bool ObservationTable::isConsistent()
{
    // I need at least 4 elements
    int size = this->K.size();
    if (size < 4) {
        return true;
    }

    vector<string> vectorK(this->K.begin(), this->K.end());
    vectorK.erase(vectorK.begin());
    do {
        string u1 = vectorK[0];
        string u2 = vectorK[1];
        string v1 = vectorK[2];
        string v2 = vectorK[3];
        cout << u1 << "-" << u2 << "-" << v1 << "-" << v2 << endl;

        if (this->equivalent(u1, u2) && this->equivalent(v1, v2) && !this->equivalent(u1 + u2, v1 + v2)) {
            return false;
        }
    } while (next_permutation(vectorK.begin(), vectorK.begin() + 4));

    return true;
}

ObservationTable::ContextSet ObservationTable::MakeConsistent()
{
    for (Context context : this->F) {
        string l = context.first;
        string r = context.second;

        //TODO find a better way to implement this (maybe more efficient
        for (string u1 : this->K) {
            for (string u2 : this->K) {
                for (string v1 : this->K) {
                    for (string v2 : this->K) {

                        if (this->equivalent(u1, u2) && this->equivalent(v1, v2) &&
                            this->D.find(l + u1 + v1 + r) != this->D.end() && // lu1v1r in D
                            this->D.find(l + u2 + v2 + r) == this->D.end() // lu2v2r not in D
                        ) {
                            Context f;
                            if (this->Mem(l, u1 + v2, r)) {
                                f = make_pair(l, v2 + r);
                            } else {
                                f = make_pair(l + u1, r);
                            }
                            this->F.insert(f);

                            // Use Mem() to increase D to fill in the observation table;
                            for (string k : this->K) {
                                this->insert(f, k);
                            }

                            for (string kkmk : this->KKmK) {
                                this->insert(f, kkmk);
                            }
                        } // end if
                    }
                }
            }
        }
    } // end for (Context ...)

    //TODO maybe we don't need it but the Algorithm 1 from the paper returns it
    return this->F;
}

ObjectiveContextFreeGrammar ObservationTable::MakeGrammar()
{
    ObjectiveContextFreeGrammar G;
    map<ContextSet, set<string>> equivalenceClasses; // map(distribution: set of strings)
    map<string, string> lexicalRulesData; // map(terminal: non-terminal)
    map<string, string> binaryRulesData; // map(non-terminal: string w)
    char currentChar = 'A';

    // Divide K into equivalence classes according to \equiv F
    for (string k : this->K) {
        ContextSet distribution;
        for (Context context : this->F) {
            string l = context.first;
            string r = context.second;
            if (this->Mem(l, k, r)) {
                distribution.insert(context);
            }
        }
        if (equivalenceClasses.find(distribution) != equivalenceClasses.end()) { // Found
            equivalenceClasses[distribution].insert(k);
        } else { // Not found
            set<string> ks;
            ks.insert(k);
            equivalenceClasses.insert({ distribution, ks });
        }
    }

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
            nonTerminal = "S";
            G.I.insert(nonTerminal);
        } else {
            //TODO make it infinite (now it's useful only till Z)
            //TODO avoid S
            nonTerminal = string(1, currentChar);
            currentChar = static_cast<char>(currentChar + 1);
        }
        G.V.insert(nonTerminal);

        // Compute lexical rules
        for (string w : equivalenceClass.second) {
            if (w == this->_language.lambda) {
                G.P.insert({ nonTerminal, w }); // N -> lambda
                continue;
            }

            // Terminal
            if (this->_language.getAlphabet().in(w)) {
                G.P.insert({nonTerminal, w}); // N -> a
                lexicalRulesData.insert({ w, nonTerminal });
                continue;
            }

            // Remember binary rules
            binaryRulesData.insert({ nonTerminal, w });
        }
    }

    // Compute binary rules
    if (binaryRulesData.size() > 0) {
        int substringLength = 1;
        for (pair<string, string> binaryRuleData : binaryRulesData) {
            string rightSide;
            for (char c : binaryRuleData.second) {
                string s(1, c);
                //TODO throw error message if it's not found
                rightSide += lexicalRulesData[s];
            }
            G.P.insert({ binaryRuleData.first, rightSide });
        }
    }

    return G;
}

void ObservationTable::addCounterExample(string w, bool positive)
{
    if (positive) { // positive
        // Add Sub(w) to K
        StringSet substrings = this->getSub(w);
        this->K.insert(substrings.begin(), substrings.end());

        // Compute K x K / K
        StringSet KK = this->getKK();
        set_difference(KK.begin(), KK.end(), this->K.begin(), this->K.end(), inserter(this->KKmK, this->KKmK.end()));
    }

    //TODO what to do on negative????
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

ObservationTable::StringSet ObservationTable::getKK()
{
    StringSet KK;
    for (string k1 : this->K) {
        for (string k2 : this->K) {
            KK.insert(k1 + k2);
        }
    }
    return KK;
}