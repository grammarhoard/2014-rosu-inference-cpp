/*
 * Minimally Adequate Teacher (MAT)
 * Provides the learner two sources of information about the language
 *     1. Membership queries  -> Mem()
 *     2. Equivalence queries -> Equiv()
 */
#pragma once

#include "Language.h"
#include "ContextFreeGrammar.h"

class MinimallyAdequateTeacher
{
public:
    MinimallyAdequateTeacher(Language& L);
    ~MinimallyAdequateTeacher();

    Language& getLanguage();

    /*
     * Membership query (Oracle)
     * Returns true if the given string is in the language,
     *     and false otherwise
     */
    //TODO maybe use just one parameter for MinimallyAdequateTeacher::Mem()
    bool Mem(const string l, const string u, const string r);

    /*
     * Equivalence query
     * Returns true if the grammar is correct,
     *     and false otherwise
     * If the grammar is incorrect, a counter-example is available
     *     by calling getCounterExample()
     */
    bool Equiv(const ContextFreeGrammar& G);

    /*
     * After checking if the grammar is correct or not,
     *     this method returns the counter-example if the grammar is incorrect
     */
    string getCounterExample();

private:
    Language& _L;
    string _counterExample;
};
