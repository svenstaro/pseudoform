path = %PATH%;%CD%\CodeBlocks\MinGW\bin
copy "%CG_BIN_PATH%"\* bin\
copy "%CG_LIB_PATH%"\* lib\
cp -r "%CG_INC_PATH%"/* include/
