#include "ObservationTable.h"

ObservationTable::ObservationTable(MinimallyAdequateTeacher& mat) : _mat(mat),
    _alphabet(this->_mat.getLanguage().getAlphabet()), _lambda(this->_mat.getLanguage().lambda)
{
}

ObservationTable::~ObservationTable()
{
}

void ObservationTable::computeKK()
{
    this->_KK.clear();
    for (string k1 : this->K) {
        for (string k2 : this->K) {
            this->_KK.insert(k1 + k2);
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
    for (string k : this->_KK) {
        for (Context f : this->F) {
            this->insert(f, k);
        }
    }
}

bool ObservationTable::equivalent(const string u, const string v)
{
    if (u == v) {
        return true;
    }
    for (Context context : this->F) {
        if (this->_table[u][context] != this->_table[v][context]) {
            return false;
        }
    }
    return true;
}

void ObservationTable::MakeConsistent()
{
    //TODO ObservationTable::MakeConsistent -> This is the hottest path (do something about it)
    for (Context context : this->F) {
        string l = context.first;
        string r = context.second;

        //TODO maybe: Find a better way to get the four strings (ObservationTable::MakeConsistent)
        for (string u1 : this->K) {
            for (string u2 : this->K) {
                for (string v1 : this->K) {
                    for (string v2 : this->K) {

                        //TWEAK If we put the two equivalent calls in the end, it is faster
                        if (this->D.find(l + u1 + v1 + r) != this->D.end() && // lu1v1r in D
                            this->D.find(l + u2 + v2 + r) == this->D.end() && // lu2v2r not in D
                            this->equivalent(u1, u2) && this->equivalent(v1, v2)
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
    // Preparation phase
    this->build();
    this->MakeConsistent();

    ContextFreeGrammar G;
    map<NonTerminal, StringSet> binaryRulesData;  // map(non-terminal: string w)
    map<ContextSet, NonTerminal> distributionNonTerminal; // map(distribution: non-terminal)

    // Build the grammar
    this->_buildLexicalRules(G, binaryRulesData, distributionNonTerminal);
    if (binaryRulesData.size() > 0) {
        this->_buildBinaryRules(G, binaryRulesData);
    }
    this->_buildBinaryRulesFromK(G, distributionNonTerminal);

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
    StringSet equivalenceClasses = G.getEquivalenceClasses().find(X)->second;
    string l = f.first;
    string r = f.second;

    for (string u : equivalenceClasses) {
        for (string v : equivalenceClasses) {
            if (u == v) {
                continue;
            }

            if (this->_mat.Mem(l, u, r) && !this->_mat.Mem(l, v, r)) {
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

    string l = f.first;
    string r = f.second;
    set<ContextFreeGrammar::Derivation> derivations = G.getDerivationsByString(w);

    // Let X -> YZ yields u+v = w be a derivation of w such that Y yields u and Z yields v
    for (ContextFreeGrammar::Derivation derivation : derivations) {
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
        }

        return this->FindContext(G, Z, make_pair(l + u1, r), v);
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

            // Find context f that splits the category referenced by nonTerminal
            Context f = this->FindContext(G, nonTerminal, startContext, w);
            this->_addContext(f);
            ContextFreeGrammar G = this->MakeGrammar();
            generates = G.generates(w);
            break;
        }
    } while (generates);
}

void ObservationTable::saveToLaTeX(const string fileName, const string prefix, const int step)
{
    string fullFileName = fileName + prefix + "_ObservationTable_Step" + to_string(step) + ".tex";
    ofstream myfile(fullFileName); bool hasKK = false;
    if (!myfile.is_open()) {
        string message = "File '" + fullFileName + "' could not be opened";
        throw exception(message.c_str());
    }

    // Header
    myfile
    //    << "\\begin{table}[!htb]" << endl
        << "\\begin{tabularx}{\\textwidth}{|X|";
    for (Context f : this->F) {
        myfile << "c|";
    }
    myfile << "}" << endl
        << "    \\hline" << endl
        << "    K\\textbackslash F";
    for (Context f : this->F) {
        myfile << " & (" << this->getLaTeXString(f.first) << ", "
            << this->getLaTeXString(f.second) << ")";
    }
    myfile << " \\\\ \\hline" << endl;

    // Main
    for (string u : this->K) { // Output K
        myfile << "    " << this->getLaTeXString(u);
        for (Context f : this->F) {
            myfile << " & " << this->getLaTeXString(this->_table[u][f]);
        }
        myfile << "\\\\";
    }
    myfile << " \\hline" << endl;
    for (string u : this->_KK) { // Output KK / K
        if (this->K.find(u) != this->K.end()) { // Found in K
            continue;
        }
        hasKK = true;
        myfile << "    " << this->getLaTeXString(u);
        for (Context f : this->F) {
            myfile << " & " << this->getLaTeXString(this->_table[u][f]);
        }
        myfile << "\\\\";
    }
    if (hasKK) {
        myfile << " \\hline" << endl;
    }

    // Footer
    myfile << endl
        << "\\end{tabularx}" << endl
        << "\\caption{Step " << step << "}" << endl
        << "\\label{table:ObservationTable" << prefix << "_step" << step << "}" << endl
    //    << endl
    //    << "\\caption{" << prefix << " Step " << step << "}" << endl
    //    << "\\label{table:" << prefix << "_step" << this->getLaTeXString(step) << "}" << endl
    //    << "\\end{table}" << endl
    ;

    myfile.close();

}

string ObservationTable::getLaTeXString(const string s)
{
    return s == this->_lambda ? "$\\lambda$" : s;
}

string ObservationTable::getLaTeXString(const bool b)
{
    return b ? "1" : "0";
}

void ObservationTable::_addContext(Context f)
{
    this->F.insert(f);

    // Increase D and fill in the observation table
    for (string k : this->_KK) {
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

ObservationTable::EquivalenceClasses ObservationTable::_getEquivalenceClasses(bool ofK)
{
    EquivalenceClasses equivalenceClasses;
    StringSet& localK = ofK ? this->K : this->_KK;

    // Divide K into equivalence classes according to \equiv F
    for (string k : localK) {
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

pair<string, string> ObservationTable::_getStringPair(ContextFreeGrammar& G,
    NonTerminal Y, NonTerminal Z, NonTerminal X)
{
    ContextFreeGrammar::EquivalenceClasses& equivalenceClasses = G.getEquivalenceClasses();
    auto equivalenceClassY = equivalenceClasses.find(Y);
    auto equivalenceClassZ = equivalenceClasses.find(Z);
    auto equivalenceClassX = equivalenceClasses.find(X);

    string message = "No suitable pair of strings found for Y = '" +
        Y.getName() + "', Z = '" + Z.getName() + "' and X = '" + X.getName() + "'";

    if (equivalenceClassY == equivalenceClasses.end() ||
        equivalenceClassZ == equivalenceClasses.end() ||
        equivalenceClassX == equivalenceClasses.end()
    ) {
        throw exception(message.c_str());
    }

    StringSet& eClassY = equivalenceClassY->second;
    StringSet& eClassZ = equivalenceClassZ->second;
    StringSet& eClassX = equivalenceClassX->second;

    // Find a pair of strings u', v' in K such that u' in Y, v' in Z and u'+v' in X
    for (string u1 : this->K) {
        for (string v1 : this->K) {

            if (eClassY.find(u1) != eClassY.end() && // u' in Y
                eClassZ.find(v1) != eClassZ.end() && // v' in Z
                eClassX.find(u1 + v1) != eClassX.end() // u'+v' in X
            ) {
                return make_pair(u1, v1);
            }
        }
    }

    throw exception(message.c_str());
}

void ObservationTable::_buildLexicalRules(ContextFreeGrammar& G,
    map<NonTerminal, StringSet>& binaryRulesData,
    map<ContextSet, NonTerminal>& distributionNonTerminal)
{
    EquivalenceClasses equivalenceClassesK = this->_getEquivalenceClasses();
    EquivalenceClasses equivalenceClassesKK = this->_getEquivalenceClasses(false);

    // Let V be the set of these equivalence classes - set of non terminals
    for (EquivalenceClass equivalenceClassK : equivalenceClassesK) {
        //TWEAK Avoid unnecessary processing
        if (equivalenceClassK.first.size() == 0) {
            continue;
        }

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
        G.V.insert(nonTerminal);
        distributionNonTerminal.insert({ equivalenceClassK.first, nonTerminal });

        // Save equivalence classes
        auto equivalenceClassesKKlocal = equivalenceClassesKK.find(equivalenceClassK.first);
        if (equivalenceClassesKKlocal == equivalenceClassesKK.end()) { // Not found
            G.getEquivalenceClasses().insert({ nonTerminal, equivalenceClassK.second });
        } else {
            // Insert equivalence classes from KK
            G.getEquivalenceClasses().insert({ nonTerminal, equivalenceClassesKKlocal->second });
        }

        // Compute lexical rules
        for (string w : equivalenceClassK.second) {
            if (w == this->_lambda || this->_alphabet.in(w)) { // terminal or lambda
                Production p(nonTerminal, new Terminal(w));
                G.P.insert(p);
                G.Sigma.insert(Terminal(w));
                G.getLexicalRules().insert({ w, nonTerminal });
                continue;
            }

            // Remember the strings that will become binary rules
            auto binaryRuleData = binaryRulesData.find(nonTerminal);
            if (binaryRuleData != binaryRulesData.end()) {
                binaryRuleData->second.insert(w);
            } else {
                binaryRulesData.insert({ nonTerminal, { w } });
            }
        }
    }
}

void ObservationTable::_buildBinaryRules(ContextFreeGrammar& G,
    map<NonTerminal, StringSet>& binaryRulesData)
{
    for (pair<NonTerminal, StringSet> binaryRuleData : binaryRulesData) {
        for (string s : binaryRuleData.second) {
            NonTerminalNonTerminal* nTnT = G.getNonTerminalPairForString(s);
            G.P.insert(Production(binaryRuleData.first, nTnT));
            G.getLexicalRules().insert({ s, binaryRuleData.first });
        }
    }
}

void ObservationTable::_buildBinaryRulesFromK(ContextFreeGrammar& G,
    map<ContextSet, NonTerminal>& distributionNonTerminal)
{
    // Compute binary rules from KK \ K
    for (string k1 : this->K) {
        for (string k2 : this->K) {
            string kk = k1 + k2;

            if (this->K.find(kk) != this->K.end()) { // Found in K
                continue;
            }

            // Get the non-terminal based on this distribution of this string
            ContextSet distribution = this->_getDistributionByK(kk);
            auto nonTerminalPair = distributionNonTerminal.find(distribution);
            if (nonTerminalPair == distributionNonTerminal.end()) { // Not found
                continue;
            }

            // Check if we have a lexical rule for these elements
            map<string, NonTerminal>::iterator elementK1 = G.getLexicalRules().find(k1);
            map<string, NonTerminal>::iterator elementK2 = G.getLexicalRules().find(k2);
            if (elementK1 == G.getLexicalRules().end() || elementK2 == G.getLexicalRules().end()) {
                string message = "Lexical rule data not found for strings '" + k1 +
                    "' and/or '" + k2 + "'";
                throw exception(message.c_str());
                continue;
            }

            // Build the binary rule of the form N -> PQ
            NonTerminalNonTerminal* nTnT = new NonTerminalNonTerminal(
                make_pair(elementK1->second, elementK2->second));
            G.P.insert(Production(nonTerminalPair->second, nTnT));
        }
    }

}
