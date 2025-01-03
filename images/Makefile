# ***************************************************************************************
# ***************************************************************************************
#
#		Name : 		Makefile
#		Author :	Paul Robson (paul@robsons.org.uk)
#		Date : 		18th December 2024
#		Reviewed :	No
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

LOGO_DIRECTORY = $(ROOTDIR)images/scripts/
LOGO_OUT_FILE = $(ROOTDIR)images/firmware/kernel/include/support/__logo_data.h
LOGO_IN_FILE = $(LOGO_DIRECTORY)olimex.png

CMAKE_OUT_FILE = $(ROOTDIR)images/firmware/CMakeLists.txt

prelim: $(CONFIG_OUT_FILE) $(LOGO_OUT_FILE) $(CMAKE_OUT_FILE)
	mkdir -p $(BINDIR)
	rm -f $(BINDIR)*.elf $(BINDIR)*.uf2
	cd firmware/kernel/locale ; $(PYTHON) keymaps.py >$(LOCALE_OUT_FILE)

#		Build the C configuration header file, passing constants into code.
#
$(CONFIG_OUT_FILE) : $(BUILDENVDIR)/config.make
	$(PYTHON) scripts/makeconfig.py '$(CONFIG_DATA)' >$(CONFIG_OUT_FILE)
#
#		Build any logos required. (Currently just the Olimex one)
#
$(LOGO_OUT_FILE) : $(LOGO_IN_FILE)
	cd $(LOGO_DIRECTORY) ; $(PYTHON) makelogo.py >$(LOGO_OUT_FILE)
#
#		Create the middle CMakeLists.txt file (in images/firmware) which sets
#		environment variables.
#
$(CMAKE_OUT_FILE) : $(BUILDENVDIR)/config.make
	echo "#" >$(CMAKE_OUT_FILE)
	echo "# This is automatically generated." >>$(CMAKE_OUT_FILE)
	echo "#" >>$(CMAKE_OUT_FILE)
	echo 'set(APPLICATION_DIRECTORY "${ARTURO_APP_DIRECTORY}")' >>$(CMAKE_OUT_FILE)
	echo 'set(LIBRARY_DIRECTORY "$(ARTURO_LIB_DIRECTORY)")' >>$(CMAKE_OUT_FILE)
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

	