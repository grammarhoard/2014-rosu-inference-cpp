#include <string>

#include "Alphabet.h"
#include "Language.h"
#include "MinimallyAdequateTeacher.h"
#include "Learner.h"
#include "ContextFreeGrammar.h"

using namespace std;

#pragma once
class MainHelper
{
public:
    MainHelper(const string samplesLocation);
    ~MainHelper();

    void runSample(int sampleNumber, const string outputLocation);

private:
    const string _samplesLocation;
};
