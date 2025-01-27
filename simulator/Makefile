# *******************************************************************************************
# *******************************************************************************************
#
#		Name : 		Makefile
#		Purpose :	Build Emulator
#		Date :		22nd December 2024
#		Author : 	Paul Robson (paul@robsons.org.uk)
#
# *******************************************************************************************
# *******************************************************************************************

include ../build_env/common.make
include ../build_env/config.make

APPNAME = artsim

TGTBIN = $(BINDIR)$(APPNAME)
KERNELDIR = $(ROOTDIR)images/firmware/kernel/

SOURCE1 = $(shell find -L source -name "*.c")
SOURCE2 = $(shell find -L $(ARTURO_APP_DIRECTORY) -name "*.c")
SOURCE3 = $(shell find -L $(KERNELDIR)support -name "*.c")

SOURCES = $(SOURCE1) $(SOURCE2) $(SOURCE3)
OBJECTS = $(subst .c,.o,$(SOURCES))
INCLUDES = -I include -I $(ARTURO_APP_DIRECTORY)/include -I $(KERNELDIR)include

SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs)

# ***************************************************************************************
#
#							Build application / compile files
#
# ***************************************************************************************

.any:

build : setup $(TGTBIN)

setup : .any
	mkdir -p $(ROOTDIR)bin
	make -C $(ROOTDIR)images prelim_data

run : build
	$(TGTBIN)

$(TGTBIN): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(SDL_LDFLAGS) -o $@

%.o:%.c
	$(CC) $(CADDRESSES) $(CFLAGS) $(SDL_CFLAGS) $(INCLUDES) -c -o $@ $<

