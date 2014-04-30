#include "Language.h"

const string Language::lambda = "";
const int Language::_MAXSTRING = 1024;
const set<string> Language::_NEWLINES {"\n", "\r"};

Language::Language()
{
    this->_Sigma = Alphabet();
    this->addSample(this->lambda);
}

Language::~Language()
{
}

Alphabet& Language::getAlphabet()
{
    return this->_Sigma;
}

vector<string>& Language::getData()
{
    return this->_data;
}


void Language::addSample(string sample)
{
    // Remove newlines
    for (string newLine: this->_NEWLINES) {
        if (sample.find(newLine) != string::npos) { // Found
            sample.erase(sample.size() - 1);
        }
    }

    // Add characters to alphabet
    for (char c : sample) {
        string s(1, c);
        this->_Sigma.insert(s);
    }

    // Add string to data
    if (find(this->_data.begin(), this->_data.end(), sample) == this->_data.end()) { // Not found
        this->_data.push_back(sample);
    }
}

void Language::addSampleFromFile(const string& fileName)
{
    if (this->isDirectory(fileName)) {
        this->addSampleFromDirectory(fileName);
        return;
    }

    FILE * pFile = this->getFilePointer(fileName);

    char buffer[_MAXSTRING];
    while (!feof(pFile)) {
        if (fgets(buffer, _MAXSTRING, pFile) == NULL) {
            break;
        }
        this->addSample(buffer); // add sample line by line
    }
}

void Language::addSampleFromDirectory(const string & fileName)
{
    const string & fileName2 = fileName + "\\*";
    WIN32_FIND_DATA searchData;
    memset(&searchData, 0, sizeof(WIN32_FIND_DATA));
    HANDLE handle = FindFirstFile(fileName2.c_str(), &searchData);

    while (handle != INVALID_HANDLE_VALUE) {
        const string & currentFile = searchData.cFileName;
        if (currentFile != "." && currentFile != "..") {
            this->addSampleFromFile(fileName + "\\" + currentFile);
        }
        if (FindNextFile(handle, &searchData) == FALSE) {
            break;
        }
    }

    // Close the handle after use or memory/resource leak
    FindClose(handle);
}

FILE * Language::getFilePointer(const string & fileName)
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

bool Language::isDirectory(const string & fileName)
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

bool Language::in(const string element)
{
    for (string s : this->_data) {
        if (s == element) { // Found
            return true;
        }
    }

    // Not in the language
    return false;
}
