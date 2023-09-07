#include <iostream>
#include "class_avl_index.h"

int main()
{
    AVLIndex index("avlIndex.dat");
    Data item1, item2, item3, item4, item5;
    item1.dato = 30;
    item2.dato = 15;
    item3.dato = 20;
    item4.dato = 150;
    item5.dato = 50;


    index.insert(item1);
    index.insert(item2);
    index.insert(item3);
    index.insert(item4);
    index.insert(item5);
    
    std::cout << "Hello Micho!" << std::endl;
    std::cout << std::endl;

    AVLIndexNode result;
    result = index.search(item4);

    std::cout << "result:" << std::endl;
    std::cout << "result.value: " << result.item.dato << std::endl;
    std::cout << "result.left: " << result.leftChildren << std::endl;
    std::cout << "result.right: " << result.rightChildren << std::endl;
    std::cout << "result.height: " << result.height << std::endl;
    std::cout << std::endl;

    return 0;
}