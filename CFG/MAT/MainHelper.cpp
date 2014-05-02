#include "MainHelper.h"

MainHelper::MainHelper(const string samplesLocation) : _samplesLocation(samplesLocation)
{
}

MainHelper::~MainHelper()
{
}

void MainHelper::runSample(int sampleNumber, const string outputLocation, const bool output)
{
    // Setup language L
    Language language;
    language.addSampleFromFile(this->_samplesLocation + "sample" + to_string(sampleNumber) + ".txt");

    // Setup the Minimally Adequate Teacher
    MinimallyAdequateTeacher mat(language);

    // Learn the grammar
    Learner learner(mat, output, outputLocation, "Sample" + to_string(sampleNumber));
    ContextFreeGrammar grammar = learner.LearnCFG();
}
