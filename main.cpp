#include <iostream>
#include "class_avl_index.h"

int main()
{
    AVLIndex index("avlIndex.dat");
    Data item1, item2, item3, item4;
    item1.dato = 30;
    item2.dato = 15;
    item3.dato = 20;
    item4.dato = 150;

    index.insert(item1);
    index.insert(item2);
    index.insert(item3);
    index.insert(item4);
    
    std::cout << "Hello Micho!" << std::endl;
    return 0;
}