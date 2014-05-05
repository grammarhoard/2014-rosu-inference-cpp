#include "Learner.h"

Learner::Learner(MinimallyAdequateTeacher& mat, const bool output,
    const string outputLocation, const string outputPrefix) :
    _mat(mat), _output(output), _outputLocation(outputLocation), _outputPrefix(outputPrefix)
{
}

Learner::~Learner()
{
}

ContextFreeGrammar Learner::LearnCFG()
{
    clock_t time;
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

    time = clock();
    LOG(INFO) << "Finished initialization: " << clock() - time << " ms";

    int step = 0;
    while (true) {
        ContextFreeGrammar G = observationTable.MakeGrammar();

        // Save to file
        if (this->_output) {
            observationTable.saveToLaTeX(this->_outputLocation, this->_outputPrefix, step);
            G.saveToLaTeX(this->_outputLocation, this->_outputPrefix, step);
        }
        LOG(INFO) << "Step " << step << " - " << clock() - time << " ms"; time = clock();

        if (this->_mat.Equiv(G)) {
            return G;
        }

        counterExample = this->_mat.getCounterExample();
        if (w == counterExample.first) {
            string message = "This counter-example ('" + w + "') is received twice in a row";
            throw exception(message.c_str());
        }
        w = counterExample.first;

        if (counterExample.second) { // w is not in L(G)
            observationTable.addPositiveCounterExample(w);
        } else {
            observationTable.AddContexts(G, w);
        }

        step++;
    }
}
