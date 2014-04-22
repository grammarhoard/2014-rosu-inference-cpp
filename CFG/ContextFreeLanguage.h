#include "Alphabet.h"

#pragma once
class ContextFreeLanguage
{
public:
    static const string lambda; // empty string

    ContextFreeLanguage(Alphabet alphabet);
    ~ContextFreeLanguage();

    /*
     * Check if an element is in this language
     */
    bool in(string element);
    Alphabet getAlphabet();
private:
    Alphabet _alphabet;
};

