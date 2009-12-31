set oldpath = %PATH%
echo %PATH%
path = %PATH%;%CD%\CodeBlocks\MinGW\bin
svn co https://svn.ogre3d.org/svnroot/ogre/trunk ogre
cd ogre
copy ..\bin\* Samples\Common\bin\Release\
mv Samples\Common\bin\Release\OIS_d.dll Samples\Common\bin\Debug\
cp Samples\Common\bin\Release\cg.dll Samples\Common\bin\Debug\
cp -r ../lib Dependencies/
cp -r ../include Dependencies/
mkdir build
cd build
path = %oldpath%
cmake -G "MinGW Makefiles" ..
make
exit
