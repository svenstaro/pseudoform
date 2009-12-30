path = %PATH%;%CD%\CodeBlocks\MinGW\bin
mkdir bin
mkdir include
mkdir lib
wget -c "http://download.microsoft.com/download/4/C/F/4CFED5F5-B11C-4159-9ADC-E133B7E42E5C/DXSDK_Aug09.exe"
start /wait DXSDK_Aug09.exe
wget -c "http://developer.download.nvidia.com/cg/Cg_2.2/Cg-2.2_October2009_Setup.exe"
start /wait Cg-2.2_October2009_Setup.exe
start /WAIT build-zlib.bat
start /WAIT buiid-freeimage.bat
start /WAIT build-ois.bat
start /WAIT build-freetype2.bat
start /WAIT build-boost.bat
start /WAIT build-nvcg.bat
start /WAIT build-dxsdk.bat
start /WAIT build-ogre.bat
