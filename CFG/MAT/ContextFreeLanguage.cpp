#include "ContextFreeLanguage.h"

const string ContextFreeLanguage::lambda = "";

ContextFreeLanguage::ContextFreeLanguage(Alphabet alphabet) : _alphabet(alphabet)
{
}

ContextFreeLanguage::~ContextFreeLanguage()
{
}

Alphabet ContextFreeLanguage::getAlphabet()
{
    return this->_alphabet;
}

bool ContextFreeLanguage::in(string element)
{
    if (element == lambda) {
        return true;
    }

    //TODO find a better way to implement ContextFreeLanguage::in()
    if (element == "ab" || element == "abab" || element == "aabb") {
        // ... and so on -> this is an infinite non-linear context-free language
        return true;
    }

    // Not in the language
    return false;
}