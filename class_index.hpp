#ifndef INDEX_HPP
#define INDEX_HPP

#include "utils.hpp"
#include "class_response.hpp"
#include "class_data.hpp"

template<typename KEY_TYPE = default_data_type>
class Index {
    virtual Response add(Data<KEY_TYPE> data, physical_pos raw_pos) = 0;
    virtual Response search(Data<KEY_TYPE> data) = 0;
    virtual Response rangeSearch(Data<KEY_TYPE> begin, Data<KEY_TYPE> end) = 0;
    virtual Response erase(Data<KEY_TYPE> data) = 0;
};

#endif // INDEX_HPP