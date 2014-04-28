#include "ObservationTable.h"

ObservationTable::ObservationTable(MinimallyAdequateTeacher& mat) :
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

void ObservationTable::insert(const Context f, const string u)
{
    this->_table[u][f] = this->_mat.Mem(f.first, u, f.second);

    if (this->_table[u][f]) {
        this->D.insert(f.first + u + f.second);
    }
}

void ObservationTable::build()
{
    this->computeKK();
    for (string k : this->KK) {
        for (Context f : this->F) {
            this->insert(f, k);
        }
    }
}

bool ObservationTable::equivalent(const string u, const string v)
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

        //TODO maybe find a better way to get the four strings (ObservationTable::MakeConsistent)
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
                            this->_addContext(f);

                        } // end if
                    }
                }
            }
        } // end for (string u1 ...)
    } // end for (Context ...)
}

ContextFreeGrammar ObservationTable::MakeGrammar()
{
    this->build();
    this->MakeConsistent();

    EquivalenceClasses equivalenceClasses = this->_getEquivalenceClasses();

    //TODO change this->_getCategories() to this->_getFullEquivalenceClasses()
    EquivalenceClasses fullEquivalenceClasses = this->_getCategories();
    map<ContextSet, NonTerminal> distributionNonTerminal; // map(distribution: non-terminal)

    ContextFreeGrammar G;

    map<string, NonTerminal> lexicalRulesData; // map(string: non-terminal)
    map<NonTerminal, string> binaryRulesData;  // map(non-terminal: string w)

    // Let V be the set of these equivalence classes - set of non terminals
    for (EquivalenceClass equivalenceClass : equivalenceClasses) {

        // Check if this equivalence class can generate a start symbol
        bool isStart = true;
        for (string w : equivalenceClass.second) {
            if (this->D.find(w) == this->D.end()) { // Not found
                isStart = false;
            }
        }

        // Create the non-terminal for this equivalence class
        NonTerminal nonTerminal = isStart ? G.getStartSymbol() : G.getNonTerminalSymbol();
        if (isStart) {
            G.I.insert(nonTerminal);
        }
        G.V.insert(nonTerminal);
        // G.equivalenceClasses.insert({nonTerminal, equivalenceClass.second});
        //TODO find a better way to combine equivalenceClasses with fullEquivalenceClasses
        G.equivalenceClasses.insert({ nonTerminal, fullEquivalenceClasses.find(equivalenceClass.first)->second });
        //TODO tie V.insert and equivalenceClasses.insert together

        // Compute lexical rules
        for (string w : equivalenceClass.second) {

            distributionNonTerminal.insert({ equivalenceClass.first, nonTerminal });

            if (w == this->_lambda || this->_alphabet.in(w)) { // terminal or lambda
                G.P.insert({ nonTerminal, new Terminal(w) }); // N -> a
                G.Sigma.insert(Terminal(w));
                lexicalRulesData.insert({ w, nonTerminal });
                continue;
            }

            // Remember the strings that will become binary rules
            binaryRulesData.insert({ nonTerminal, w });
        }
    }

    // Compute binary rules
    if (binaryRulesData.size() > 0) {
        for (pair<NonTerminal, string> binaryRuleData : binaryRulesData) {
            string s = binaryRuleData.second;
            string s1 = s.substr(0, 1);
            string s2 = s.substr(1, 1);

            map<string, NonTerminal>::iterator elementL1 = lexicalRulesData.find(s1);
            map<string, NonTerminal>::iterator elementL2 = lexicalRulesData.find(s2);
            if (elementL1 == lexicalRulesData.end() || elementL2 == lexicalRulesData.end()) {
                string message = "Lexical rule data not found for strings " + s1 + " and/or " + s2 + "!";
                throw exception(message.c_str());
                continue;
            }
            NonTerminalNonTerminal* nTnT = new NonTerminalNonTerminal(make_pair(elementL1->second, elementL2->second));
            G.P.insert(make_pair(binaryRuleData.first, nTnT));
            // delete nTnT;

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

            NonTerminal nonTerminal = nonTerminalPair->second;

            map<string, NonTerminal>::iterator elementK1 = lexicalRulesData.find(k1);
            map<string, NonTerminal>::iterator elementK2 = lexicalRulesData.find(k2);
            if (elementK1 == lexicalRulesData.end() || elementK2 == lexicalRulesData.end()) {
                string message = "Lexical rule data not found for strings " + k1 + " and/or " + k2 + "!";
                throw exception(message.c_str());
                continue;
            }

            NonTerminalNonTerminal* nTnT = new NonTerminalNonTerminal(make_pair(elementK1->second, elementK2->second));
            G.P.insert({ nonTerminal, nTnT });
            // delete nTnT;
        }
    }

    return G;
}

void ObservationTable::addPositiveCounterExample(const string w)
{
    // Add Sub(w) to K
    StringSet substrings = this->getSub(w);
    this->K.insert(substrings.begin(), substrings.end());
}

ObservationTable::StringSet ObservationTable::getSub(const string w)
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

bool ObservationTable::contextSplitsCategory(const Context f, const NonTerminal X)
{
    //TODO avoid making the grammar in every method
    ContextFreeGrammar G = this->MakeGrammar();
    StringSet equivalenceClasses = G.equivalenceClasses.find(X)->second;

    string l = f.first;
    string r = f.second;

    for (string u : equivalenceClasses) {
        for (string v : equivalenceClasses) {
            if (u == v) {
                continue;
            }

            if (this->_mat.Mem(l, u, r) && !this->_mat.Mem(l, v, r)) {
                // This context splits this category
                return true;
            }
        }
    }

    return false;
}

ObservationTable::Context ObservationTable::FindContext(const NonTerminal X, const Context f, const string w)
{
    // Used by ObservationTable::AddContexts

    // if f splits X then return f
    if (this->contextSplitsCategory(f, X)) {
        return f;
    }

    // w is not congruent with the strings in X

    //TODO avoid making the grammar in every method
    ContextFreeGrammar G = this->MakeGrammar();
    EquivalenceClasses categories = this->_getCategories();
    string l = f.first;
    string r = f.second;
    if (G.cykYields(X, w)) {
        set<ContextFreeGrammar::Derivation> derivations = G.getDerivations(w);

        // Let X -> YZ yields uv = w be a derivation of w such that Y yields u and Z yields v
        for (ContextFreeGrammar::Derivation derivation: derivations) {
            string u = derivation.second.first;
            string v = derivation.second.second;
            NonTerminal Y = derivation.first.second.getPair().first;
            NonTerminal Z = derivation.first.second.getPair().second;

            // Find a pair of strings u', v' in K such that u' in Y, v' in Z and u' v' in X
            pair<string, string> stringPair = this->_getStringPair(Y, Z, X);
            string u1 = stringPair.first;
            string v1 = stringPair.second;

            if (this->_mat.Mem(f.first, u1 + v, f.second)) {
                return this->FindContext(Y, make_pair(l, v + r), u);
            } else {
                return this->FindContext(Z, make_pair(l + u1, r), v);
            }
        }
    }

    throw exception("No context found!");
}

void ObservationTable::AddContexts(const string w)
{
    // The string w is not in L
    ContextFreeGrammar G = this->MakeGrammar();
    bool generates = G.generates(w);

    //TODO maybe: the algorithm is with while do and we have do while, check again if you need it that way
    do {
        // Suppose S yields w for some S in I
        for (NonTerminal nonTerminal : G.I) {
            if (!G.yields(nonTerminal, w)) {
                continue;
            }

            Context f = this->FindContext(nonTerminal, make_pair(this->_lambda, this->_lambda), w);
            this->_addContext(f);
            ContextFreeGrammar G = this->MakeGrammar();
            generates = G.generates(w);
            break;
        }

    } while (generates);
}

void ObservationTable::_addContext(Context f)
{
    this->F.insert(f);

    // Increase D and fill in the observation table
    for (string k : this->KK) {
        this->insert(f, k);
    }
}

ObservationTable::ContextSet ObservationTable::_getDistributionByK(const string k)
{
    ContextSet distribution;
    for (Context context : this->F) {
        if (this->_mat.Mem(context.first, k, context.second)) {
            distribution.insert(context);
        }
    }
    return distribution;
}

ObservationTable::EquivalenceClasses ObservationTable::_getEquivalenceClasses()
{
    EquivalenceClasses equivalenceClasses;

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
    return equivalenceClasses;
}

ObservationTable::EquivalenceClasses ObservationTable::_getCategories()
{
    EquivalenceClasses equivalenceClasses;

    // Divide KK into equivalence classes according to \equiv F
    for (string kk : this->KK) {
        ContextSet distribution = this->_getDistributionByK(kk);
        auto equivalenceClass = equivalenceClasses.find(distribution);

        if (equivalenceClass != equivalenceClasses.end()) { // Found
            equivalenceClass->second.insert(kk);
        } else { // Not found
            StringSet kks; // set of k strings
            kks.insert(kk);
            equivalenceClasses.insert({ distribution, kks });
        }
    }

    //TODO remove equivalence classes that do not have at least one element from K
    return equivalenceClasses;
}

pair<string, string> ObservationTable::_getStringPair(NonTerminal Y, NonTerminal Z, NonTerminal X)
{
    //TODO avoid making the grammar in every method
    ContextFreeGrammar G = this->MakeGrammar();
    // Find a pair of strings u', v' in K such that u' in Y, v' in Z and u' v' in X
    //TODO find a better way to compute the string pair
    for (string k1 : this->K) {
        for (string k2 : this->K) {
            //HACK find out if it is a good thing to ignore lambda strings
            if (k1 == this->_lambda || k2 == this->_lambda) {
                continue;
            }
            if (G.yields(Y, k1) && G.yields(Z, k2) && G.yields(X, k1 + k2)) {
                return make_pair(k1, k2);
            }
        }
    }

    throw exception("No suitable pairs of strings found for this combination");
}
