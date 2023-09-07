#include "utils.h"

struct AVLIndexNode
{
    Data item;
    posType posRecord;

    posType leftChildren = -1;
    posType rightChildren = -1;
    

    int height = 0;
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
        else { return ; }

        updateHeigth(cPointer);
        if (!isBalanced(cPointer))
        {
            std::cout << "va a balancear!" << std::endl;
            balance(cPointer);
        }
        return; 
    }

    //* check
    void balance(posType nodePointer)
    {
        if (nodePointer == -1) { return; }
        std::cout << "inicia el balanceo!" << std::endl;
        AVLIndexNode node;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &node, sizeof(AVLIndexNode));
        std::cout << "Nodo: \n";
        std::cout << "nodo.item: " << node.item.dato << std::endl;
        std::cout << std::endl;

        int balance = balancingFactor(nodePointer);
        std::cout << "balancing Factor: " << balance << std::endl;

        if (balance > 1) // Esta cargado a la izquierda?
        {
            std::cout << "balanceado a la izquierda " << std::endl;
            if (balancingFactor(node.leftChildren) <= -1) { 
                std::cout << "se hará rotación a la izuquierda!\n" ;
                leftRotation(node.leftChildren); }
            std::cout << "se hará rotación a la derecha!\n" ;
            rightRotation(nodePointer);
        }
        else if (balance < -1) // Esta cargado a la derecha?
        {
            std::cout << "balanceado a la derecha " << std::endl;
            if (balancingFactor(node.rightChildren) >= 1) { 
                std::cout << "se hará rotación a la derecha!\n" ;
                rightRotation(node.rightChildren); }
                std::cout << "se hará rotación a la izuquierda!\n" ;
            leftRotation(nodePointer);
        }
        return;
    }

    //* check
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

        file.seekp(childPointer, std::ios::beg);
        file.write((char*) &a, sizeof(AVLIndexNode));

        updateHeigth(childPointer);
        updateHeigth(nodePointer);

        return;
    }

    //* check
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

        file.seekp(childPointer, std::ios::beg);
        file.write((char*) &a, sizeof(AVLIndexNode));

        updateHeigth(childPointer);
        updateHeigth(nodePointer);
        return;
    }

    //* check
    bool isBalanced(posType nodePointer)
    {
        if (nodePointer == -1) { return true; }
        if (std::abs(balancingFactor(nodePointer)) > 1) { return false; }
        return true;
    }

    //* check
    int balancingFactor(posType nodePointer)
    {
        if (nodePointer == -1) { return 0; }
        AVLIndexNode node;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &node, sizeof(AVLIndexNode));
        return height(node.leftChildren) - height(node.rightChildren);
    }

    //* check
    void updateHeigth(posType nodePointer)
    {
        if (nodePointer == -1) { return; }
        AVLIndexNode node;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &node, sizeof(AVLIndexNode));
        posType hLeft = height(node.leftChildren);
        posType hRigth = height(node.rightChildren);
        node.height = 1 + (hRigth > hLeft ? hRigth : hLeft);
        file.seekp(nodePointer, std::ios::beg);
        file.write((char*) &node, sizeof(AVLIndexNode));
        return;
    }
    
    //* check
    long height(posType nodePointer)
    {
        if (nodePointer == -1) { return -1; }
        AVLIndexNode node;
        file.seekg(nodePointer, std::ios::beg);
        file.read((char*) &node, sizeof(AVLIndexNode));
        return node.height;
    }

    AVLIndexNode search(posType currentPointer, AVLIndexNode &cNode, Data &item)
    {
        if (currentPointer == -1) { throw std::runtime_error("No se ha encontrado el elemento!"); }

        file.seekg(currentPointer, std::ios::beg);
        file.read((char*) &cNode, sizeof(AVLIndexNode));
        if (item > cNode.item) { return search(cNode.rightChildren, cNode, item); }
        else if (item < cNode.item) { return search(cNode.leftChildren, cNode, item); }
        else { return cNode; }
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

        insert(header.rootPointer, currentNode, insertNode);

        file.close();
        return true;
    }

    AVLIndexNode search(Data item)
    {
        file.open(this->indexFileName, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) { throw std::runtime_error("No se pudo abrir el archivo AVLIndex!"); }
        
        AVLIndexNode searchNode;

        search(header.rootPointer, searchNode, item);

        file.close();
        return searchNode;
    }
};