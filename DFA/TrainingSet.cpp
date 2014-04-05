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
    this->_data.insert({ sample, label });
}

void TrainingSet::addSampleFromFile(const string & fileName, bool label)
{
    if (this->isDirectory(fileName)) {
        this->addSampleFromDirectory(fileName, label);
        return;
    }

    FILE * pFile = this->getFilePointer(fileName);

    char buffer[_MAXSTRING];
    while (!feof(pFile)) {
        if (fgets(buffer, _MAXSTRING, pFile) == NULL) {
            break;
        }
        this->addSample(buffer, label); // add sample line by line
    }
}

void TrainingSet::addSampleFromDirectory(const string & fileName, bool label)
{
    const string & fileName2 = fileName + "\\*";
    WIN32_FIND_DATA searchData;
    memset(&searchData, 0, sizeof(WIN32_FIND_DATA));
    HANDLE handle = FindFirstFile(fileName2.c_str(), &searchData);

    while (handle != INVALID_HANDLE_VALUE) {
        const string & currentFile = searchData.cFileName;
        if (currentFile != "." && currentFile != "..") {
            this->addSampleFromFile(fileName + "\\" + currentFile, label);
        }
        if (FindNextFile(handle, &searchData) == FALSE) {
            break;
        }
    }

    // Close the handle after use or memory/resource leak
    FindClose(handle);
}

FILE * TrainingSet::getFilePointer(const string & fileName)
{
    FILE * pFile;
    errno_t errorNo;
    errorNo = fopen_s(&pFile, fileName.c_str(), "r");
    if (errorNo == 0) {
        return pFile;
    }

    char buffer[_MAXSTRING];
    strerror_s(buffer, _MAXSTRING, errorNo);

    char message[_MAXSTRING];
    sprintf_s(message, _MAXSTRING, "Error opening file '%s': %s\n", fileName.c_str(), buffer);
    throw exception(message);
}

bool TrainingSet::isDirectory(const string & fileName)
{
    DWORD fileDword = GetFileAttributesA(fileName.c_str());
    if (fileDword == INVALID_FILE_ATTRIBUTES) {
        char message[_MAXSTRING];
        sprintf_s(message, _MAXSTRING, "Invalid file path: %s\n", fileName.c_str());
        throw exception(message);
    }

    if (fileDword & FILE_ATTRIBUTE_DIRECTORY) {
        return true; // this is a directory!
    }

    return false; // this is not a directory!
}

TrainingSet::~TrainingSet()
{
}
