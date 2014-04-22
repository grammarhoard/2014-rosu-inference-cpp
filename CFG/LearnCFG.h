#include "Alphabet.h"
#include "ContextFreeLanguage.h"
#include "ObservationTable.h"
#include "ObjectiveContextFreeGrammar.h"

#pragma once
class LearnCFG
{
public:
    LearnCFG();
    ~LearnCFG();

    /*
     * Algorithm 5. LearnCFG
     */
    ObjectiveContextFreeGrammar run();
};

