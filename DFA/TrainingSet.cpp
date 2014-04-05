#include "TrainingSet.h"

const int TrainingSet::_MAXSTRING = 1024;

TrainingSet::TrainingSet()
{
}

TrainingSet::T TrainingSet::get()
{
    return this->_data;
}

void TrainingSet::addSample(string sample, bool label)
{
    this->_data.insert(make_pair(sample, label));
}

void TrainingSet::addSampleFromFile(FILE * pFile, bool label)
{
    char buffer[_MAXSTRING];
    while (!feof(pFile)) {
        if (fgets(buffer, _MAXSTRING, pFile) == NULL) {
            break;
        }
        this->addSample(buffer, label);
    }
}

TrainingSet::~TrainingSet()
{
}
