# ***************************************************************************************
# ***************************************************************************************
#
#		Name : 		Makefile
#		Author :	Paul Robson (paul@robsons.org.uk)
#		Date : 		18th December 2024
#		Purpose :	Main firmware makefile, most of the work is done by CMake.
#
# ***************************************************************************************
# ***************************************************************************************

include ../build_env/common.make
include $(BUILDENVDIR)/config.make

PROJECT = kernel

all: build

always:

# ***************************************************************************************
#
#									Preliminary setup stuff
#
# ***************************************************************************************

CONFIG_OUT_FILE = firmware/kernel/include/support/__config.h
LOCALE_OUT_FILE = ../include/support/__locale.h

CMAKE_OUT_FILE = $(ROOTDIR)images/firmware/CMakeLists.txt

prelim_data: $(CONFIG_OUT_FILE)
	cd firmware/kernel/locale ; $(PYTHON) keymaps.py >$(LOCALE_OUT_FILE)

prelim: prelim_data bbc_font $(CMAKE_OUT_FILE)
	mkdir -p $(BINDIR)
	rm -f $(BINDIR)*.elf $(BINDIR)*.uf2

bbc_font:
	cd scripts ; $(PYTHON) makefont.py >$(ROOTDIR)images/firmware/kernel/include/support/font_bbc_8x8.h
#
#		Build the C configuration header file, passing constants into code.
#
$(CONFIG_OUT_FILE) : $(BUILDENVDIR)/config.make
	$(PYTHON) scripts/makeconfig.py '$(CONFIG_DATA)' >$(CONFIG_OUT_FILE)
#
#		Create the middle CMakeLists.txt file (in images/firmware) which sets
#		environment variables.
#
$(CMAKE_OUT_FILE) : always
	echo "#" >$(CMAKE_OUT_FILE)
	echo "# This is automatically generated." >>$(CMAKE_OUT_FILE)
	echo "#" >>$(CMAKE_OUT_FILE)
	echo 'set(APPLICATION_DIRECTORY "${ARTURO_APP_DIRECTORY}")' >>$(CMAKE_OUT_FILE)
	echo 'add_subdirectory(kernel)' >>$(CMAKE_OUT_FILE)

	cmake --fresh -B build 

# ***************************************************************************************
#
#									Build the firmware
#
# ***************************************************************************************

run: build upload
	
build: prelim always
	cmake --fresh -B build 
	$(MAKE) -C build
	cp build/firmware/$(PROJECT)/$(PROJECT).elf $(BINDIR)
	cp build/firmware/$(PROJECT)/$(PROJECT).uf2 $(BINDIR)

# ***************************************************************************************
#
#								Clean build directories
#
# ***************************************************************************************

cleanall: always
	rm -Rf  build/*

clean: always
	cmake --fresh -B build 
	cmake --build build --target clean

# ***************************************************************************************
#
#								Upload via openocd/
#
# ***************************************************************************************

upload: 
	$(UPLOADER) $(UPCONFIG) $(UPCOMMANDS)

	
