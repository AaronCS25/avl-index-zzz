#include "utils.h"

class AVLIndexHeader
{
public:
    posType rootPointer = -1;
    posType nroNodos = 0;

    AVLIndexHeader(){}
    AVLIndexHeader(posType _rootPointer, posType _nroNodos)
    {
        this->rootPointer = _rootPointer;
        this->nroNodos = _nroNodos;
    }
};