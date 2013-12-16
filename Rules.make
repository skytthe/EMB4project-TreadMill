# Global path definitions for all demos

#Specify target architecture here or give as command line option
# allowed values Linux_x86, Linux_x64, BBB

######################## Defines ######################
MKDIR = mkdir -p
RMDIR = rm -r -f
DEL_FILE = rm -f

############### DO NOT EDIT BELOW THIS LINE ###########

# Check Target Architecture

ALLOWED_ARCHS = Linux_x86 Linux_x64 BBB

ifndef TARGET_ARCH
  TARGET_ARCH = BBB
  $(info Target architecture not specified - Selecting BBB)
endif

_TARGET_ARCH := $(filter $(TARGET_ARCH),$(ALLOWED_ARCHS))

$(info Building for $(_TARGET_ARCH))

ifndef _TARGET_ARCH
  $(error Variable TARGET_ARCH not set correctly! Possible target architectures: $(ALLOWED_ARCHS))
endif

## Linux_x86
CROSS_COMPILE_PREFIX_Linux_x86 =
INCPATH_Linux_x86 = 
LDFLAGS_Linux_x86 = -Wl,-rpath,/usr/local/lib

## Linux_x64
CROSS_COMPILE_PREFIX_Linux_x64 =
INCPATH_Linux_x64 =
LDFLAGS_Linux_x64 = -Wl,-rpath,/usr/local/lib

## D3
CROSS_COMPILE_PREFIX_BBB = arm-linux-gnueabi-
INCPATH_BBB = -I/usr/arm-linux-gnueabi/include -I/usr/arm-linux-gnueabi/opencv/include
#LDFLAGS_BBB = -L/usr/arm-linux-gnueabi/opencv/lib -rdynamic /usr/arm-linux-gnueabi/lib/librt.so.1 -Wl,-rpath,/usr/arm-linux-gnueabi/lib
LDFLAGS_BBB = -L/usr/arm-linux-gnueabi/opencv/lib -rdynamic /usr/arm-linux-gnueabi/lib/librt.so.1 -Wl,-rpath,/usr/arm-linux-gnueabi/lib
################ Put things together ####################

CROSS_COMPILE_PREFIX 	= ${CROSS_COMPILE_PREFIX_${_TARGET_ARCH}}
INCPATH         	= ${INCPATH_${_TARGET_ARCH}}
LDFLAGS           	= ${LDFLAGS_${_TARGET_ARCH}}


