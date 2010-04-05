INCLUDE_DIRECTORIES(newton/ newton/dAnimation/ newton/dContainers/ newton/dCustomJoints newton/dMath)

AUX_SOURCE_DIRECTORY(newton/dAnimation/ Newton_dAnimation_SRCS)
AUX_SOURCE_DIRECTORY(newton/dContainers/ Newton_dContainers_SRCS)
AUX_SOURCE_DIRECTORY(newton/dCustomJoints/ Newton_dCustomJoints_SRCS)
AUX_SOURCE_DIRECTORY(newton/dMath/ Newton_dMath_SRCS)

ADD_LIBRARY(Newton_extras STATIC
	${Newton_dAnimation_SRCS}
	${Newton_dContainers_SRCS}
	${Newton_dCustomJoints_SRCS}
	${Newton_dMath_SRCS})

IF(APPLE)
	FIND_LIBRARY(Newton_LIBRARY NAMES NewtonMac64 NewtonMac32 PATHS newton/)
ELSEIF(UNIX)
	FIND_LIBRARY(Newton_LIBRARY NAMES NewtonLinux64 NewtonLinux32 PATHS newton/)
ELSEIF(WIN32)
	FIND_LIBRARY(Newton_LIBRARY NAMES NewtonWin64 NewtonWin32 PATHS newton/)
ENDIF(APPLE)
