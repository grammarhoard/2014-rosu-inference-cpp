/*
 * The language that needs to be learned -> L
 */
#pragma once

#include <Windows.h> // is directory
#include <string>
#include <vector>
#include <algorithm> // std::find

#include "Alphabet.h"

using namespace std;

class Language
{
public:
    static const string lambda; // empty string

    Language();
    ~Language();

    Alphabet& getAlphabet();
    vector<string>& getData();

    void addSample(string sample);
    void addSampleFromFile(const string& fileName); // fileName can be a directory or a file
    void addSampleFromDirectory(const string & fileName); // fileName needs to be a directory
    FILE * getFilePointer(const string & fileName);
    bool isDirectory(const string & fileName);

    /*
     * Returns true if the element is in the language,
     *     and false otherwise
     */
    bool in(string element);

private:
    static const int _MAXSTRING;
    static const set<string> _NEWLINES;
    Alphabet _Sigma;

    vector<string> _data; // we use a vector because we want to preserve the insertion order
};
