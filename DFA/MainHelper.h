#include "TrainingSet.h"
#include "Apta.h"
#include "AptaVisualization.h"
#include "Exbar.h"
#include "Edsm.h"
#include "Dfa.h"
#include "easylogging++.h"

#pragma once
class MainHelper
{
public:
    MainHelper();
    ~MainHelper();

    void populateTrainingSet1(TrainingSet & trainingSet);
    void populateTrainingSet2(TrainingSet & trainingSet);
    void populateTrainingSet3(TrainingSet & trainingSet);
    void populateTrainingSetFromFile(TrainingSet & trainingSet, string fileName);
    Apta buildApta(TrainingSet & trainingSet, bool useWhiteNodes, string aptaVisualizationOutputName);

    void buildLegendVisualization(string visualizationPrefix);
    void runSimpleTrainingSet(string visualizationPrefix, int sampleNo = 1);
    void runTrainingSetFromFile(string fileName);
};

