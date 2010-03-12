SRC = ../newtonSDK/sdk/dMath/dMatrix.cpp ../newtonSDK/sdk/dMath/dQuaternion.cpp ../newtonSDK/sdk/dMath/dVector.cpp \
 ../newtonSDK/sdk/dMath/dMathDefines.cpp ../newtonSDK/sdk/dMath/dStdAfxMath.cpp ../newtonSDK/sdk/dCustomJoints/NewtonCustomJoint.cpp \
 ../newtonSDK/sdk/dCustomJoints/CustomJointLibraryStdAfx.cpp

OBJ = dMatrix.o dQuaternion.o dVector.o dMathDefines.o dStdAfxMath.o NewtonCustomJoint.o CustomJointLibraryStdAfx.o

OUT = sdk/libnewton_utils.a

INCLUDES = -I../newtonSDK/sdk/dMath -I../newtonSDK/sdk/dCustomJoints -I../newtonSDK/sdk

CCFLAGS =

CCC = g++

.SUFFIXES: .cpp .o .h

.cpp.o:
	$(CCC) $(INCLUDES) $(CCFLAGS) -c $< -o $@

default: $(OUT)

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

#dep:
#	makedepend -- $(CCFLAGS) -- $(INCLUDES) $(SRC) 


dMatrix.o: ../newtonSDK/sdk/dMath/dMatrix.cpp \
 ../newtonSDK/sdk/dMath/dStdAfxMath.h \
 ../newtonSDK/sdk/dMath/dMathDefines.h ../newtonSDK/sdk/dMath/dVector.h \
 ../newtonSDK/sdk/dMath/dMatrix.h ../newtonSDK/sdk/dMath/dVector.h \
 ../newtonSDK/sdk/dMath/dQuaternion.h
	$(CCC) $(INCLUDES) $(CCFLAGS) -c ../newtonSDK/sdk/dMath/dMatrix.cpp -o dMatrix.o
dQuaternion.o: ../newtonSDK/sdk/dMath/dQuaternion.cpp \
 ../newtonSDK/sdk/dMath/dStdAfxMath.h \
 ../newtonSDK/sdk/dMath/dMathDefines.h ../newtonSDK/sdk/dMath/dVector.h \
 ../newtonSDK/sdk/dMath/dMatrix.h ../newtonSDK/sdk/dMath/dVector.h \
 ../newtonSDK/sdk/dMath/dQuaternion.h
	$(CCC) $(INCLUDES) $(CCFLAGS) -c ../newtonSDK/sdk/dMath/dQuaternion.cpp -o dQuaternion.o
dVector.o: ../newtonSDK/sdk/dMath/dVector.cpp \
 ../newtonSDK/sdk/dMath/dStdAfxMath.h
	$(CCC) $(INCLUDES) $(CCFLAGS) -c ../newtonSDK/sdk/dMath/dVector.cpp -o dVector.o
dMathDefines.o: ../newtonSDK/sdk/dMath/dMathDefines.cpp \
 ../newtonSDK/sdk/dMath/dStdAfxMath.h \
 ../newtonSDK/sdk/dMath/dMathDefines.h
	$(CCC) $(INCLUDES) $(CCFLAGS) -c ../newtonSDK/sdk/dMath/dMathDefines.cpp -o dMathDefines.o
dStdAfxMath.o: ../newtonSDK/sdk/dMath/dStdAfxMath.cpp \
 ../newtonSDK/sdk/dMath/dStdAfxMath.h
	$(CCC) $(INCLUDES) $(CCFLAGS) -c ../newtonSDK/sdk/dMath/dStdAfxMath.cpp -o dStdAfxMath.o
NewtonCustomJoint.o: ../newtonSDK/sdk/dCustomJoints/NewtonCustomJoint.cpp \
 ../newtonSDK/sdk/dCustomJoints/CustomJointLibraryStdAfx.h \
 ../newtonSDK/sdk/dCustomJoints/NewtonCustomJoint.h \
 ../newtonSDK/sdk/dMath/dMathDefines.h ../newtonSDK/sdk/dMath/dVector.h \
 ../newtonSDK/sdk/dMath/dMathDefines.h ../newtonSDK/sdk/dMath/dMatrix.h \
 ../newtonSDK/sdk/dMath/dQuaternion.h \
 ../newtonSDK/sdk/dCustomJoints/NewtonCustomJoint.h
	$(CCC) $(INCLUDES) $(CCFLAGS) -c ../newtonSDK/sdk/dCustomJoints/NewtonCustomJoint.cpp -o NewtonCustomJoint.o
CustomJointLibraryStdAfx.o: \
 ../newtonSDK/sdk/dCustomJoints/CustomJointLibraryStdAfx.cpp \
 ../newtonSDK/sdk/dCustomJoints/CustomJointLibraryStdAfx.h \
 ../newtonSDK/sdk/dCustomJoints/NewtonCustomJoint.h \
 ../newtonSDK/sdk/dMath/dMathDefines.h ../newtonSDK/sdk/dMath/dVector.h \
 ../newtonSDK/sdk/dMath/dMathDefines.h ../newtonSDK/sdk/dMath/dMatrix.h \
 ../newtonSDK/sdk/dMath/dQuaternion.h
	$(CCC) $(INCLUDES) $(CCFLAGS) -c ../newtonSDK/sdk/dCustomJoints/CustomJointLibraryStdAfx.cpp -o CustomJointLibraryStdAfx.o
