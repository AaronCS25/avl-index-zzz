class AVLIndexHeader
{
public:
    long rootPointer = -1;
    long nroNodos = 0;

    AVLIndexHeader(){}
    AVLIndexHeader(long _rootPointer, long _nroNodos)
    {
        this->rootPointer = _rootPointer;
        this->nroNodos = _nroNodos;
    }
};