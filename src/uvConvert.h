#ifndef UVCONVERT_H_
#define UVCONVERT_H_

#include "IndexLocalConversion.h"
#include <ImathVec.h>
#include <ImfRgba.h>
#include <boost/math/constants/constants.hpp>
const double pi = boost::math::constants::pi<float>();

inline Imath::V2f
NormalToWorldUV(const Imath::V3f &normal)
{
    Imath::V2f uv;
  uv.x = ((atan2(normal.x, normal.z) / (2.0f * pi)));
  uv.y = acos(-normal.y) / pi;

  uv.x = fmod(uv.x + 10.0f, 1.0f);

  return uv;
}


inline Imath::V3f
WorldUVToNormal(const Imath::V2f &envUV)
{

  float hAng(envUV.x * 2.0 * pi);
  float vAng(envUV.y * pi);

  Imath::V3f normal;
  normal.x = sin(hAng) * sin(vAng);
  normal.y = -cos(vAng);
  normal.z = cos(hAng) * sin(vAng);

  return normal;
}



inline Imath::V3f
WorldUVToNormal(const Imf::Rgba &envUV)
{
    return WorldUVToNormal(Imath::V2f(envUV.r, envUV.g));
}

inline float
angularDist(Imath::V3f a, Imath::V3f b)
{
    return acos(a.dot(b));
}

#endif /* UVCONVERT_H_ */
