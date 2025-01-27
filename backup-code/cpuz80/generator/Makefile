# *******************************************************************************************
# *******************************************************************************************
#
#		Name : 		Makefile
#		Purpose :	Generate Z80 CPU code
#		Date :		16th January 2025
#		Author : 	Paul Robson (paul@robsons.org.uk)
#
# *******************************************************************************************
# *******************************************************************************************

include ../../../build_env/common.make
include ../../../build_env/config.make

.PHONY: all 

all: 
	$(RUBY) generate.rb
	cp code/*.h ..
	echo "Successfully generated Z80 opcodes."