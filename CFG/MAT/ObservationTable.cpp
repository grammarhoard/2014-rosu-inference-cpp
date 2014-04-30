#include "ObservationTable.h"

ObservationTable::ObservationTable(MinimallyAdequateTeacher& mat) :
    _mat(mat), _alphabet(this->_mat.getLanguage().getAlphabet()),
    _lambda(this->_mat.getLanguage().lambda)
{
}

ObservationTable::~ObservationTable()
{
}

ContextFreeGrammar ObservationTable::LearnCFG()
{
    while (true) {
        ContextFreeGrammar G = this->MakeGrammar();

        if (this->Equiv(G)) {
            return G;
        }
        string w = this->_counterExample;

        // Query the current language L(G)
        if (this->_counterExampleIsUndergeneration) { // Not found -> w is not in L(G)
            this->addPositiveCounterExample(w);
        } else {
            this->AddContexts(G, w);
        }
    }
}

bool ObservationTable::Equiv(ContextFreeGrammar& G)
{
    this->_counterExample = "";
    this->_counterExampleIsUndergeneration = NULL;

    // The learner provides the teacher with a hypothesis, and
    //     the teacher will either confirm that it is correct, or
    //     it will provide the learner with a counter - example

    // Check if all of the strings in L can be generated with the grammar G (prevent undergeneration)
    for (string s : this->_mat.getLanguage().getData()) {
        if (!G.generates(s)) {
            this->_counterExample = s;
            this->_counterExampleIsUndergeneration = true;
            return false;
        }
    }

    // Check if grammar G does not generate more strings than L has (prevent overgeneration)

    // If the partition of KK into classes is correct, then we will not overgeneralise
    // If we overgeneralise, then there must be two strings w1, w2 in KK that appear to be congruent but are not
    //     so we need to add a feature/context to have a more fine division into classes
    //     so that the two string w1 and w2 are in different classes

    for (ContextFreeGrammar::EquivalenceClass equivalenceClass : G.equivalenceClasses) {
        for (string u : equivalenceClass.second) {
            for (string v : equivalenceClass.second) {
                if (u == v) {
                    continue;
                }

                // We already know that the strings are equivalent

                // Try to find a context (l, r) with l, r in Sigma such that lur in L and lvr not in L
                for (string l : this->_alphabet.get()) {
                    for (string r : this->_alphabet.get()) {
                        if (this->_mat.Mem(l, u, r) && !this->_mat.Mem(l, v, r)) {
                            this->_counterExample = l + v + r;
                            this->_counterExampleIsUndergeneration = false;
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
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

        //TODO maybe: Find a better way to get the four strings (ObservationTable::MakeConsistent)
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

    EquivalenceClasses equivalenceClassesK = this->_getEquivalenceClassesK();
    EquivalenceClasses equivalenceClassesKK = this->_getEquivalenceClassesKK();
    map<ContextSet, NonTerminal> distributionNonTerminal; // map(distribution: non-terminal)
    map<string, NonTerminal> lexicalRulesData; // map(string: non-terminal)
    map<NonTerminal, string> binaryRulesData;  // map(non-terminal: string w)

    // Create a new CFG G
    ContextFreeGrammar G;

    // Let V be the set of these equivalence classes - set of non terminals
    for (EquivalenceClass equivalenceClassK : equivalenceClassesK) {

        // Check if this equivalence class can generate a start symbol
        bool isStart = true;
        for (string w : equivalenceClassK.second) {
            if (this->D.find(w) == this->D.end()) { // Not found
                isStart = false;
            }
        }

        // Create the non-terminal for this equivalence class
        NonTerminal nonTerminal = isStart ? G.getStartSymbol() : G.getNonTerminalSymbol();
        if (isStart) {
            G.I.insert(nonTerminal);
        }
        G.V.insert(nonTerminal); // Add non-terminal

        auto equivalenceClassesKKlocal = equivalenceClassesKK.find(equivalenceClassK.first);
        if (equivalenceClassesKKlocal == equivalenceClassesKK.end()) { // Not found
            G.equivalenceClasses.insert({ nonTerminal, equivalenceClassK.second });
        } else {
            // Insert equivalence classes from KK
            G.equivalenceClasses.insert({ nonTerminal, equivalenceClassesKKlocal->second });
        }

        // Compute lexical rules
        for (string w : equivalenceClassK.second) {

            distributionNonTerminal.insert({ equivalenceClassK.first, nonTerminal });

            if (w == this->_lambda || this->_alphabet.in(w)) { // terminal or lambda
                G.P.insert({ nonTerminal, new Terminal(w) });
                G.Sigma.insert(Terminal(w));
                lexicalRulesData.insert({ w, nonTerminal });
                continue;
            }

            // Remember the strings that will become binary rules
            binaryRulesData.insert({ nonTerminal, w });
        }
    }

    // Compute binary rules from binary rules data
    if (binaryRulesData.size() > 0) {
        for (pair<NonTerminal, string> binaryRuleData : binaryRulesData) {
            string s = binaryRuleData.second;
            string s1 = s.substr(0, 1);
            string s2 = s.substr(1, 1);

            map<string, NonTerminal>::iterator elementL1 = lexicalRulesData.find(s1);
            map<string, NonTerminal>::iterator elementL2 = lexicalRulesData.find(s2);
            if (elementL1 == lexicalRulesData.end() || elementL2 == lexicalRulesData.end()) {
                string message = "Lexical rule data not found for strings '" + s1 +
                    "' and/or '" + s2 + "'";
                throw exception(message.c_str());
                continue;
            }
            NonTerminalNonTerminal* nTnT = new NonTerminalNonTerminal(
                make_pair(elementL1->second, elementL2->second));
            G.P.insert(make_pair(binaryRuleData.first, nTnT));

            lexicalRulesData.insert({ binaryRuleData.second, binaryRuleData.first });
        }
    }

    // Compute binary rules from KK \ K
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
                string message = "Lexical rule data not found for strings '" + k1 +
                    "' and/or '" + k2 + "'";
                throw exception(message.c_str());
                continue;
            }

            NonTerminalNonTerminal* nTnT = new NonTerminalNonTerminal(
                make_pair(elementK1->second, elementK2->second));
            G.P.insert({ nonTerminal, nTnT });
        }
    }

    return G;
}

void ObservationTable::addPositiveCounterExample(const string w)
{
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

bool ObservationTable::contextSplitsCategory(ContextFreeGrammar& G,
    const Context f, const NonTerminal X)
{
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

ObservationTable::Context ObservationTable::FindContext(ContextFreeGrammar& G,
    const NonTerminal X, const Context f, const string w)
{
    if (this->contextSplitsCategory(G, f, X)) {
        return f;
    }
    // w is not congruent with the strings in X

    string l = f.first;
    string r = f.second;

    if (G.cykYields(X, w)) {
        set<ContextFreeGrammar::Derivation> derivations = G.getDerivationsByString(w);

        // Let X -> YZ yields uv = w be a derivation of w such that Y yields u and Z yields v
        for (ContextFreeGrammar::Derivation derivation: derivations) {
            string u = derivation.second.first;
            string v = derivation.second.second;
            NonTerminal Y = derivation.first.second.getPair().first;
            NonTerminal Z = derivation.first.second.getPair().second;

            // Find a pair of strings u', v' in K such that u' in Y, v' in Z and u' v' in X
            pair<string, string> stringPair = this->_getStringPair(G, Y, Z, X);
            string u1 = stringPair.first;
            string v1 = stringPair.second;

            if (this->_mat.Mem(f.first, u1 + v, f.second)) {
                return this->FindContext(G, Y, make_pair(l, v + r), u);
            } else {
                return this->FindContext(G, Z, make_pair(l + u1, r), v);
            }
        }
    }

    string message = "No context found for non-terminal '" + X.getName() +
        "' and string '" + w + "'";
    throw exception(message.c_str());
}

void ObservationTable::AddContexts(ContextFreeGrammar& G, const string w)
{
    // The string w is not in L
    bool generates = true; // We already know that this grammar generates w
    Context startContext = make_pair(this->_lambda, this->_lambda);

    do {
        // Suppose S yields w for some S in I
        for (NonTerminal nonTerminal : G.I) {
            if (!G.cykYields(nonTerminal, w)) {
                continue;
            }

            // Find context f that splits category referenced by nonTerminal
            Context f = this->FindContext(G, nonTerminal, startContext, w);
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

ObservationTable::EquivalenceClasses ObservationTable::_getEquivalenceClassesK()
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

ObservationTable::EquivalenceClasses ObservationTable::_getEquivalenceClassesKK()
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

    return equivalenceClasses;
}

pair<string, string> ObservationTable::_getStringPair(ContextFreeGrammar& G,
    NonTerminal Y, NonTerminal Z, NonTerminal X)
{
    auto equivalenceClassY = G.equivalenceClasses.find(Y);
    auto equivalenceClassZ = G.equivalenceClasses.find(Z);
    auto equivalenceClassX = G.equivalenceClasses.find(X);

    string message = "No suitable pair of strings found for Y = '" +
        Y.getName() + "', Z = '" + Z.getName() + "' and X = '" + X.getName() + "'";

    if (equivalenceClassY == G.equivalenceClasses.end() ||
        equivalenceClassZ == G.equivalenceClasses.end() ||
        equivalenceClassX == G.equivalenceClasses.end()
    ) {
        throw exception(message.c_str());
    }

    StringSet& eClassY = equivalenceClassY->second;
    StringSet& eClassZ = equivalenceClassZ->second;
    StringSet& eClassX = equivalenceClassX->second;

    // Find a pair of strings u', v' in K such that u' in Y, v' in Z and u'v' in X
    for (string u1 : this->K) {
        for (string v1 : this->K) {

            if (eClassY.find(u1) != eClassY.end() && // u' in Y
                eClassZ.find(v1) != eClassZ.end() && // v' in Z
                eClassX.find(u1 + v1) != eClassX.end() // u'v' in X
            ) {
                return make_pair(u1, v1);
            }
        }
    }

    throw exception(message.c_str());
}
