#include "MainHelper.h"

#include "easylogging++.h"
_INITIALIZE_EASYLOGGINGPP


int main()
{
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.set(el::Level::Global, el::ConfigurationType::Format, "%datetime %level %msg");
    defaultConf.set(el::Level::Global, el::ConfigurationType::LogFlushThreshold, "1");
    defaultConf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "true");
    defaultConf.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize, "33554432");
    defaultConf.set(el::Level::Global, el::ConfigurationType::PerformanceTracking, "true");
    defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled, "false");
    el::Loggers::reconfigureLogger("default", defaultConf);

    MainHelper mainHelper;
    string visualizationPrefix = "D:\\Master Thesis\\Documentation\\Thesis LaTeX\\Pictures\\DFA\\";
    string legendVisualizationPrefix = visualizationPrefix + "Legend.svg";
    string legendVisualizationPrefix2 = visualizationPrefix + "Legend2.svg";
    mainHelper.buildLegendVisualization(legendVisualizationPrefix);
    mainHelper.buildLegendVisualization(legendVisualizationPrefix2); // We need it twice because latex will not include the same legend twice
    mainHelper.runSimpleTrainingSet(visualizationPrefix);
    mainHelper.runSimpleTrainingSet(visualizationPrefix, 2);
    mainHelper.runSimpleTrainingSet(visualizationPrefix, 3);

    // mainHelper.runTrainingSetFromFile("samples//sample1.php");

    return 0;
}
