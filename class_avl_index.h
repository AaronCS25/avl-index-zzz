#include "utils.h"

struct AVLIndexNode
{
    Data item;
    posType posRecord;

    posType leftChildren = -1;
    posType rightChildren = -1;
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
            if (cNode.rightChildren != -1) { insert(cNode.rightChildren, cNode, iNode); }
            else
            {
                posType insertPointer = sizeof(AVLIndexHeader) + sizeof(AVLIndexNode) * header.nroNodos;

                //iNode.parent = cPointer;
                file.seekp(insertPointer, std::ios::beg);
                file.write((char*) &iNode, sizeof(AVLIndexNode));

                cNode.rightChildren = insertPointer;
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
        
        if (!isBalanced(cPointer))
        {
            balance(cPointer);
        }
        updateHeigth(cPointer);
        return; 
    }

    void balance(posType nodePointer)
    {
        if (nodePointer == -1) { return; }
        
        AVLIndexNode node;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &node, sizeof(AVLIndexNode));

        int balance = balancingFactor(nodePointer);

        if (balance > 1) // Esta cargado a la izquierda?
        {
            if (balancingFactor(node.leftChildren) <= -1) { leftRotation(node.leftChildren); }
            rightRotation(nodePointer);
        }
        else if (balance < -1) // Esta cargado a la derecha?
        {
            if (balancingFactor(node.rightChildren) >= 1) { rightRotation(node.rightChildren); }
            leftRotation(nodePointer);
        }
        return;
    }

    void leftRotation(posType nodePointer)
    {
        AVLIndexNode a, b;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &a, sizeof(AVLIndexNode));

        posType childPointer = a.rightChildren;

        file.seekg(a.rightChildren, std::ios::beg);
        file.read((char*) &b, sizeof(AVLIndexNode));

        a.rightChildren = b.leftChildren;
        b.leftChildren = childPointer;

        file.seekp(nodePointer, std::ios::beg);
        file.write((char*) &b, sizeof(AVLIndexNode));

        file.seekp(childPointer, std::ios::end);
        file.write((char*) &a, sizeof(AVLIndexNode));
        return;
    }

    void rightRotation(posType nodePointer)
    {
        AVLIndexNode a, b;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &a, sizeof(AVLIndexNode));

        posType childPointer = a.leftChildren;

        file.seekg(a.leftChildren, std::ios::beg);
        file.read((char*) &b, sizeof(AVLIndexNode));

        a.leftChildren = b.rightChildren;
        b.rightChildren = childPointer;

        file.seekp(nodePointer, std::ios::beg);
        file.write((char*) &b, sizeof(AVLIndexNode));

        file.seekp(childPointer, std::ios::end);
        file.write((char*) &a, sizeof(AVLIndexNode));
        return;
    }

    bool isBalanced(posType nodePointer)
    {
        if (nodePointer == -1) { return true; }
        if (std::abs(balancingFactor(nodePointer)) > 1) { return false; }
        return true;        
    }

    int balancingFactor(posType nodePointer)
    {
        if (nodePointer == -1) { return 0; }
        AVLIndexNode node;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &node, sizeof(AVLIndexNode));
        return heigth(node.leftChildren) - heigth(node.rightChildren);
    }

    void updateHeigth(posType nodePointer)
    {
        if (nodePointer == -1) { return; }
        AVLIndexNode node;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &node, sizeof(AVLIndexNode));
        posType hLeft = heigth(node.leftChildren);
        posType hRigth = heigth(node.rightChildren);
        node.heigth = 1 + (hRigth > hLeft ? hRigth : hLeft);
        file.seekp(nodePointer, std::ios::beg);
        file.write((char*) &node, sizeof(AVLIndexNode));
        return;
    }

    long heigth(posType nodePointer)
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