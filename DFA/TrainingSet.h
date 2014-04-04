/**
 * Set of pairs (tuples) T = {<s1, l1>, ..., <sm, lm>},
 *    where each pair <si, li> is one input string and its label (output)
 * E.g. T = {<1, true>, <11, true>, <0, false>, <101, false>}
 * The first two tuples are positive samples and the last two are negative
 */

#include <utility>    // std::pair, std::make_pair
#include <string>     // std::string
#include <set>        // std::set
#include <stdio.h>    // IO library

using namespace std;

#pragma once
class TrainingSet
{
public:
    // Aliases
    typedef set<pair<string, bool>> T;

    TrainingSet();
    T get();
    void addSample(string sample, bool label);
    void addSampleFromFile(FILE * pFile, bool label);
    //TODO maybe implement addSampleFromDirectory
    ~TrainingSet();
private:
    T _data;
    const static int _maxString;
};

