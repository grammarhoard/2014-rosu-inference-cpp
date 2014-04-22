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
    for (string k : this->K) {
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
    if (this->K.size() < 4) {
        return true;
    }

    vector<string> vectorK(this->K.begin(), this->K.end());
    do {
        string u1 = vectorK[0];
        string u2 = vectorK[1];
        string v1 = vectorK[2];
        string v2 = vectorK[3];

        if (this->equivalent(u1, u2) && this->equivalent(v1, v2) && !this->equivalent(u1 + u2, v1 + v2)) {
            return false;
        }
    } while (next_permutation(vectorK.begin(), vectorK.begin() + 4));

    return true;
}

ObservationTable::ContextSet ObservationTable::MakeConsistent()
{
    while (!this->isConsistent()) {
        // Find u1, u2 and v1, v2 in K, and (l, r) in F
        //     such that u1 equivalent to u2 and v1 equivalent to v2,
        //     l u1 v1 r in D and l u2 v2 4 is not in D

        //TODO improve performance (the permutations are done twice (once for isConsistent and once here)
        vector<string> vectorK(this->K.begin(), this->K.end());
        do {
            string u1 = vectorK[0];
            string u2 = vectorK[1];
            string v1 = vectorK[2];
            string v2 = vectorK[3];

            for (Context context : this->F) {
                string l = context.first;
                string r = context.second;
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
                }
            }
        } while (next_permutation(vectorK.begin(), vectorK.begin() + 4));
    }

    //TODO maybe we don't need it but the Algorithm 1 from the paper returns it
    return this->F;
}

ObjectiveContextFreeGrammar ObservationTable::MakeGrammar()
{
    ObjectiveContextFreeGrammar G;
    map<string, string> equivalenceClasses; // map(k, value)

    // Divide K into equivalence classes according to \equiv F
    for (string k : this->K) {
        if (equivalenceClasses.find(k) != equivalenceClasses.end()) { // Found
            continue;
        }

        //TODO improve this
        equivalenceClasses.insert({ k, k });
    }

    char currentTerminal = 'A';

    // Let V be the set of these equivalence classes - set of non terminals
    for (pair<string, string> equivalenceClass : equivalenceClasses) {
        if (equivalenceClass.first == "") {
            G.V.insert("S");
            continue;
        }

        G.V.insert(string(1, currentTerminal));
        currentTerminal = static_cast<char>(currentTerminal + 1);
        //TODO what happens when you don't have enough letters :P
    }

    //TODO Let I ... - set of initial symbols
    G.I = G.V; //TODO for now

    // Populate productions
    ObjectiveContextFreeGrammar::Production p;
    for (string N: G.V) { //TODO N seems to be a nonterminal
        p = make_pair(N, ""); //TODO get it's value
        G.P.insert(p);

        //TODO this
        p = make_pair(N, "SS");
        G.P.insert(p);

        //TODO another case
    }

    return G;
}

void ObservationTable::addCounterExample(string w, bool positive)
{
    if (positive) { // positive
        // Add Sub(w) to K
        StringSet substrings = this->getSub(w);
        this->K.insert(substrings.begin(), substrings.end());

        // Compute KK
        StringSet KK = this->getKK();
        this->K.insert(KK.begin(), KK.end());
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