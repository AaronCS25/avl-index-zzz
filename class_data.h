struct Data
{
    int dato;
    bool operator>(const Data& otherData){
        return dato > otherData.dato;
    }
    bool operator<(const Data& otherData){
        return dato < otherData.dato;
    }
    bool operator>=(const Data& otherData){
        return dato >= otherData.dato;
    }
    bool operator<=(const Data& otherData){
        return dato <= otherData.dato;
    }
    bool operator==(const Data& otherData){
        return dato == otherData.dato;
    }
};