set oldpath = %PATH%
echo %PATH%
path = %PATH%;%CD%\CodeBlocks\MinGW\bin
wget -c http://downloads.sourceforge.net/project/ogre/ogre/1.7/ogre-v1-7-0RC1.tar.bz2
tar xvjf ogre-v1-7-0RC1.tar.bz2
cd ogre
copy ..\bin\* Samples\Common\bin\Release\
mv Samples\Common\bin\Release\OIS_d.dll Samples\Common\bin\Debug\
cp Samples\Common\bin\Release\cg.dll Samples\Common\bin\Debug\
mkdir Dependencies
cp -r ../lib Dependencies/
cp -r ../include Dependencies/
mkdir build
cd build
path = %oldpath%
cmake -G "MinGW Makefiles" ..
make
exit
