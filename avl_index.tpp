#ifndef AVL_INDEX_CPP
#define AVL_INDEX_CPP

#include "avl_index.hpp"

template <typename KEY_TYPE>
void AVLIndex<KEY_TYPE>::initFile()
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

template <typename KEY_TYPE>
void AVLIndex<KEY_TYPE>::initIndex()
{
    initFile();
    file.open(this->indexFileName, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) { throw std::runtime_error("No se pudo abrir el archivo AVLIndex!"); }
    file.seekg(0, std::ios::end);
    long bytes = file.tellg();
    if (bytes < sizeof(AVLIndexHeader))
    {
        this->header.rootPointer = -1;
        this->header.lastDelete = -1;
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

template <typename KEY_TYPE>
void AVLIndex<KEY_TYPE>::add(physical_pos cPointer, AVLIndexNode<KEY_TYPE> &cNode, AVLIndexNode<KEY_TYPE> &iNode, Response &response)
{
    if (cPointer == -1)
        {
            file.seekp(sizeof(AVLIndexHeader), std::ios::beg);
            file.write((char*) &iNode, sizeof(AVLIndexNode<KEY_TYPE>));
            header.rootPointer = sizeof(AVLIndexHeader);

            file.seekp(0, std::ios::beg);
            file.write((char*) &header, sizeof(AVLIndexHeader));
            return;
        }

        file.seekg(cPointer, std::ios::beg);
        file.read((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));

        if (iNode.data > cNode.data)
        {
            if (cNode.rightChildren != -1) { add(cNode.rightChildren, cNode, iNode, response); }
            else
            {
                file.seekg(0, std::ios::end);
                physical_pos insertPointer = file.tellg();
                if (header.lastDelete != -1)
                {
                    insertPointer = header.lastDelete;
                    AVLIndexNode<KEY_TYPE> tempNode;
                    file.seekg(insertPointer, std::ios::beg);
                    file.read((char*) &tempNode, sizeof(AVLIndexNode<KEY_TYPE>));

                    header.lastDelete = tempNode.nextDelete;
                    file.seekp(0, std::ios::beg);
                    file.write((char*) &header, sizeof(AVLIndexHeader));
                }

                //actualiza el response
                response.records.push_back(insertPointer);

                file.seekp(insertPointer, std::ios::beg);
                file.write((char*) &iNode, sizeof(AVLIndexNode<KEY_TYPE>));

                cNode.rightChildren = insertPointer;
                file.seekp(cPointer, std::ios::beg);
                file.write((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));
            }
        }
        else if (iNode.data < cNode.data)
        {
            if (cNode.leftChildren != -1) { add(cNode.leftChildren, cNode, iNode, response); }
            else
            {
                file.seekg(0, std::ios::end);
                physical_pos insertPointer = file.tellg();
                if (header.lastDelete != -1)
                {
                    insertPointer = header.lastDelete;
                    AVLIndexNode<KEY_TYPE> tempNode;
                    file.seekg(insertPointer, std::ios::beg);
                    file.read((char*) &tempNode, sizeof(AVLIndexNode<KEY_TYPE>));

                    header.lastDelete = tempNode.nextDelete;
                    file.seekp(0, std::ios::beg);
                    file.write((char*) &header, sizeof(AVLIndexHeader));
                }

                //actualiza el response
                response.records.push_back(insertPointer);

                file.seekp(insertPointer, std::ios::beg);
                file.write((char*) &iNode, sizeof(AVLIndexNode<KEY_TYPE>));

                cNode.leftChildren = insertPointer;
                file.seekp(cPointer, std::ios::beg);
                file.write((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));
            }
        }
        else { return ; }

        updateHeigth(cPointer);
        if (!isBalanced(cPointer)) { balance(cPointer); }
        return;
}

template <typename KEY_TYPE>
void AVLIndex<KEY_TYPE>::balance(physical_pos nodePointer)
{
    if (nodePointer == -1) { return; }
    AVLIndexNode<KEY_TYPE> node;
    file.seekg(nodePointer, std::ios::beg);
    file.read((char*) &node, sizeof(AVLIndexNode<KEY_TYPE>));

    int balance = balancingFactor(nodePointer);

    if (balance > 1) // Esta cargado a la izquierda?
    {
        if (balancingFactor(node.leftChildren) <= -1) { 
            leftRotation(node.leftChildren); }
        rightRotation(nodePointer);
    }
    else if (balance < -1) // Esta cargado a la derecha?
    {
        if (balancingFactor(node.rightChildren) >= 1) { 
            rightRotation(node.rightChildren); }
        leftRotation(nodePointer);
    }
    return;
}

template <typename KEY_TYPE>
void AVLIndex<KEY_TYPE>::leftRotation(physical_pos nodePointer)
{
    AVLIndexNode<KEY_TYPE> a, b;
    file.seekg(nodePointer, std::ios::beg);
    file.read((char*) &a, sizeof(AVLIndexNode<KEY_TYPE>));

    physical_pos childPointer = a.rightChildren;

    file.seekg(a.rightChildren, std::ios::beg);
    file.read((char*) &b, sizeof(AVLIndexNode<KEY_TYPE>));

    a.rightChildren = b.leftChildren;
    b.leftChildren = childPointer;

    file.seekp(nodePointer, std::ios::beg);
    file.write((char*) &b, sizeof(AVLIndexNode<KEY_TYPE>));

    file.seekp(childPointer, std::ios::beg);
    file.write((char*) &a, sizeof(AVLIndexNode<KEY_TYPE>));

    updateHeigth(childPointer);
    updateHeigth(nodePointer);

    return;
}

template <typename KEY_TYPE>
void AVLIndex<KEY_TYPE>::rightRotation(physical_pos nodePointer)
{
    AVLIndexNode a, b;
    file.seekg(nodePointer, std::ios::beg);
    file.read((char*) &a, sizeof(AVLIndexNode<KEY_TYPE>));

    physical_pos childPointer = a.leftChildren;

    file.seekg(a.leftChildren, std::ios::beg);
    file.read((char*) &b, sizeof(AVLIndexNode<KEY_TYPE>));

    a.leftChildren = b.rightChildren;
    b.rightChildren = childPointer;

    file.seekp(nodePointer, std::ios::beg);
    file.write((char*) &b, sizeof(AVLIndexNode<KEY_TYPE>));

    file.seekp(childPointer, std::ios::beg);
    file.write((char*) &a, sizeof(AVLIndexNode<KEY_TYPE>));

    updateHeigth(childPointer);
    updateHeigth(nodePointer);
    return;
}

template <typename KEY_TYPE>
bool AVLIndex<KEY_TYPE>::isBalanced(physical_pos nodePointer)
{
    if (nodePointer == -1) { return true; }
    if (std::abs(balancingFactor(nodePointer)) > 1) { return false; }
    return true;
}

template <typename KEY_TYPE>
int AVLIndex<KEY_TYPE>::balancingFactor(physical_pos nodePointer)
{
    if (nodePointer == -1) { return 0; }
    AVLIndexNode<KEY_TYPE> node;
    file.seekg(nodePointer, std::ios::beg);
    file.read((char*) &node, sizeof(AVLIndexNode<KEY_TYPE>));
    return height(node.leftChildren) - height(node.rightChildren);
}

template <typename KEY_TYPE>
void AVLIndex<KEY_TYPE>::updateHeigth(physical_pos nodePointer)
{
    if (nodePointer == -1) { return; }
    AVLIndexNode<KEY_TYPE> node;
    file.seekg(nodePointer, std::ios::beg);
    file.read((char*) &node, sizeof(AVLIndexNode<KEY_TYPE>));
    physical_pos hLeft = height(node.leftChildren);
    physical_pos hRigth = height(node.rightChildren);
    node.height = 1 + (hRigth > hLeft ? hRigth : hLeft);
    file.seekp(nodePointer, std::ios::beg);
    file.write((char*) &node, sizeof(AVLIndexNode<KEY_TYPE>));
    return;
}

template <typename KEY_TYPE>
long AVLIndex<KEY_TYPE>::height(physical_pos nodePointer)
{
    if (nodePointer == -1) { return -1; }
    AVLIndexNode<KEY_TYPE> node;
    file.seekg(nodePointer, std::ios::beg);
    file.read((char*) &node, sizeof(AVLIndexNode<KEY_TYPE>));
    return node.height;
}

template <typename KEY_TYPE>
AVLIndexNode<KEY_TYPE> AVLIndex<KEY_TYPE>::search(physical_pos currentPointer, AVLIndexNode<KEY_TYPE> &cNode, Data<KEY_TYPE> &item)
{
    if (currentPointer == -1) { throw std::runtime_error("No se ha encontrado el elemento!"); }

    file.seekg(currentPointer, std::ios::beg);
    file.read((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));
    if (item > cNode.data) { return search(cNode.rightChildren, cNode, item); }
    else if (item < cNode.data) { return search(cNode.leftChildren, cNode, item); }
    else { return cNode; }
}

template <typename KEY_TYPE>
physical_pos AVLIndex<KEY_TYPE>::maxNode(physical_pos nodePointer)
{
    if (nodePointer == -1) { throw std::runtime_error("El arbol está vacío!"); }

    AVLIndexNode<KEY_TYPE> node;
    file.seekg(nodePointer, std::ios::beg);
    file.read((char*) &node, sizeof(AVLIndexNode<KEY_TYPE>));

    if (node.rightChildren == -1) { return nodePointer; }
    else { return maxNode(node.rightChildren); }
}

template <typename KEY_TYPE>
bool AVLIndex<KEY_TYPE>::removeIndex(physical_pos cPointer, physical_pos pPointer, AVLIndexNode<KEY_TYPE> &cNode, Data<KEY_TYPE> item)
{
    if (cPointer == -1) { return false; }

    file.seekg(cPointer, std::ios::beg);
    file.read((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>)); 

    if (item > cNode.data)
    {
        pPointer = cPointer;
        if (!erase(cNode.rightChildren, pPointer, cNode, item)) { return false; }
    }
    else if (item < cNode.data)
    {
        pPointer = cPointer;
        if (!erase(cNode.leftChildren, pPointer, cNode, item)) { return false; }
    }
    else
    {
        if (cNode.leftChildren == -1 && cNode.rightChildren == -1)
        {
            if (pPointer != -1)
            {
                AVLIndexNode<KEY_TYPE> pNode;
                file.seekg(pPointer, std::ios::beg);
                file.read((char*) &pNode, sizeof(AVLIndexNode<KEY_TYPE>));

                if (pNode.leftChildren == cPointer) { pNode.leftChildren = -1; }
                else if (pNode.rightChildren == cPointer) { pNode.rightChildren = -1; }

                file.seekp(pPointer, std::ios::beg);
                file.write((char*) &pNode, sizeof(AVLIndexNode<KEY_TYPE>));
            }

            cNode.nextDelete = header.lastDelete;
            file.seekp(cPointer, std::ios::beg);
            file.write((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));
            
            header.lastDelete = cPointer;
            file.seekp(0, std::ios::beg);
            file.write((char*) &header, sizeof(AVLIndexHeader));
        }
        else if (cNode.leftChildren == -1)
        {
            if (pPointer != -1)
            {
                AVLIndexNode<KEY_TYPE> pNode;
                file.seekg(pPointer, std::ios::beg);
                file.read((char*) &pNode, sizeof(AVLIndexNode<KEY_TYPE>));

                if (pNode.leftChildren == cPointer) { pNode.leftChildren = cNode.rightChildren; }
                else if (pNode.rightChildren == cPointer) { pNode.rightChildren = cNode.rightChildren; }

                file.seekp(pPointer, std::ios::beg);
                file.write((char*) &pNode, sizeof(AVLIndexNode<KEY_TYPE>));
            }

            cNode.nextDelete = header.lastDelete;
            file.seekp(cPointer, std::ios::beg);
            file.write((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));

            header.lastDelete = cPointer;
            file.seekp(0, std::ios::beg);
            file.write((char*) &header, sizeof(AVLIndexHeader));
        }
        else if (cNode.rightChildren == -1)
        {
            if (pPointer != -1)
            {
                AVLIndexNode<KEY_TYPE> pNode;
                file.seekg(pPointer, std::ios::beg);
                file.read((char*) &pNode, sizeof(AVLIndexNode<KEY_TYPE>));

                if (pNode.leftChildren == cPointer) { pNode.leftChildren = cNode.leftChildren; }
                else if (pNode.rightChildren == cPointer) { pNode.rightChildren = cNode.leftChildren; }

                file.seekp(pPointer, std::ios::beg);
                file.write((char*) &pNode, sizeof(AVLIndexNode<KEY_TYPE>));
            }

            cNode.nextDelete = header.lastDelete;
            file.seekp(cPointer, std::ios::beg);
            file.write((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));

            header.lastDelete = cPointer;
            file.seekp(0, std::ios::beg);
            file.write((char*) &header, sizeof(AVLIndexHeader));
        }
        else
        {
            physical_pos newPos = maxNode(cNode.leftChildren);
            AVLIndexNode<KEY_TYPE> tempNode;

            file.seekg(newPos, std::ios::beg);
            file.read((char*) &tempNode, sizeof(AVLIndexNode<KEY_TYPE>));

            Data<KEY_TYPE> tempItem = tempNode.data;
            erase(header.rootPointer, -1, tempNode, tempItem);
            fixValue(cPointer, tempNode, cNode.data, tempItem);
        }
    }

    updateHeigth(cPointer);
    if (!isBalanced(cPointer)) {  balance(cPointer); }
    return true;
}

template <typename KEY_TYPE>
void AVLIndex<KEY_TYPE>::fixValue(physical_pos cPointer, AVLIndexNode<KEY_TYPE> &cNode, Data<KEY_TYPE> &item1, Data<KEY_TYPE> &item2)
{
    if (cPointer == -1) { return; }
    
    file.seekg(cPointer, std::ios::beg);
    file.read((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));

    if (cNode.data > item1) { return fixValue(cNode.leftChildren, cNode, item1, item2); }
    else if (cNode.data < item1) { return fixValue(cNode.rightChildren, cNode, item1, item2); }
    else
    {
        cNode.data = item2;
        file.seekp(cPointer, std::ios::beg);
        file.write((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));
    }
    return;
}

template <typename KEY_TYPE>
void AVLIndex<KEY_TYPE>::displayPretty(const std::string &prefix, physical_pos cPointer, bool isLeft)
{
    if (cPointer == -1) { return; }

    AVLIndexNode<KEY_TYPE> cNode;

    file.seekg(cPointer, std::ios::beg);
    file.read((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));

    std::cout << prefix;
    std::cout << (isLeft ? "|--" : "|__");
    std::cout << cNode.data.numero << "(" << cNode.height << ")" << std::endl;

    displayPretty(prefix + (isLeft ? "|   " : "    "), cNode.leftChildren, true);
    displayPretty(prefix + (isLeft ? "|   " : "    "), cNode.rightChildren, false);
    return;
}

template <typename KEY_TYPE>
void AVLIndex<KEY_TYPE>::searchIndexsByRange(physical_pos cPointer, AVLIndexNode<KEY_TYPE> &cNode, std::vector<AVLIndexNode<KEY_TYPE>> &cVector, Data<KEY_TYPE> &begin, Data<KEY_TYPE> &end)
{
    if (cPointer == -1) { return; }
    
    file.seekg(cPointer, std::ios::beg);
    file.read((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));

    if (cNode.data > begin)
    {
        rangeSearch(cNode.leftChildren, cNode, cVector, begin, end);
        file.seekg(cPointer, std::ios::beg);
        file.read((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));
    }
    if (cNode.data >= begin && cNode.data <= end) { cVector.push_back(cNode); }
    if (cNode.data < end)
    {
        rangeSearch(cNode.rightChildren, cNode, cVector, begin, end);
        file.seekg(cPointer, std::ios::beg);
        file.read((char*) &cNode, sizeof(AVLIndexNode<KEY_TYPE>));
    }

    return;
}

template <typename KEY_TYPE>
Response AVLIndex<KEY_TYPE>::add(Data<KEY_TYPE> item)
{
    Response response;
    file.open(this->indexFileName, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) { throw std::runtime_error("No se pudo abrir el archivo AVLIndex!"); }

    response.start_time();

    try
    {
        AVLIndexNode<KEY_TYPE> insertNode;
        insertNode.data = item;
        AVLIndexNode<KEY_TYPE> currentNode;

        add(header.rootPointer, currentNode, insertNode, response);
    }
    catch(std::runtime_error)
    {
        response.stopTimer();
        throw std::runtime_error("Couldn't search");
    }

    response.stopTimer();
    file.close();
    return true;
}

template <typename KEY_TYPE>
Response AVLIndex<KEY_TYPE>::search(Data<KEY_TYPE> item)
{
    file.open(this->indexFileName, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) { throw std::runtime_error("No se pudo abrir el archivo AVLIndex!"); }
    
    AVLIndexNode<KEY_TYPE> searchNode;

    search(header.rootPointer, searchNode, item);

    file.close();
    return searchNode;
}

template <typename KEY_TYPE>
Response AVLIndex<KEY_TYPE>::erase(Data<KEY_TYPE> item)
{
    file.open(this->indexFileName, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) { throw std::runtime_error("No se pudo abrir el archivo AVLIndex!"); }

    AVLIndexNode currentNode;

    bool isRemoved = erase(header.rootPointer, -1, currentNode, item);

    file.close();
    return isRemoved;
}

template <typename KEY_TYPE>
Response AVLIndex<KEY_TYPE>::rangeSearch(Data<KEY_TYPE> start, Data<KEY_TYPE> end)
{
    file.open(this->indexFileName, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) { throw std::runtime_error("No se pudo abrir el archivo AVLIndex!"); }

    AVLIndexNode node;
    std::vector<AVLIndexNode> vector;

    rangeSearch(header.rootPointer, node, vector, start, end);

    file.close();
    return vector;
}

template <typename KEY_TYPE>
void AVLIndex<KEY_TYPE>::displayPretty()
{
    file.open(this->indexFileName, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) { throw std::runtime_error("No se pudo abrir el archivo AVLIndex!"); }

    AVLIndexNode<KEY_TYPE> node;
    file.seekg(header.rootPointer, std::ios::beg);
    file.read((char*) &node, sizeof(AVLIndexNode<KEY_TYPE>));
    displayPretty("", header.rootPointer, true);
    std::cout << std::endl;

    file.close();
    return;
}

#endif // AVL_INDEX_CPP