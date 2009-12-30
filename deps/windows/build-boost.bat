path = %PATH%;%CD%\CodeBlocks\MinGW\bin
wget -c http://sourceforge.net/projects/boost/files/boost/1.41.0/boost_1_41_0.tar.bz2/download
tar xjvf boost_1_41_0.tar.bz2
cd boost_1_41_0
bjam --toolset=gcc --variant=release -j3 link=static runtime-link=static threading=multi stage
copy stage\lib\* ..\lib\
cp -r boost ../include/
cd ..
rm -rf boost_1_41_0
