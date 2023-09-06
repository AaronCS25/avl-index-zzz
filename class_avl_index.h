#include "utils.h"

struct AVLIndexNode
{
    Data item;
    posType posRecord;

    posType leftChildren = -1;
    posType rigthChildren = -1;
    //posType parent = -1;

    int heigth = 0;
};

class AVLIndex
{
    AVLIndexHeader header;
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
            this->header.rootPointer = -1;
            this->header.nroNodos = 0;
            file.seekp(0, std::ios::beg);
            file.write((char*) &header, sizeof(AVLIndexHeader));
        }
        else
        {
            file.seekg(0, std::ios::beg);
            file.read((char*) &this->header, sizeof(AVLIndexHeader));
        }
        
        file.close();
        return;
    }

    void insert(posType cPointer, AVLIndexNode &cNode, AVLIndexNode &iNode)
    {
        // * Empieza a insertar el nodo.
        if (cPointer == -1)
        {
            file.seekp(sizeof(AVLIndexHeader), std::ios::beg);
            file.write((char*) &iNode, sizeof(AVLIndexNode));
            header.rootPointer = sizeof(AVLIndexHeader);
            header.nroNodos = header.nroNodos + 1;

            

            file.seekp(0, std::ios::beg);
            file.write((char*) &header, sizeof(AVLIndexHeader));
            return;
        }

        file.seekg(cPointer, std::ios::beg);
        file.read((char*) &cNode, sizeof(AVLIndexNode));

        if (iNode.item > cNode.item)
        {
            if (cNode.rigthChildren =! -1) { insert(cNode.rigthChildren, cNode, iNode); }
            else
            {
                posType insertPointer = sizeof(AVLIndexHeader) + sizeof(AVLIndexNode) * header.nroNodos;

                //iNode.parent = cPointer;
                file.seekp(insertPointer, std::ios::beg);
                file.write((char*) &iNode, sizeof(AVLIndexNode));

                cNode.rigthChildren = insertPointer;
                file.seekp(cPointer, std::ios::beg);
                file.write((char*) &cNode, sizeof(AVLIndexNode));

                header.nroNodos++;
                file.seekp(0, std::ios::beg);
                file.write((char*) &header, sizeof(AVLIndexHeader));
            }
        }
        else if (iNode.item < cNode.item)
        {
            if (cNode.leftChildren != -1) { insert(cNode.leftChildren, cNode, iNode); }
            else
            {
                posType insertPointer = sizeof(AVLIndexHeader) + sizeof(AVLIndexNode) * header.nroNodos;

                //iNode.parent = cPointer;
                file.seekp(insertPointer, std::ios::beg);
                file.write((char*) &iNode, sizeof(AVLIndexNode));

                cNode.leftChildren = insertPointer;
                file.seekp(cPointer, std::ios::beg);
                file.write((char*) &cNode, sizeof(AVLIndexNode));

                header.nroNodos++;
                file.seekp(0, std::ios::beg);
                file.write((char*) &header, sizeof(AVLIndexHeader));
            }
        }
        else
        { return ; }
        // * Ya se inserto el nodo.

        // * Verifico si está balanceado:
        std::cout << "Balanceo?" << std::endl;
        if (!isBalanced(cPointer))
        {
            // * Balanceo
            std::cout << "Necesita balancear!" << std::endl;
        }
        // * No balanceo.
        updateHeigth(cPointer);
        return; 
    }

    void leftRotation(posType nodePointer)
    {
        AVLIndexNode a, b;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &a, sizeof(AVLIndexNode));

        posType childPointer = a.rigthChildren;

        file.seekg(a.rigthChildren, std::ios::beg);
        file.read((char*) &b, sizeof(AVLIndexNode));

        a.rigthChildren = b.leftChildren;
        b.leftChildren = childPointer;
        //b.parent = a.parent;
        //a.parent = childPointer;

        file.seekp(nodePointer, std::ios::beg);
        file.write((char*) &b, sizeof(AVLIndexNode));

        file.seekp(childPointer, std::ios::end);
        file.write((char*) &a, sizeof(AVLIndexNode));
        return;
    }

    bool isBalanced(posType nodePointer)
    {
        if (nodePointer == -1) { return true; }
        AVLIndexNode node;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &node, sizeof(AVLIndexNode));
        posType hLeft = heigth(node.leftChildren);
        posType hRigth = heigth(node.rigthChildren);
        std::cout << "hLeft: " << hLeft << std::endl;
        std::cout << "hRigth: " << hRigth << std::endl;
        std::cout << std::endl;
        if (std::abs(hRigth - hLeft) > 1) { return false; }
        return true;        
    }

    void updateHeigth(posType nodePointer)
    {
        if (nodePointer == -1) { return; }
        AVLIndexNode node;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &node, sizeof(AVLIndexNode));
        posType hLeft = heigth(node.leftChildren);
        posType hRigth = heigth(node.rigthChildren);
        node.heigth = 1 + (hRigth > hLeft ? hRigth : hLeft);
        file.seekp(nodePointer, std::ios::beg);
        file.write((char*) &node, sizeof(AVLIndexNode));
        return;
    }

    posType heigth(posType nodePointer)
    {
        if (nodePointer == -1) { return -1; }
        AVLIndexNode node;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &node, sizeof(AVLIndexNode));
        return node.heigth;
    }

public:
    //* Constructores:
    AVLIndex(std::string _indexFileName)
    {
        this->indexFileName = _indexFileName;
        initIndex();
    }

    bool insert(Data item)
    {
        file.open(this->indexFileName, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) { throw std::runtime_error("No se pudo abrir el archivo AVLIndex!"); }

        AVLIndexNode insertNode;
        insertNode.item = item;

        AVLIndexNode currentNode;

        std::cout << "header:" << std::endl;
        std::cout << "header-Root Pointer: " << this->header.rootPointer << std::endl;
        std::cout << "header-nroNodos: " << this->header.nroNodos << std::endl;
        std::cout << std::endl;

        insert(header.rootPointer, currentNode, insertNode);

        file.close();
        return true;
    }
};