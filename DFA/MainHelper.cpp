#include "MainHelper.h"

MainHelper::MainHelper()
{
}

MainHelper::~MainHelper()
{
}

void MainHelper::populateTrainingSet1(TrainingSet & trainingSet)
{
    trainingSet.addSample("1", true);
    trainingSet.addSample("110", true);
    trainingSet.addSample("01", true);
    trainingSet.addSample("001", true);

    trainingSet.addSample("00", false);
    trainingSet.addSample("10", false);
    trainingSet.addSample("000", false);
}

void MainHelper::populateTrainingSet2(TrainingSet & trainingSet)
{
    trainingSet.addSample("1", true);
    trainingSet.addSample("11", true);
    trainingSet.addSample("1111", true);

    trainingSet.addSample("0", false);
    trainingSet.addSample("101", false);
}

void MainHelper::populateTrainingSet3(TrainingSet & trainingSet)
{
    trainingSet.addSample("a", true);
    trainingSet.addSample("abaa", true);
    trainingSet.addSample("bb", true);

    trainingSet.addSample("abb", false);
    trainingSet.addSample("b", false);
}

void MainHelper::populateTrainingSetFromFile(TrainingSet & trainingSet, string fileName)
{
    try {
        trainingSet.addSampleFromFile(fileName, true);
    }
    catch (exception exception) {
        LOG(FATAL) << exception.what();
    }
}

Apta MainHelper::buildApta(TrainingSet & trainingSet, bool useWhiteNodes, string aptaVisualizationOutputName)
{
    Apta apta;
    apta.build(trainingSet, useWhiteNodes);

    AptaVisualization aptaVisualization(aptaVisualizationOutputName.c_str());
    aptaVisualization.build(apta);

    return apta;
}

void MainHelper::runSimpleTrainingSet(string visualizationPrefix, int sampleNo)
{
    clock_t time;

    // Add positive and negative samples
    LOG(INFO) << "----- Building training set ..."; time = clock();
    TrainingSet trainingSet;

    switch (sampleNo) {
    case 2:
        visualizationPrefix += "TS2\\";
        populateTrainingSet2(trainingSet);
        break;
    case 3:
        visualizationPrefix += "TS3\\";
        populateTrainingSet3(trainingSet);
        break;
    case 1:
    default:
        visualizationPrefix += "TS1\\";
        populateTrainingSet1(trainingSet);
    }
    LOG(INFO) << "Finished building training: " << clock() - time << " ms";

    // Build APTA
    LOG(INFO) << "Building APTA for Exbar ..."; time = clock();
    Apta apta;
    string aptaVisualizationOutputName;

    aptaVisualizationOutputName = visualizationPrefix + "Exbar_0_Apta.svg";
    apta = buildApta(trainingSet, false, aptaVisualizationOutputName);
    LOG(INFO) << "Finished building APTA for Exbar: " << clock() - time << " ms";

    LOG(INFO) << "Running Exbar ..."; time = clock();
    Exbar exbar(apta, true, visualizationPrefix + "Exbar_");
    exbar.search();
    LOG(INFO) << "Finished running Exbar: " << clock() - time << " ms";

    // Build APTA again because now we also have white nodes
    LOG(INFO) << "Building APTA for EDSM..."; time = clock();
    aptaVisualizationOutputName = visualizationPrefix + "Edsm_0_Apta.svg";
    apta = buildApta(trainingSet, true, aptaVisualizationOutputName);
    LOG(INFO) << "Finished building APTA for EDSM: " << clock() - time << " ms";

    LOG(INFO) << "Running EDSM..."; time = clock();
    Edsm edsm(apta, true, visualizationPrefix + "Edsm_");
    edsm.search();
    LOG(INFO) << "Finished running EDSM: " << clock() - time << " ms";

    // Build DFA
    LOG(INFO) << "Building DFA ..."; time = clock();
    Dfa dfa;
    dfa.build(apta);
    dfa.get();
    LOG(INFO) << "Finished building DFA: " << clock() - time << " ms";
}

void MainHelper::runTrainingSetFromFile(string fileName)
{
    clock_t time;

    // Add positive and negative samples
    LOG(INFO) << "----- Building training set ..."; time = clock();
    TrainingSet trainingSet;
    populateTrainingSetFromFile(trainingSet, fileName);
    LOG(INFO) << "Finished building training: " << clock() - time << " ms";

    // Build APTA
    LOG(INFO) << "Building APTA for Exbar ..."; time = clock();
    Apta apta1;
    apta1.build(trainingSet, false);
    LOG(INFO) << "Finished building APTA for Exbar: " << clock() - time << " ms";

    LOG(INFO) << "Running Exbar ..."; time = clock();
    Exbar exbar(apta1, false);
    exbar.search();
    LOG(INFO) << "Finished running Exbar: " << clock() - time << " ms";

    // Build APTA again because now we also have white nodes
    LOG(INFO) << "Building APTA for EDSM..."; time = clock();
    Apta apta2;
    apta2.build(trainingSet, true);
    LOG(INFO) << "Finished building APTA for EDSM: " << clock() - time << " ms";

    LOG(INFO) << "Running EDSM..."; time = clock();
    Edsm edsm(apta2, false);
    edsm.search();
    LOG(INFO) << "Finished running EDSM: " << clock() - time << " ms";
}
