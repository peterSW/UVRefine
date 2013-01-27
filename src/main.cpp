#include "ImageRgba.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <string>

using namespace std;

using namespace boost::program_options;
namespace po = boost::program_options;


typedef Imath::Vec2<int> Index2D;


template<typename DiffFunctor>
void testIndex(
        Index2D indexUT,
        float &curBestScore,
        Index2D &bmI,
        const Imf::Rgba &target,
        const ImageRgba &searchImage)
{
    float scoreUT(DiffFunctor()(target, searchImage[indexUT]));
    if(scoreUT < curBestScore)
    {
        bmI = indexUT;
        curBestScore = scoreUT;
    }
}

template<typename SearchFunctor>
class BestMatchSearchPixelFunctor
{
    const ImageRgba &m_searchImage;
    SearchFunctor   &m_searchFunctor;

public:
    BestMatchSearchPixelFunctor(
            const ImageRgba &searchImage,
            SearchFunctor searchFunctor = SearchFunctor())
    :
        m_searchImage(searchImage),
        m_searchFunctor(searchFunctor)
    {}

    void operator() (Imf::Rgba &curUV, const Imf::Rgba &target) const
    {
        Index2D index(m_searchImage.convertUV2Index(curUV));

        m_searchFunctor.search(index, target, m_searchImage);

        curUV = m_searchImage.convertIndex2UV(index);
    }
};

template<typename DiffFunctor>
struct FullSearch
{
   void search(Index2D &index,  const Imf::Rgba &target, const ImageRgba &searchImage) const
   {
       float curBestScore(DiffFunctor()(target,searchImage[index]));
       for(int y(0); y < searchImage.height; ++y)
       {
           for(int x(0); x < searchImage.width; ++x)
           {
               testIndex<DiffFunctor>(Index2D(x,y), curBestScore, index, target, searchImage);
           }
       }
   }
};

template<typename DiffFunctor>
struct NormDotWeightedFullSearch
{

    float m_normWeight;

    NormDotWeightedFullSearch(float normalWeight)
        :
        m_normWeight(normalWeight)
    {}

   void search(Index2D &index,  const Imf::Rgba &target, const ImageRgba &searchImage) const
   {
       Imath::V3f priorNormal(WorldUVToNormal(searchImage.convertIndex2UV(index)));

       float curBestScore(DiffFunctor()(target,searchImage[index]));
       for(int y(0); y < searchImage.height; ++y)
       {
           for(int x(0); x < searchImage.width; ++x)
           {
               Index2D indexUT(x,y);

//               if(normDot > 0)
               {
                   float shadDiff(DiffFunctor()(target, searchImage[indexUT]));
                   float normAngle(angularDist(priorNormal, WorldUVToNormal(searchImage.convertIndex2UV(indexUT))));
//                   cout << "D: " << shadDiff << "  A: " << normAngle << endl;

                   float scoreUT(shadDiff + normAngle/100.0);
                   if(scoreUT < curBestScore)
                   {
                       index = indexUT;
                       curBestScore = scoreUT;
                   }
               }
           }
       }
   }

};


template<typename DiffFunctor>
class FourNeighbourSearch
{
    const int m_maxIt;

public:
    FourNeighbourSearch(int maxIt)
        :
        m_maxIt(maxIt)
    {}

    void search(Index2D &index,  const Imf::Rgba &target, const ImageRgba &searchImage) const
    {
        float curBestScore(DiffFunctor()(target, searchImage[index]));
        for(int remainingIt(m_maxIt); remainingIt; --remainingIt)
        {
            Index2D bmI(index);
            testIndex<DiffFunctor>(searchImage.indexYDec(index), curBestScore, bmI, target, searchImage);
            testIndex<DiffFunctor>(searchImage.indexYInc(index), curBestScore, bmI, target, searchImage);
            testIndex<DiffFunctor>(searchImage.indexXDec(index), curBestScore, bmI, target, searchImage);
            testIndex<DiffFunctor>(searchImage.indexXInc(index), curBestScore, bmI, target, searchImage);

            if(bmI == index)
            {
                break;
            }
            else
            {
                index = bmI;
            }
        }
    }
};


inline float sqr(float val)
{
    return val * val;
}

struct RgbSumSqrDiff
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

template<typename UVSearchFunctorT>
void refineAllUVs(
        ImageRgba        &curUVImage,
        const ImageRgba  &observedShad,
        UVSearchFunctorT &uvSearchFunctor)
{
    for(int y(0); y < observedShad.height; ++y)
    {
        cout << "y: " << y << endl;
        for(int x(0); x < observedShad.width; ++x)
        {
            uvSearchFunctor(curUVImage[y][x], observedShad[y][x]);
        }
    }
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
                ("ObservedShad,s", po::value<string>(), "OpenExr image of the observed shading.")
                ("ResultShad", po::value<string>(), "Output the resultant shading.")
                ("FS", "Use full search.")
                ("NWFS", "Use normal dot proir full search.")
                ("normWeight", po::value<float>()->default_value(0.001), "The weighting of the normal.")
                ("N4S", "Use iterative nearest 4 search.");


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

        if(vm.count("FS"))
        {
            BestMatchSearchPixelFunctor<FullSearch<RgbSumSqrDiff> > uvSearchFunctor(
                    diffuseIllum);
            refineAllUVs(curUVImage, observedShad, uvSearchFunctor);
        }
        else if(vm.count("NWFS"))
        {
            BestMatchSearchPixelFunctor<NormDotWeightedFullSearch<RgbSumSqrDiff> > uvSearchFunctor(
                    diffuseIllum,
                    NormDotWeightedFullSearch<RgbSumSqrDiff>(vm["normWeight"].as<float>()));
            refineAllUVs(curUVImage, observedShad, uvSearchFunctor);
        }
        else if(vm.count("N4S"))
        {
            BestMatchSearchPixelFunctor<FourNeighbourSearch<RgbSumSqrDiff> > uvSearchFunctor(
                    diffuseIllum,
                    FourNeighbourSearch<RgbSumSqrDiff>(1000));
            refineAllUVs(curUVImage, observedShad, uvSearchFunctor);
        }
        else
        {
            cout << "A search method must be specified\n" << desc  << "\n";
            return 1;
        }

        curUVImage.write(vm["OutputUV"].as<string>().c_str());

        if(vm.count("ResultShad"))
        {
            cout << "Calcualting shading result:" << flush;
            for(int y(0); y < observedShad.height; ++y)
            {
                cout << "y: " << y << endl;
                for(int x(0); x < observedShad.width; ++x)
                {
                   observedShad[y][x] =
                           diffuseIllum[
                                diffuseIllum.convertUV2Index(curUVImage[y][x])
                                       ];
                }
            }
            observedShad.write(vm["ResultShad"].as<string>().c_str());
        }
    }
    else
    {
        cout << "Invalid options!!!\n" << desc  << "\n";
        return 1;
    }

    return 0;
}

