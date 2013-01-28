
#include "ImageRgba.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <string>

using namespace std;

using namespace boost::program_options;
namespace po = boost::program_options;



int main(int argc, char *argv[])
{
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
                ("help,h", "produce help message")
                ("in,i", po::value<string>(), "OpenExr image of the Normal values.")
                ("out,o", po::value<string>(), "OpenExr image UV output.")
                ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("in") && vm.count("out"))
    {
        cout << "Loading images\n";

        ImageRgba image(vm["in"].as<string>().c_str());

        {
            cout << "Calculating normal result:" << flush;
            for(int y(0); y < image.height; ++y)
            {
                cout << "y: " << y << endl;
                for(int x(0); x < image.width; ++x)
                {
                    Imf::Rgba &curPixel(image[y][x]);
                    Imath::V3f normal(curPixel.r,curPixel.g, curPixel.b);
                    Imath::V2f uv(NormalToWorldUV(normal));
                    curPixel = Imf::Rgba(uv.x, uv.y, 0.0);
                }
            }
            image.write(vm["out"].as<string>().c_str());
        }
    }
    else
    {
        cout << "Invalid options!!!\n" << desc  << "\n";
        return 1;
    }

    return 0;
}


