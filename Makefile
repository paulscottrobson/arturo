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
	make -C images run

sw:
	make -C simulator run

#
#		Build Doxygen documentation
#
docs:
	rm -Rf documents/htmldocs
	doxygen

view: docs
	firefox file://$(ROOTDIR)/documents/htmldocs/html/index.html &
