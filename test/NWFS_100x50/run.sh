#!/bin/bash
cd ../../Release/
make all
cd -
../../Release/NormalRefine --NWFS -u ../UVStartFromFlat.exr -i ../SHDiffuse3Light_100x50.exr -s ../SphereLit.exr -o resultUV.exr --ResultShad resultShad.exr
