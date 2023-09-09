#ifndef DATA_HPP
#define DATA_HPP

#include "utils.hpp"

template<typename KEY_TYPE = default_data_type>
struct Data {
    KEY_TYPE key;

    Data(){}

    Data(KEY_TYPE _key) {
        this->key = _key;
    }
    friend std::ostream& operator<<(std::ostream& stream, const Data& data);

    bool operator==(const Data& other) const { return this->key == other.key; }

    bool operator<(const Data& other) const { return this->key < other.key; }
    
    bool operator<=(const Data& other) const { return this->key <= other.key; }

    bool operator>(const Data& other) const { return this->key > other.key; }

    bool operator>=(const Data& other) const { return this->key >= other.key; }
};

template<typename KEY_TYPE>
inline std::ostream& operator<<(std::ostream& stream, const Data<KEY_TYPE>& data)
{
    stream<<" | key: "<<data.key;
    return stream;
}

#endif // DATA_HPP