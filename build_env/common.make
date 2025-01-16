# *******************************************************************************************
# *******************************************************************************************
#
#		Name : 		common.make
#		Purpose :	Common make
#		Date :		12th December 2024
#		Author : 	Paul Robson (paul@robsons.org.uk)
#
# *******************************************************************************************
# *******************************************************************************************

# *******************************************************************************************
#
#										Configuration
#
# *******************************************************************************************

CC = gcc
PYTHON = python3
RUBY = ruby
MAKEFLAGS = --no-print-directory
CFLAGS = -Wall -Werror -O2 -fmax-errors=5 -Wno-unused-function -Wno-unused-variable
LDFLAGS = -fmax-errors=5

# *******************************************************************************************
#
#										Directories
#
# *******************************************************************************************
#
#		It's really unhappy if there are spaces after these things.
#

#
#		Root directory of repo
#
ROOTDIR =  $(dir $(realpath $(lastword $(MAKEFILE_LIST))))../
#
#		Stores executables and uf2
#
BINDIR = $(ROOTDIR)bin/
#
#		Where the build environment files are (e.g. like this)
#
BUILDENVDIR = $(ROOTDIR)build_env/
#
#		Where applications are stored. The actual location is set in config.make
#
APPDIR = $(ROOTDIR)applications/

# *******************************************************************************************
#
#				    Uncommenting .SILENT will shut the whole build up.
#
# *******************************************************************************************

ifndef VERBOSE
.SILENT:
endif
