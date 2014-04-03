#include "TrainingSet.h"

const int TrainingSet::_maxString = 1024;

TrainingSet::TrainingSet()
{
}

set<pair<string, bool>> TrainingSet::get()
{
    return this->_data;
}

void TrainingSet::addSample(string sample, bool label)
{
    this->_data.insert(make_pair(sample, label));
}

void TrainingSet::addSampleFromFile(FILE * pFile, bool label)
{
    char buffer[_maxString];
    while (!feof(pFile)) {
        if (fgets(buffer, _maxString, pFile) == NULL) {
            break;
        }
        this->addSample(buffer, label);
    }
}

TrainingSet::~TrainingSet()
{
}
