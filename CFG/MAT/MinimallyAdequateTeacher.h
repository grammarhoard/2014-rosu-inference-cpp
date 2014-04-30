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
     * Returns true if the given grammar defines a language L(G) equivalent with language L,
     *     and false otherwise
     * If the grammar is not correct, it will also provide a counter-example,
     *     available by calling getCounterExample()
     */
    bool Equiv(ContextFreeGrammar& G);

    /*
     * Returns the counter-example found by last equivalence query
     */
    pair<string, bool> getCounterExample();

private:
    Language& _L;
    pair<string, bool> _counterExample; // pair(w, true if the counter-example is undergeneration)

    /*
     * Returns true if all of the strings in L can be generated with the grammar G,
     *     and false otherwise
     */
    bool _checkUndergeneration(ContextFreeGrammar& G);

    /*
     * Returns true if the grammar G does not generate strings that are not in L,
     *     and false otherwise
     */
    bool _checkOvergeneration(ContextFreeGrammar& G);
};
