path = %PATH%;%CD%\CodeBlocks\MinGW\bin
wget -c "http://downloads.sourceforge.net/wgois/ois_1.2.0.tar.gz"
tar xzvf ois_1.2.0.tar.gz
cd ois
copy "%DXSDK_DIR%"\include\* includes\
copy ..\extras\ois.cbp %CD%\win32\ois.cbp
start /WAIT ..\codeblocks\run_codeblocks.bat --build --target="Release" %CD%\win32\ois.cbp
start /WAIT ..\codeblocks\run_codeblocks.bat --build --target="OIS_ReleaseDll" %CD%\win32\ois.cbp
start /WAIT ..\codeblocks\run_codeblocks.bat --build --target="Debug" %CD%\win32\ois.cbp
start /WAIT ..\codeblocks\run_codeblocks.bat --build --target="OIS_DebugDll" %CD%\win32\ois.cbp
copy lib\libOIS_static.a ..\lib\libOIS.a
copy lib\libOIS_static.a ..\lib\libOIS_d.a
copy dll\OIS.dll ..\bin
copy dll\OIS_d.dll ..\bin
cp -r includes/* ../include/
cd ..
rm -rf ois
exit
