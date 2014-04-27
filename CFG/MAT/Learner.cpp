#include "Learner.h"

Learner::Learner(MinimallyAdequateTeacher& mat) : _mat(mat)
{
}

Learner::~Learner()
{
}

ContextFreeGrammar Learner::LearnCFG()
{
    Language & L = this->_mat.getLanguage();
    const string lambda = L.lambda;
    ObservationTable observationTable(this->_mat);

    observationTable.K.insert(lambda);

    //TODO maybe: not sure if the alphabet of the language L should be included in K (Learner::LearnCFG())
    // set<string> alphabetSet = this->_mat.getLanguage().getAlphabet().get();
    // observationTable.K.insert(alphabetSet.begin(), alphabetSet.end());

    observationTable.F.insert({ lambda, lambda });
    if (L.in(lambda)) {
        observationTable.D.insert(lambda);
    }

    while (true) {
        ContextFreeGrammar G = observationTable.MakeGrammar();

        if (this->_mat.Equiv(G)) {
            return G;
        }
        string w = this->_mat.getCounterExample();

        // Query the current language L(G)
        if (!G.generates(w)) { // Not found -> w is not in L(G)
            observationTable.addPositiveCounterExample(w);
        } else {
            observationTable.AddContexts(w);
        }
    }
}
