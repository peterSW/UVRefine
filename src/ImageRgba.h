#ifndef IMAGERGBA_H_
#define IMAGERGBA_H_

#include "uvConvert.h"

#include <ImfRgbaFile.h>
#include <ImfArray.h>
#include <assert.h>

struct ImageRgba
{
    typedef Imath::Vec2<int> Index2D;
    typedef Imf::Rgba PixelT;

    int width;
    int height;
    Imf::Array2D<PixelT> pixels;

    ImageRgba(std::string filename)
    :
        width(0),
        height(0)
    {
        Imf::RgbaInputFile file (filename.c_str());
        Imath::Box2i dw = file.dataWindow();
        width = dw.max.x - dw.min.x + 1;
        height = dw.max.y - dw.min.y + 1;
        pixels.resizeErase (height, width);
        file.setFrameBuffer (&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
        file.readPixels (dw.min.y, dw.max.y);
    }

    void write (const char fileName[])
    {
        Imf::RgbaOutputFile file (fileName, width, height, Imf::WRITE_RGBA);
        file.setFrameBuffer (pixels[0], 1, width);
        file.writePixels (height);
    }

    PixelT* operator [] (int y)
    {
        return pixels[y];
    }
    const PixelT* operator [] (int y) const
    {
        return pixels[y];
    }

    PixelT &operator [] (Imath::Vec2<int> index)
    {
        return pixels[index.y][index.x];
    }
    const PixelT &operator [] (Imath::Vec2<int> index) const
    {
        return pixels[index.y][index.x];
    }

    Imath::Vec2<int> indexYDec(Imath::Vec2<int> index) const
    {
        assert(index.y >= 0);
        if(index.y)
        {
            --index.y;
            return index;
        }
        else
        {
            index.x = (index.x + width/2) % width;
            return index;
        }
    }
    Imath::Vec2<int> indexYInc(Imath::Vec2<int> index) const
    {
        assert(index.y >= 0);
        if(index.y < (height-1))
        {
            ++index.y;
            return index;
        }
        else
        {
            index.x = (index.x + width/2) % width;
            return index;
        }
    }
    Imath::Vec2<int> indexXInc(Imath::Vec2<int> index) const
    {
        if(index.x < (width-1))
        {
            ++index.x;
            return index;
        }
        else
        {
            index.x = 0;
            return index;
        }
    }
    Imath::Vec2<int> indexXDec(Imath::Vec2<int> index) const
    {
        if(index.x)
        {
            --index.x;
            return index;
        }
        else
        {
            index.x = width - 1;
            return index;
        }
    }

    Index2D convertUV2Index(const Imf::Rgba &curUV) const
    {
        Index2D index(0,0);
        index.x = local2index(curUV.r, width);
        assert(index.x < width);
        assert(index.x >= 0);

        index.y = local2index(curUV.g, height);
        assert(index.y < height);
        assert(index.y >= 0);
        if(index.y < 0)
        {
            index.y = 0;
        }

        return index;
    }
    Imf::Rgba convertIndex2UV(const Index2D &index) const
    {
        Imf::Rgba uv(0,0,0);
        uv.r = index2local(index.x, width);
        uv.g = index2local(index.y, height);

//   I        0        0     1
//         |-----|  |-----|-----|
//   uv    0     1  0    0.5    1

        return uv;
    }
};

#endif /* IMAGERGBA_H_ */
