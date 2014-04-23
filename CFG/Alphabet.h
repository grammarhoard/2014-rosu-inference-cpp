#include <set>
#include <string>

using namespace std;

#pragma once
class Alphabet
{
public:
    Alphabet();
    ~Alphabet();

    void insert(string element);
    set<string> get();
    bool in(string element);
private:
    set<string> _data; //TODO maybe use chars here
};

