include ../Rules.make

CC = $(CROSS_COMPILE_PREFIX)gcc
CXX = $(CROSS_COMPILE_PREFIX)g++
CCFLAGS = -pipe -O3 -fmessage-length=0 -fPIC -Wno-deprecated -Wall -W -Wno-unused
CXXFLAGS = -pipe -O3 -fmessage-length=0 -fPIC -Wno-deprecated -Wall -W -Wno-unused -std=c++0x

LIBS =  -lopencv_core -lopencv_imgproc -lopencv_nonfree -lopencv_highgui -lprotobuf -lzmq

####### Files
OBJECTS =   main.o \
            find_markers.o \
            GeometryTypes.o \
            Marker.o \
            ../common/MarkerMessage.pb.o

TARGET = build/$(_TARGET_ARCH)/marker_detector

first: all
####### Implicit rules

.SUFFIXES: .o .cpp

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CCFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: Makefile $(TARGET) clean-obj

$(TARGET):  $(OBJECTS)
	$(MKDIR) build
	$(MKDIR) build/$(_TARGET_ARCH)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(LIBS)

clean-obj:
	-$(DEL_FILE) $(OBJECTS)

clean:
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core
	-$(DEL_FILE) $(TARGET)
	-$(RMDIR) build/$(_TARGET_ARCH)
