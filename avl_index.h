#ifndef AVL_INDEX_H
#define AVL_INDEX_H

#include "avl_index_header.hpp"
#include "avl_index_node.hpp"

template<typename KEY_TYPE = default_data_type>
class AVLIndex : public Index<KEY_TYPE>
{
    AVLIndexHeader header;
    std::string indexFileName;
    std::fstream file;

    void initFile();

    void initIndex();

    void insert(posType cPointer, AVLIndexNode<KEY_TYPE> &cNode, AVLIndexNode<KEY_TYPE> &iNode);

    void balance(posType nodePointer);

    void leftRotation(posType nodePointer);

    void rightRotation(posType nodePointer);

    bool isBalanced(posType nodePointer);

    int balancingFactor(posType nodePointer);

    void updateHeigth(posType nodePointer);
    
    long height(posType nodePointer);

    AVLIndexNode<KEY_TYPE> search(posType currentPointer, AVLIndexNode<KEY_TYPE> &cNode, Data<KEY_TYPE> &item);

    posType maxNode(posType nodePointer);

    bool removeIndex(posType cPointer, posType pPointer, AVLIndexNode<KEY_TYPE> &cNode, Data<KEY_TYPE> item);

    void fixValue(posType cPointer, AVLIndexNode<KEY_TYPE> &cNode, Data<KEY_TYPE> &item1, Data<KEY_TYPE> &item2);

    void displayPretty(const std::string &prefix, posType cPointer, bool isLeft);

    void searchIndexsByRange(posType cPointer, AVLIndexNode<KEY_TYPE> &cNode, std::vector<AVLIndexNode<KEY_TYPE>> &cVector, Data<KEY_TYPE> &begin, Data<KEY_TYPE> &end);

public:
    AVLIndex(std::string _indexFileName)
    {
        this->indexFileName = _indexFileName;
        initIndex();
    }

    bool insert(Data<KEY_TYPE> item);

    AVLIndexNode<KEY_TYPE> search(Data<KEY_TYPE> item);

    bool removeIndex(Data<KEY_TYPE> item);

    std::vector<AVLIndexNode<KEY_TYPE>> searchIndexsByRange(Data<KEY_TYPE> start, Data<KEY_TYPE> end);

    void displayPretty();
};

#endif // AVL_INDEX_H