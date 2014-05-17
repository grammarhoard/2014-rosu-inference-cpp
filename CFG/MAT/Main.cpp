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

    string outputLocation =
        "D://Master Thesis//Documentation//Thesis LaTeX//Primitives//ContextFreeGrammars//";
    try {
        MainHelper mainHelper("D://Master Thesis//Sources//Thesis Cpp//CFG//samples//");
        // mainHelper.runSample(1, outputLocation, true);
        // mainHelper.runSample(2, outputLocation, true);
        mainHelper.runSample(3, outputLocation);

        // mainHelper.runSample(4, outputLocation, true);
        // mainHelper.runSample(5, outputLocation);
        // mainHelper.runSample(6, outputLocation);

        return 0;
    } catch(exception exception) {
        LOG(FATAL) << exception.what();
        return 1;
    }
}
