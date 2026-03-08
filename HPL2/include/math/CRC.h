#ifndef HPL_CRC_H
#define HPL_CRC_H

#include <cstddef>

namespace hpl
{
//----------------------------------------

typedef unsigned int tCRCKey;

//----------------------------------------

class cCRCTable
{
public:
    cCRCTable () : mKey (0) {}

    void Init (tCRCKey key);

    tCRCKey operator [] (unsigned i)
    {
        return mTable [i];
    }

private:
    tCRCKey mTable [256];
    tCRCKey mKey;
};

//----------------------------------------

class cCRC
{
public:
    cCRC (tCRCKey key) : mKey (key), mRegister(0)
    {
        mTable.Init (key);
    }

    void PutByte (unsigned aByte);
    void PutData(char *apData, size_t alSize);

    tCRCKey Done ()
    {
        tCRCKey temp = mRegister;
        mRegister = 0;
        return temp;
    }

private:
    static cCRCTable mTable;
    tCRCKey mKey;
    tCRCKey mRegister;
};

//----------------------------------------
}

#endif // HPL_CRC_H
