path = %PATH%;%CD%\CodeBlocks\MinGW\bin
wget -c http://downloads.sourceforge.net/boost/boost_1_41_0.tar.bz2
tar xjvf boost_1_41_0.tar.bz2
cd boost_1_41_0
bjam --toolset=gcc --variant=release -j3 link=static runtime-link=static threading=multi stage
copy stage\lib\* ..\lib\
cp -r boost ../include/
cd ..
rm -rf boost_1_41_0
exit

