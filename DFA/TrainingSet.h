/**
 * Set of pairs (tuples) T = {<s1, l1>, ..., <sm, lm>},
 *    where each pair <si, li> is one input string and its label (output)
 * E.g. T = {<1, true>, <11, true>, <0, false>, <101, false>}
 * The first two tuples are positive samples and the last two are negative
 */

#include <stdio.h> // reading from files
#include <utility> // pair, make_pair

#define NOMINMAX // Take care of ancient macros
#include <Windows.h> // is directory
#include <errno.h>  // strerror
#include <exception>
#include <string>
#include <set>

using namespace std;

#pragma once
class TrainingSet
{
public:
    typedef set<pair<string, bool>> T;

    TrainingSet();
    T get();
    void addSample(string sample, bool label);
    void addSampleFromFile(const string & fileName, bool label); // fileName can be a directory
    void addSampleFromDirectory(const string & fileName, bool label); // fileName needs to be a directory
    FILE * getFilePointer(const string & fileName);
    bool isDirectory(const string & fileName);
    ~TrainingSet();
private:
    T _data;
    static const int _MAXSTRING;
};
