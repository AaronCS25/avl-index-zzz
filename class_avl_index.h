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

    void initIndex()
    {
        initFile();
        file.open(this->indexFileName, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) { throw std::runtime_error("No se pudo abrir el archivo AVLIndex!"); }
        file.seekg(0, std::ios::end);
        long bytes = file.tellg();
        if (bytes < sizeof(AVLIndexHeader))
        {
            AVLIndexHeader header;
            file.seekp(0, std::ios::beg);
            file.write((char*) &header, sizeof(AVLIndexHeader));
        }
        file.close();
        return;        
    }

public:
    //* Constructores:
    AVLIndex(std::string _indexFileName)
    {
        this->indexFileName = _indexFileName;
        initIndex();
    }
};