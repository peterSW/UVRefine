
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
                ("UVin,i", po::value<string>(), "OpenExr image of the UV values.")
                ("Nout,o", po::value<string>(), "OpenExr image normals output.")
                ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("UVin") && vm.count("Nout"))
    {
        cout << "Loading images\n";

        ImageRgba image(vm["UVin"].as<string>().c_str());

        {
            cout << "Calculating normal result:" << flush;
            for(int y(0); y < image.height; ++y)
            {
                cout << "y: " << y << endl;
                for(int x(0); x < image.width; ++x)
                {
                    Imf::Rgba &curPixel(image[y][x]);
                    Imath::V2f uv(curPixel.r,curPixel.g);
                    Imath::V3f normal(WorldUVToNormal(uv));
                    curPixel = Imf::Rgba(normal.x, normal.y, normal.z);
                }
            }
            image.write(vm["Nout"].as<string>().c_str());
        }
    }
    else
    {
        cout << "Invalid options!!!\n" << desc  << "\n";
        return 1;
    }

    return 0;
}


