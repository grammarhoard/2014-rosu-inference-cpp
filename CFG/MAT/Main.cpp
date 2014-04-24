#include "Alphabet.h"
#include "ContextFreeLanguage.h"
#include "MinimallyAdequateTeacher.h"
#include "Learner.h"

int main()
{
    // Setup the Minimally Adequate Teacher
    Alphabet alphabet;
    alphabet.insert("a");
    alphabet.insert("b");
    ContextFreeLanguage language(alphabet);
    MinimallyAdequateTeacher mat(language);

    // Learn the grammar
    Learner learner(mat);
    learner.LearnCFG();

    return 0;
}
