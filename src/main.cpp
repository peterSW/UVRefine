#include "ReadExr.h"
#include "uvConvert.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <string>

using namespace std;

using namespace boost::program_options;
namespace po = boost::program_options;

struct ImageRgba
{
    typedef Imf::Rgba PixelT;
    ImageRgba(std::string filename)
    :
        width(0),
        height(0)
    {
        readRgba(filename.c_str(),pixels,width,height);
    }

    void write (const char fileName[])
    {
        Imf::RgbaOutputFile file (fileName, width, height, Imf::WRITE_RGBA); // 1
        file.setFrameBuffer (pixels[0], 1, width); // 2
        file.writePixels (height); // 3
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
    int width;
    int height;
    Imf::Array2D<PixelT> pixels;
};

template<typename DiffFunctor>
class BestMatchSearchPixelFunctor
{
    const ImageRgba &m_searchImage;
    const half       m_threshold;
    size_t            m_maxIt;

public:
    typedef Imath::Vec2<int> Index2D;

    BestMatchSearchPixelFunctor(
            const ImageRgba &searchImage,
            const half threshold,
            const size_t maxIt)
    :
        m_searchImage(searchImage),
        m_threshold(threshold),
        m_maxIt(maxIt)
    {}

    void operator() (Imf::Rgba &curUV, const Imf::Rgba &target) const
    {
        Index2D index(convertUV2Index(curUV));

        float curBestScore(DiffFunctor()(target,m_searchImage[index]));
        for(int remainingIt(m_maxIt);
            (remainingIt && curBestScore > m_threshold);
            --remainingIt   )
        {
            Index2D bmI(index);
            testIndex(m_searchImage.indexYDec(index), curBestScore, bmI, target);
            testIndex(m_searchImage.indexYInc(index), curBestScore, bmI, target);
            testIndex(m_searchImage.indexXDec(index), curBestScore, bmI, target);
            testIndex(m_searchImage.indexXInc(index), curBestScore, bmI, target);

            if(bmI == index)
            {
                break;
            }
            else
            {
                index = bmI;
            }
        }
        curUV = convertIndex2UV(index);
    }

    void testIndex(
            Index2D indexUT,
            float &curBestScore,
            Index2D &bmI,
            const Imf::Rgba &target) const
    {
        float scoreUT(DiffFunctor()(target,m_searchImage[indexUT]));
        if(scoreUT < curBestScore)
        {
            bmI = indexUT;
            curBestScore = scoreUT;
        }
    }

    Index2D convertUV2Index(const Imf::Rgba &curUV) const
    {
        Index2D index(0,0);
        index.x = local2index(curUV.r, m_searchImage.width);
        assert(index.x < m_searchImage.width);
        assert(index.x >= 0);

        index.y = local2index(curUV.g, m_searchImage.height);
        assert(index.y < m_searchImage.height);
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
        uv.r = index2local(index.x, m_searchImage.width);
        uv.g = index2local(index.y, m_searchImage.height);

//   I        0        0     1
//         |-----|  |-----|-----|
//   uv    0     1  0    0.5    1

        return uv;
    }


};

inline float sqr(float val)
{
    return val * val;
}

struct RgSumSqrDiff
{
    float operator() (const Imf::Rgba &a, const Imf::Rgba &b) const
    {
        return
                sqr(a.r - b.r) +
                sqr(a.g - b.g) +
                sqr(a.b - b.b);
    }
};

ostream &operator << (ostream &out, const Imf::Rgba &rgba)
{
    out << "(" << rgba.r << ", " << rgba.g << ", " << rgba.b << ", " << rgba.a << ")";
    return out;
}

int main(int argc, char *argv[])
{
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
	            ("help,h", "produce help message")
	            ("PriorUV,u", po::value<string>(), "OpenExr image of the UV values.")
	            ("DiffuseIllum,i", po::value<string>(), "OpenExr image of the diffuse illumination map.")
	            ("OutputUV,o", po::value<string>()->default_value("outUV.exr"), "Output")
                ("ObservedShad,s", po::value<string>(), "OpenExr image of the obserevd shading.");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("PriorUV") && vm.count("DiffuseIllum")  && vm.count("ObservedShad"))
    {
        cout << "Loading images\n";

        ImageRgba curUVImage(vm["PriorUV"].as<string>().c_str());
        ImageRgba diffuseIllum(vm["DiffuseIllum"].as<string>().c_str());
        ImageRgba observedShad(vm["ObservedShad"].as<string>().c_str());

        if(curUVImage.width != observedShad.width ||
            curUVImage.height != observedShad.height     )
        {
            cerr << "ObservedShad must be the same size as PriorUV!\n";
            cerr << "ObservedShad: " << observedShad.width << ", " << observedShad.height << "\n";
            cerr << "curUVImage:   " << curUVImage.width << ", " << curUVImage.height << "\n";
            return 1;
        }

        BestMatchSearchPixelFunctor<RgSumSqrDiff> uvSearchFunctor(
                diffuseIllum, 0.0000, 10000);

        for(int y(0); y < observedShad.height; ++y)
        {
            cout << "y: " << y << endl;
            for(int x(0); x < observedShad.width; ++x)
            {
//                cout << "x: " << x << flush;
//                cout << "  uvStart: " << curUVImage[y][x] << flush;
//                cout << "  shading: " << observedShad[y][x] << flush;
                uvSearchFunctor(curUVImage[y][x], observedShad[y][x]);
//                cout << "  result UV " << curUVImage[y][x] << endl;
            }
        }

        curUVImage.write(vm["OutputUV"].as<string>().c_str());
    }
    else
    {
        cout << "Invalid options!!!\n" << desc  << "\n";
    }

    return 0;
}

