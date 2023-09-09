#ifndef AVL_INDEX_NODE_HPP
#define AVL_INDEX_NODE_HPP

#include "class_index.hpp"

template<typename KEY_TYPE = default_data_type>
struct AVLIndexNode
{
    Data<KEY_TYPE> data;
    physical_pos posRecord;

    physical_pos leftChildren = -1;
    physical_pos rightChildren = -1;

    physical_pos nextDelete = -1;

    physical_pos height = 0;

    friend std::ostream& operator<<(std::ostream& os, const AVLIndexNode& node)
    {
        os << "Data: " << node.data.numero << "\n";
        os << "Left Children: " << node.leftChildren << "\n";
        os << "Right Children: " << node.rightChildren << "\n";
        os << "Next Delete: " << node.nextDelete << "\n";
        os << "Height: " << node.height << "\n\n";
        return os;
    }

};

#endif // AVL_INDEX_NODE_HPP