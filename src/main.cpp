
#include <OpenEXR/ImfRgbaFile.h>
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
	    ("help", "produce help message")
	    ("PriorNormals,p", po::value<string>(), "OpenExr image of the prior normals.")
	    ("DiffuseIllum,i", po::value<string>(), "OpenExr image of the diffuse illumination map.")
	    ("OutputNormals,o", po::value<string>()->default_value("outNorm.exr"),
	    		                                 "Refined normal OpenExr image.")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
	    cout << desc << "\n";
	    return 1;
	}

	if (vm.count("PriorNormals")) {
	    cout << "PriorNormals was set to "
	 << vm["PriorNormals"].as<string>() << ".\n";
	} else {
	    cout << "PriorNormals was not set.\n";
	}

	return 0;
}

//void
//readRgba2 (const char fileName[])
//{
//RgbaInputFile file (fileName);
//Box2i dw = file.dataWindow();
//int width = dw.max.x - dw.min.x + 1;
//int height = dw.max.y - dw.min.y + 1;
//Array2D<Rgba> pixels (10, width);
