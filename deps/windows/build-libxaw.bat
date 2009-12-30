path = %PATH%;%CD%\CodeBlocks\MinGW\bin
wget -c http://xorg.freedesktop.org/releases/individual/lib/libXaw-1.0.7.tar.bz2
tar xjvf libXaw-1.0.7.tar.bz2
cd libXaw-1.0.7
sh configure
make
