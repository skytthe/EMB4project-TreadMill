CC = arm-linux-gnueabi-gcc
CXX = arm-linux-gnueabi-g++
CCFLAGS = -pipe -O2 -fmessage-length=0 -fPIC -Wno-deprecated -Wall -W -Wno-unused
CXXFLAGS = -pipe -O2 -fmessage-length=0 -fPIC -Wno-deprecated -Wall -W -Wno-unused
RM=rm -rf

# append your additional includes with -I<path_to_new_dir>
INCPATH=-I/usr/arm-linux-gnueabi/include -I/usr/arm-linux-gnueabi/opencv/include
# append your additional library search dirs with -L<path_to_new_dir>
LIBPATH= -L/usr/arm-linux-gnueabi/opencv/lib -lopencv_core -lopencv_imgproc -lopencv_nonfree -lopencv_highgui -lprotobuf
# add additional linker flags as needed 
LFLAGS= 

LIBS = -rdynamic /usr/arm-linux-gnueabi/lib/librt.so.1

####### Files
CCSOURCES=$(wildcard *.c)
CCOBJECTS=$(CCSOURCES:.c=.o)
CXXSOURCES=$(wildcard *.cpp)
CXXOBJECTS=$(CXXSOURCES:.cpp=.o)
OBJECTS = $(CCOBJECTS) $(CXXOBJECTS)

TARGET = marker_detector

first: all
####### Implicit rules

.SUFFIXES: .o .cpp .c

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

%.o: %.c
	$(CC) -c $(CCFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: Makefile $(TARGET) clean-obj

$(TARGET):  $(OBJECTS)
	$(CXX) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBPATH) $(LIBS)

clean-obj:
	-$(RM) $(OBJECTS)

clean:
	-$(RM) $(OBJECTS)
	-$(RM) $(TARGET)
