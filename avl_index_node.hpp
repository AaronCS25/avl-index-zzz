#ifndef AVL_INDEX_NODE_HPP
#define AVL_INDEX_NODE_HPP

#include "index.hpp"
#include "record.hpp"

template<typename KEY_TYPE = default_data_type>
struct AVLIndexNode : public Record<KEY_TYPE>
{
    physical_pos leftChildren = -1;
    physical_pos rightChildren = -1;

    physical_pos nextDelete = -1;

    physical_pos height = 0;

    friend std::ostream& operator<<(std::ostream& os, const AVLIndexNode& node)
    {
        os << "Data: " << node.data << "\n";
        os << "raw_pos:" << node.raw_pos << "\n";
        os << "Left Children: " << node.leftChildren << "\n";
        os << "Right Children: " << node.rightChildren << "\n";
        os << "Next Delete: " << node.nextDelete << "\n";
        os << "Height: " << node.height << "\n\n";
        return os;
    }

};

#endif // AVL_INDEX_NODE_HPP