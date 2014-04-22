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
private:
    set<string> _data;
};

