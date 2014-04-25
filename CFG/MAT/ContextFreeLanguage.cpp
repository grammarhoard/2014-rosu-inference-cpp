#include "ContextFreeLanguage.h"

ContextFreeLanguage::ContextFreeLanguage()
{
}

ContextFreeLanguage::~ContextFreeLanguage()
{
}

bool ContextFreeLanguage::in(const string w)
{
    //TODO implement ContextFreeLanguage::in()
    if (w == "" || w == "aab") {
        return true;
    }

    return false;
}
