#include <set>
#include <string>

using namespace std;

#pragma once
class Alphabet
{
public:
    Alphabet();
    ~Alphabet();

    /*
     * Insert an element into the alphabet
     */
    void insert(const string element);

    /*
     * Get all elements of the alphabet
     */
    //TODO maybe check if you can remove Alphabet::get() method (now it is used in Learner::LearnCFG())
    // set<string> get();

    /*
     * Returns true if the specified element is in the alphabet,
     *     and false otherwise
     */
    bool in(const string element);

private:

    //TODO maybe store the alphabet as a set of chars
    set<string> _data;
};
