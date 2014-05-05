#include <string>

#include "Alphabet.h"
#include "Language.h"
#include "MinimallyAdequateTeacher.h"
#include "Learner.h"
#include "ContextFreeGrammar.h"
#include "easylogging++.h"

using namespace std;

#pragma once
class MainHelper
{
public:
    MainHelper(const string samplesLocation);
    ~MainHelper();

    void runSample(int sampleNumber, const string outputLocation, const bool output = false);

private:
    const string _samplesLocation;
};
