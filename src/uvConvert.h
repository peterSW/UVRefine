#ifndef UVCONVERT_H_
#define UVCONVERT_H_

inline float index2local(int index, int size)
{
    return index / size;
}

inline float local2index(float local, int size)
{
    return local*size;
}

#endif /* UVCONVERT_H_ */
