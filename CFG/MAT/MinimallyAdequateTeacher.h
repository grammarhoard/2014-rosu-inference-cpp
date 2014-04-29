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
    bool Mem(const string l, const string u, const string r);

    /*
     * Equivalence query
     * Implemented in Observation Table
     */
    // bool Equiv(ContextFreeGrammar& G);

private:
    Language& _L;
};
