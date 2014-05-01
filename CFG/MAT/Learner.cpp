#include "Learner.h"

Learner::Learner(MinimallyAdequateTeacher& mat, const string outputLocation, const string outputPrefix) :
    _mat(mat), _outputLocation(outputLocation), _outputPrefix(outputPrefix)
{
}

Learner::~Learner()
{
}

ContextFreeGrammar Learner::LearnCFG()
{
    pair<string, bool> counterExample;
    string w;

    Language & L = this->_mat.getLanguage();
    const string lambda = L.lambda;

    ObservationTable observationTable(this->_mat);
    observationTable.K.insert(lambda);

    // Note: In the original algorithm the alphabet of the language L is included in K
    // set<string> alphabetSet = this->_mat.getLanguage().getAlphabet().get();
    // observationTable.K.insert(alphabetSet.begin(), alphabetSet.end());

    observationTable.F.insert({ lambda, lambda });
    if (L.in(lambda)) {
        observationTable.D.insert(lambda);
    }

    int step = 0;
    while (true) {
        ContextFreeGrammar G = observationTable.MakeGrammar();

        // Save to file
        observationTable.saveToLaTeX(this->_outputLocation, this->_outputPrefix, step);
        G.saveToLaTeX(this->_outputLocation, this->_outputPrefix, step);

        if (this->_mat.Equiv(G)) {
            return G;
        }

        counterExample = this->_mat.getCounterExample();
        w = counterExample.first;

        if (counterExample.second) { // w is not in L(G)
            observationTable.addPositiveCounterExample(w);
        } else {
            observationTable.AddContexts(G, w);
        }

        step++;
    }
}
