#ifndef UVCONVERT_H_
#define UVCONVERT_H_

#include <ImathVec.h>

inline float index2local(int index, int size)
{
    return (float)index / size;
}

inline int local2index(float local, int size)
{
    return local*size;
}

Imath::V2f
NormalToWorldUV(const Imath::V3f &normal)
{
    Imath::V2f uv;
  uv.x = ((atan2(normal.x, normal.z) / (2.0f * M_PI_F)));
  uv.y = acos(-normal.y) / M_PI_F;

  uv.x = fmod(uv.x + 10.0f, 1.0f);

  return uv;
}

Imath::V3f
WorldUVToNormal(const Imath::V2f &envUV)
{

  float hAng(envUV.x * 2.0 * M_PI_F);
  float vAng(envUV.y * M_PI_F);

  Imath::V3f normal;
  normal.x = sin(hAng) * sin(vAng);
  normal.y = -cos(vAng);
  normal.z = cos(hAng) * sin(vAng);

  return normal;
}

#endif /* UVCONVERT_H_ */
