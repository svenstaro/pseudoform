path = %PATH%;%CD%\CodeBlocks\MinGW\bin
wget -c http://xorg.freedesktop.org/releases/individual/lib/libXrandr-1.3.0.tar.bz2
tar xjvf libXrandr-1.3.0.tar.bz2
cd libXaw-1.3.0
sh configure
make
exit
