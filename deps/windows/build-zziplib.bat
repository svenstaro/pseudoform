path = %PATH%;%CD%\CodeBlocks\MinGW\bin
wget -c http://sourceforge.net/projects/zziplib/files/zziplib13/0.13.58/zziplib-0.13.58.tar.bz2/download
tar xjvf zziplib-0.13.58.tar.bz2
cd zziplib-0.13.58
CPPFLAGS=-I../../include
LDFLAGS=-L../../lib
sh configure
make
copy libzzip.a ..\lib
copy zzip\zzip.h ..\include
cd ..
pause 
rem rm -rf zziplib-0.13.58
exit

