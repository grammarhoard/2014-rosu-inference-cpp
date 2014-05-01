#include <iostream>

#include "MainHelper.h"

using namespace std;

int main()
{
    string outputLocation =
        "D://Master Thesis//Documentation//Thesis LaTeX//Chapters//ContextFreeGrammars//";
    try {
        MainHelper mainHelper("D://Master Thesis//Sources//Thesis Cpp//CFG//samples//");
        mainHelper.runSample(1, outputLocation);
        mainHelper.runSample(2, outputLocation);
        mainHelper.runSample(3, outputLocation);

        return 0;
    } catch(exception exception) {
        cout << exception.what() << endl;
    }
}
