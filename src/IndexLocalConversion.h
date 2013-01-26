#ifndef INDEXLOCALCONVERSION_H_
#define INDEXLOCALCONVERSION_H_

namespace LowMapped
{
    inline float index2local(int index, int size)
    {
        return (float)index / size;
    }

    inline int local2index(float local, int size)
    {
        if(local >= 1.0f)
        {
            return size - 1;
        }
        else
        {
            return local*size;
        }
    }
}
// 0    0.2   0.4   0.6   0.8   1.0
// |-----|-----|-----|-----|-----|
//    0     1     2     3     4
//-.5   0.5   1.5   2.5   3.5   4.5

// 0       0.5      1.0
// |-----|-----|-----|
//    0     1     2
//-.5   0.5   1.5   2.5

namespace MidMapped
{
    int local2index(float local, int size);
    float index2local(int index, int size);

}
inline int
MidMapped::local2index(float local, int size)
{
    if(local >= 1.0)
    {
        return size - 1;
    }
    else
    {
        return local*size;
    }
}

inline float
MidMapped::index2local(int index, int size)
{
    return float(0.5 + (float)index) / (float)size;
}


#endif /* INDEXLOCALCONVERSION_H_ */
