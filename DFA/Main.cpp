#include "MainHelper.h"

#include "easylogging++.h"
_INITIALIZE_EASYLOGGINGPP


using namespace std;

int main()
{
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.set(el::Level::Global, el::ConfigurationType::Format, "%datetime %level %msg");
    defaultConf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "false");
    defaultConf.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize, "33554432");
    defaultConf.set(el::Level::Global, el::ConfigurationType::PerformanceTracking, "true");
    defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled, "false");
    el::Loggers::reconfigureLogger("default", defaultConf);

    MainHelper mainHelper;
    string visualizationPrefix = "D:\\Master Thesis\\Documentation\\Thesis LaTeX\\Pictures\\DFA\\";
    // mainHelper.runSimpleTrainingSet(visualizationPrefix);
    // mainHelper.runSimpleTrainingSet(visualizationPrefix, 2);
    // mainHelper.runSimpleTrainingSet(visualizationPrefix, 3);

    mainHelper.runTrainingSetFromFile("sample\\sample2.php");

    return 0;
}
