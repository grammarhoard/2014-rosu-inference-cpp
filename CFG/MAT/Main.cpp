#include "Alphabet.h"
#include "Language.h"
#include "MinimallyAdequateTeacher.h"
#include "Learner.h"

int main()
{
    // Build alphabet Sigma, which is known
    Alphabet alphabet;
    alphabet.insert("a");
    alphabet.insert("b");

    // Setup the Minimally Adequate Teacher
    Language language(alphabet);
    MinimallyAdequateTeacher mat(language);

    // Learn the grammar
    Learner learner(mat);
    learner.LearnCFG();

    return 0;
}
