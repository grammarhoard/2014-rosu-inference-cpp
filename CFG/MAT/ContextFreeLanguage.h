/*
 * Context Free Language (CFG)
 * The language defined by a CFG G -> L(G)
 */

#include <string>

using namespace std;

#pragma once
class ContextFreeLanguage
{
public:
    ContextFreeLanguage();
    ~ContextFreeLanguage();

    /*
     * Returns true if the element is in the language,
     *     and false otherwise
     */
    bool in(string element);
};
