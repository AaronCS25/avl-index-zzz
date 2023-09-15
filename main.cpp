#include <iostream>
#include "avl_index.hpp"

int main()
{
    AVLIndex<int> avlIndex("test.dat");
    Data<int> item1, item2, item3, item4, item5, item6, item7, item8, item9, item10;
    item1.key = 1;
    item2.key = 2;
    item3.key = 3;
    item4.key = 4;
    item5.key = 5;
    item6.key = 6;
    item7.key = 7;
    item8.key = 8;
    item9.key = 9;


    index.insert(item1);
    index.insert(item2);
    index.insert(item3);
    index.insert(item4);
    index.insert(item5);
    index.insert(item6);
    index.insert(item7);
    index.insert(item8);
    index.insert(item9);

    index.displayPretty();

    std::vector<AVLIndexNode<int>> vector;

    vector = index.searchIndexsByRange(item3, item8);

    for (int i = 0; i < vector.size(); i++)
    {
        std::cout << vector[i];
    }
    

    // index.removeIndex(item1);
    // index.removeIndex(item3);
    // index.removeIndex(item6);
    // index.removeIndex(item4);

    // index.displayPretty();

    return 0;
}