path = %PATH%;%CD%\CodeBlocks\MinGW\bin
wget -c http://www.zlib.net/zlib-1.2.3.tar.gz
tar xzvf zlib-1.2.3.tar.gz
cd zlib-1.2.3
sh configure
make -f win32\Makefile.gcc
copy libz.a ..\lib
copy zlib.h ..\include
copy zconf.h ..\include
cd ..
rm -rf zlib-1.2.3
exit

