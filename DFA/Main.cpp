#include "TrainingSet.h"
#include "Apta.h"
// #include "Exbar.h"

#include <iostream> // std::cin, std::cout
#include <stdio.h>  // IO library
#include <errno.h>  // std::strerror

using namespace std;

void populateTrainingSet1(TrainingSet & trainingSet);
void populateTrainingSet2(TrainingSet & trainingSet);
void populateTrainingSet3(TrainingSet & trainingSet);
void populateTrainingSetFromFile(TrainingSet & trainingSet, const char * fileName);

int main()
{
    // Add positive and negative samples
    TrainingSet trainingSet;

    // populateTrainingSet1(trainingSet);
    // populateTrainingSet2(trainingSet);
    // populateTrainingSet3(trainingSet);
    populateTrainingSetFromFile(trainingSet, "thesis_DFA.sln");

    // Build APTA
    Apta apta;
    apta.build(trainingSet, false);

    /*
    // Build APTA
    APTA = APTA::build(trainingSet.build(), false);
    print("APTA 0: "); iprintln(APTA);
    // GraphVis::build(APTA);

    // EXBAR Search
    exbarSearch();
    GraphVis::build(APTA);

    // Build DFA
    DFA DFA = DFA::build(APTA);
    print("DFA 0: "); iprintln(DFA);
    */

    cout << "0" << endl;
    return 0;
}

void populateTrainingSet1(TrainingSet & trainingSet)
{
    trainingSet.addSample("1", true);
    trainingSet.addSample("110", true);
    trainingSet.addSample("01", true);
    trainingSet.addSample("001", true);

    trainingSet.addSample("00", false);
    trainingSet.addSample("10", false);
    trainingSet.addSample("000", false);
}

void populateTrainingSet2(TrainingSet & trainingSet)
{
    trainingSet.addSample("1", true);
    trainingSet.addSample("11", true);
    trainingSet.addSample("1111", true);

    trainingSet.addSample("0", false);
    trainingSet.addSample("101", false);
}

void populateTrainingSet3(TrainingSet & trainingSet)
{
    trainingSet.addSample("a", true);
    trainingSet.addSample("abaa", true);
    trainingSet.addSample("bb", true);

    trainingSet.addSample("abb", false);
    trainingSet.addSample("b", false);
}

void populateTrainingSetFromFile(TrainingSet & trainingSet, const char * fileName)
{
    FILE * pFile;
    errno_t errorNo;
    errorNo = fopen_s(&pFile, fileName, "r");
    if (errorNo == 0) {
        trainingSet.addSampleFromFile(pFile, true);
        return;
    }

    char buffer[1024];
    strerror_s(buffer, 1024, errorNo);
    fprintf(stderr, "Error opening file '%s': %s\n", fileName, buffer);
}
