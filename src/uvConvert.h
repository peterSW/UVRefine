#ifndef UVCONVERT_H_
#define UVCONVERT_H_

inline float index2local(int index, int size)
{
    return (float)index / size;
}

inline int local2index(float local, int size)
{
    return local*size;
}

#endif /* UVCONVERT_H_ */
