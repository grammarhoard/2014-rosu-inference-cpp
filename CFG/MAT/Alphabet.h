#pragma once
#include <set>
#include <string>

using namespace std;

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
    set<string>& get();

    /*
     * Returns true if the specified element is in the alphabet,
     *     and false otherwise
     */
    bool in(const string element);

private:
    set<string> _data;
};
