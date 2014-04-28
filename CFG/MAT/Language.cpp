#include "Language.h"

const string Language::lambda = "";

Language::Language(Alphabet& alphabet) : _Sigma(alphabet)
{
}

Language::~Language()
{
}

Alphabet& Language::getAlphabet()
{
    return this->_Sigma;
}

bool Language::in(const string element)
{
    if (element == this->lambda) {
        return true;
    }

    //TODO implement Language::in()
    if (element == "ab" || element == "abab" || element == "aabb") {
        // ... and so on -> this is an infinite non-linear context-free language
        return true;
    }

    // Not in the language
    return false;
}
