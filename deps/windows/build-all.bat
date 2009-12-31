path = %PATH%;%CD%\CodeBlocks\MinGW\bin
mkdir bin
mkdir include
mkdir lib
echo Starting to download the DirectX runtime.
echo After the download finishes the setup will launch and you will need to
echo manually go through it and finish its installation.
echo Please install all default components. You may change the path if you want.
wget -nc "http://download.microsoft.com/download/B/7/9/B79FC9D7-47B8-48B7-A75E-101DEBEB5AB4/directx_aug2009_redist.exe"
start /wait directx_aug2009_redist.exe
echo Starting to download the DirectX SDK.
echo After the download finishes the setup will launch and you will need to
echo manually go through it and finish its installation.
echo Please install all default components. You may change the path if you want.
wget -nc "http://download.microsoft.com/download/4/C/F/4CFED5F5-B11C-4159-9ADC-E133B7E42E5C/DXSDK_Aug09.exe"
start /wait DXSDK_Aug09.exe
echo Starting to download the NVIDIA Cg SDK.
echo After the download finishes the setup will launch and you will need to
echo manually go through it and finish its installation.
echo Please install all default components. You may change the path if you want.
wget -nc "http://developer.download.nvidia.com/cg/Cg_2.2/Cg-2.2_October2009_Setup.exe"
start /wait Cg-2.2_October2009_Setup.exe
start /WAIT build-zlib.bat
start /WAIT build-freeimage.bat
start /WAIT build-ois.bat
start /WAIT build-freetype2.bat
start /WAIT build-boost.bat
start /WAIT build-nvcg.bat
start /WAIT build-dxsdk.bat
start /WAIT build-ogre.bat
