path = %PATH%;%CD%\CodeBlocks\MinGW\bin
wget -c http://downloads.sourceforge.net/freetype/freetype-2.3.11.tar.bz2
tar xjvf freetype-2.3.11.tar.bz2
cd freetype-2.3.11
sh configure
make
copy objs\.libs\libfreetype.a ..\lib
cp -r include/* ../include/
cd ..
rm -rf freetype-2.3.11
exit
