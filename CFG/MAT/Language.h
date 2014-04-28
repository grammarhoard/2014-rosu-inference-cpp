/*
 * The language that needs to be learned -> L
 */
#pragma once

#include "Alphabet.h"

class Language
{
public:
    static const string lambda; // empty string

    Language(Alphabet& alphabet);
    ~Language();

    Alphabet& getAlphabet();

    /*
     * Returns true if the element is in the language,
     *     and false otherwise
     */
    bool in(string element);

private:
    Alphabet& _Sigma;
};