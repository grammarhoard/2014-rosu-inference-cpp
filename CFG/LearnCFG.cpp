#include "LearnCFG.h"

LearnCFG::LearnCFG()
{
}

LearnCFG::~LearnCFG()
{
}

ObjectiveContextFreeGrammar LearnCFG::run()
{
    Alphabet alphabet;
    alphabet.insert("a");
    alphabet.insert("b");

    ContextFreeLanguage language(alphabet);
    ObjectiveContextFreeGrammar G;

    ObservationTable observationTable(language);
    while (true) {
        if (false) {//TODO implement
            break;
        }

        observationTable.build();
        observationTable.MakeConsistent();
        G = observationTable.MakeGrammar();
        //TODO generates the language {""}

        //TODO Query the current language, and it undergenerates
        observationTable.addCounterExample("ab", true);

        // Rebuild table
        observationTable.build();

        break;
    }

    return G;
}