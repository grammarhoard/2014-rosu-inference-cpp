#include <iostream>

#include "MainHelper.h"

using namespace std;

int main()
{
    string outputLocation =
        "D://Master Thesis//Documentation//Thesis LaTeX//Primitives//ContextFreeGrammars//";
    try {
        MainHelper mainHelper("D://Master Thesis//Sources//Thesis Cpp//CFG//samples//");
        mainHelper.runSample(1, outputLocation, true);
        mainHelper.runSample(2, outputLocation, true);
        mainHelper.runSample(3, outputLocation, true);

        // mainHelper.runSample(4, outputLocation);

        return 0;
    } catch(exception exception) {
        cout << exception.what() << endl;
        return 1;
    }
}
