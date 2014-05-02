#include "MinimallyAdequateTeacher.h"

MinimallyAdequateTeacher::MinimallyAdequateTeacher(Language& L): _L(L)
{
}

MinimallyAdequateTeacher::~MinimallyAdequateTeacher()
{
}

Language& MinimallyAdequateTeacher::getLanguage()
{
    return this->_L;
}

bool MinimallyAdequateTeacher::Mem(const string l, const string u, const string r)
{
    return this->_L.in(l + u + r);
}

bool MinimallyAdequateTeacher::Equiv(ContextFreeGrammar& G)
{
    // The learner provides the teacher with a hypothesis, and
    //     the teacher will either confirm that it is correct, or
    //     it will provide the learner with a counter-example

    this->_counterExample = make_pair("", false);
    return this->_checkUndergeneration(G) && this->_checkOvergeneration(G);
}

pair<string, bool> MinimallyAdequateTeacher::getCounterExample()
{
    return this->_counterExample;
}

bool MinimallyAdequateTeacher::_checkUndergeneration(ContextFreeGrammar& G)
{
    for (string s : this->_L.getData()) {
        if (!G.generates(s)) {
            this->_counterExample = make_pair(s, true);
            return false;
        }
    }
    return true;
}

bool MinimallyAdequateTeacher::_checkOvergeneration(ContextFreeGrammar& G)
{
    // If the partition of KK into classes is correct, then we will not overgeneralise
    // If we overgeneralise, then there must be two strings w1, w2 in KK that
    //     appear to be congruent but are not
    //     so we need to add a feature/context to have a more fine division into classes
    //     so that the two string w1 and w2 are in different classes
    set<string>& alphabet = this->_L.getAlphabet().get();

    for (ContextFreeGrammar::EquivalenceClass equivalenceClass : G.getEquivalenceClasses()) {
        for (string u : equivalenceClass.second) {
            for (string v : equivalenceClass.second) {
                if (u == v) {
                    continue;
                }

                // We already know that the strings are equivalent
                // Look for a context f with l, r in Sigma such that l+u+r in L and l+v+r not in L
                for (string l : alphabet) {
                    for (string r : alphabet) {
                        if (this->Mem(l, u, r) && !this->Mem(l, v, r)) {
                            this->_counterExample = make_pair(l + v + r, false);
                            return false;
                        }
                    }
                }

            } // end for(string v ...)
        }
    }

    return true;
}
