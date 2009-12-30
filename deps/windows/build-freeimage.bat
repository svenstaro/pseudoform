path = %PATH%;%CD%\CodeBlocks\MinGW\bin
wget -c http://downloads.sourceforge.net/freeimage/FreeImage3131.zip
unzip FreeImage3131.zip
cd FreeImage
CMD /C clean.bat
FREEIMAGE_LIBRARY_TYPE = STATIC
make -f Makefile.minGW
copy Dist\FreeImage.dll ..\bin
copy Dist\FreeImage.lib ..\lib
copy Dist\FreeImage.h ..\include
cd ..
rm -rf FreeImage
