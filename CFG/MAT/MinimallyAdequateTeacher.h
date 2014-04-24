/*
 * Minimally Adequate Teacher (MAT)
 * ...
 */
//TODO write description for MinimallyAdequateTeacher

#include "ContextFreeLanguage.h"
#include "ContextFreeGrammar.h"

#pragma once
class MinimallyAdequateTeacher
{
public:
    MinimallyAdequateTeacher(ContextFreeLanguage L);
    ~MinimallyAdequateTeacher();

    ContextFreeLanguage getLanguage();

    /*
     * Membership query (Oracle)
     * Returns true if the given string is in the language,
     *     and false otherwise
     */
    //TODO maybe use just one parameter for MinimallyAdequateTeacher::Mem()
    bool Mem(string l, string u, string r);

    /*
     * Equivalence query
     * Returns true if the grammar is correct,
     *     and false otherwise
     * If the grammar is incorrect, a counter-example is available
     *     by calling getCounterExample()
     */
    bool Equiv(ContextFreeGrammar G);

    /*
     * After checking if the grammar is correct or not,
     *     this method returns the counter-example if the grammar is incorrect
     */
    string getCounterExample();

private:
    ContextFreeLanguage _L;
    string _counterExample;
};
