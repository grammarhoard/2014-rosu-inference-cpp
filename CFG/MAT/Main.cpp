#include "Alphabet.h"
#include "Language.h"
#include "MinimallyAdequateTeacher.h"
#include "Learner.h"
#include "ContextFreeGrammar.h"

#include <iostream>
using namespace std;

int main()
{
    // Build alphabet Sigma, which is known
    Alphabet alphabet;
    alphabet.insert("a");
    alphabet.insert("b");

    // Setup the Minimally Adequate Teacher
    Language language(alphabet);
    // language.addSample("ab");
    // language.addSample("abab");
    // language.addSample("aabb");

    try {
        language.addSampleFromFile("D://Master Thesis//Sources//Thesis Cpp//CFG//samples//sample0.txt");
        MinimallyAdequateTeacher mat(language);

        // Learn the grammar
        Learner learner(mat);
        ContextFreeGrammar grammar = learner.LearnCFG();

        return 0;
    } catch(exception exception) {
        cout << exception.what();
    }
}
