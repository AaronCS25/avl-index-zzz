#include "utils.h"

struct AVLIndexNode
{
    Data item;
    posType posRecord;

    posType leftChildren = -1;
    posType rigthChildren = -1;

    int heigth = 0;
};

class AVLIndex
{
    std::string indexFileName;
    std::fstream file;

    void initFile()
    {
        file.open(this->indexFileName, std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            file.open(this->indexFileName, std::ios::out | std::ios::binary);
            if (!file.good()) { throw std::runtime_error("No se pudo crear el AVLIndexFile!"); }
            file.close();
        }
        else { file.close(); }
        return;
    }

public:
    //* Constructors:
    AVLIndex(std::string _indexFileName)
    {
        this->indexFileName = _indexFileName;
        initFile();
    }
};