# *******************************************************************************************
# *******************************************************************************************
#
#		Name : 		Makefile
#		Purpose :	Build Emulator
#		Date :		29th December 2024
#		Author : 	Paul Robson (paul@robsons.org.uk)
#
# *******************************************************************************************
# *******************************************************************************************

include build_env/common.make
include build_env/config.make

# ***************************************************************************************
#
#								Run on hardware or software
#
# ***************************************************************************************

hw:
	make -C kernel run

sw:
	cd simulator ; make run

#
#		Build Doxygen documentation
#
docs:
	rm -Rf documents/htmldocs
	doxygen

view: docs
	firefox file://$(ROOTDIR)/documents/htmldocs/html/index.html &
