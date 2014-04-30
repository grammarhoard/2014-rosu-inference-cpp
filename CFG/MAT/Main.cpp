#include "Alphabet.h"
#include "Language.h"
#include "MinimallyAdequateTeacher.h"
#include "Learner.h"
#include "ContextFreeGrammar.h"

#include <iostream>
using namespace std;

int main()
{
    try {
        // Setup language L
        Language language;
        language.addSampleFromFile("D://Master Thesis//Sources//Thesis Cpp//CFG//samples//sample1.txt");

        // Setup the Minimally Adequate Teacher
        MinimallyAdequateTeacher mat(language);

        // Learn the grammar
        Learner learner(mat);
        ContextFreeGrammar grammar = learner.LearnCFG();

        return 0;
    } catch(exception exception) {
        cout << exception.what() << endl;
    }
}
