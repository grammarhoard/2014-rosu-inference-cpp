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

    // Note: In the original algorithm the alphabet of the language L is included in K
    // set<string> alphabetSet = this->_mat.getLanguage().getAlphabet().get();
    // observationTable.K.insert(alphabetSet.begin(), alphabetSet.end());

    observationTable.F.insert({ lambda, lambda });
    if (L.in(lambda)) {
        observationTable.D.insert(lambda);
    }

    return observationTable.LearnCFG();
}
