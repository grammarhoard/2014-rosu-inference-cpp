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
    void insert(string element);

    /*
     * Get all elements of the alphabet
     */
    //TODO check if you can remove Alphabet::get() method
    // set<string> get();

    /*
     * Returns true if the specified element is in the alphabet,
     *     and false otherwise
     */
    bool in(string element);
private:

    //TODO maybe store the alphabet as a set of chars
    set<string> _data;
};
