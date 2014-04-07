#include "TrainingSet.h"
#include "Apta.h"
#include "AptaVisualization.h"
#include "Exbar.h"
#include "Edsm.h"
#include "Dfa.h"

#include <iostream> // cin, cout, cerr

using namespace std;

void populateTrainingSet1(TrainingSet & trainingSet);
void populateTrainingSet2(TrainingSet & trainingSet);
void populateTrainingSet3(TrainingSet & trainingSet);
void populateTrainingSetFromFile(TrainingSet & trainingSet);

int main()
{
    // Add positive and negative samples
    TrainingSet trainingSet;

    string visualizationPrefix = "TS1\\";
    populateTrainingSet1(trainingSet);
    // populateTrainingSet2(trainingSet);
    // populateTrainingSet3(trainingSet);
    // populateTrainingSetFromFile(trainingSet);

    // Build APTA
    Apta apta;
    apta.build(trainingSet, false);

    string aptaVisualizationOutputName = visualizationPrefix + "Apta.svg";
    AptaVisualization aptaVisualization(aptaVisualizationOutputName.c_str());
    aptaVisualization.build(apta);

    // EXBAR Search
    Exbar exbar(apta, visualizationPrefix + "Exbar_");
    exbar.search();

    Edsm edsm(apta, visualizationPrefix + "Edsm_");
    edsm.search();

    // Build DFA
    Dfa dfa;
    dfa.build(apta);
    dfa.get();

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

void populateTrainingSetFromFile(TrainingSet & trainingSet)
{
    try {
        trainingSet.addSampleFromFile("sample\\sample2.php", true);
        // trainingSet.addSampleFromFile("sample", true);
    }
    catch (exception exception) {
        cerr << exception.what() << endl;
    }
}
