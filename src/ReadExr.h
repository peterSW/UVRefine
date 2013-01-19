#ifndef READEXR_H_
#define READEXR_H_

#include <ImfRgbaFile.h>
#include <ImfArray.h>

void readRgba (
        const char fileName[],
        Imf::Array2D<Imf::Rgba> &pixels,
        int &width,
        int &height)
{
    Imf::RgbaInputFile file (fileName);
    Imath::Box2i dw = file.dataWindow();
    width = dw.max.x - dw.min.x + 1;
    height = dw.max.y - dw.min.y + 1;
    pixels.resizeErase (height, width);
    file.setFrameBuffer (&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
    file.readPixels (dw.min.y, dw.max.y);
}



#endif /* READEXR_H_ */
