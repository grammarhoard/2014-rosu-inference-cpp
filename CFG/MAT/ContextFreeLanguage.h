/*
 * Context Free Language (CFG)
 * ...
 */
//TODO write description for ContextFreeLanguage

#include "Alphabet.h"

#pragma once
class ContextFreeLanguage
{
public:
    static const string lambda; // empty string

    ContextFreeLanguage(Alphabet alphabet);
    ~ContextFreeLanguage();

    Alphabet getAlphabet();

    /*
     * Returns true if the element is in the language,
     *     and false otherwise
     */
    bool in(string element);

private:
    Alphabet _alphabet;
};
