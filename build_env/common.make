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
MAKEFLAGS = --no-print-directory
CFLAGS = -Wall -Werror -O2 -fmax-errors=5 -Wno-unused-function -Wno-unused-variable
LDFLAGS = -fmax-errors=5

# *******************************************************************************************
#
#										Directories
#
# *******************************************************************************************

ROOTDIR =  $(dir $(realpath $(lastword $(MAKEFILE_LIST))))../
BINDIR = $(ROOTDIR)bin/
BUILDDIR = $(ROOTDIR)build/
BUILDENVDIR = $(ROOTDIR)build_env/
BASICDIR = $(ROOTDIR)basic/
BASICCODEDIR = $(BASICDIR)code/
INCLUDEDIR = $(BASICCODEDIR)include/
BASICSCRIPTDIR = $(BASICDIR)scripts/
APPDIR = $(ROOTDIR)applications/

# *******************************************************************************************
#
#				    Uncommenting .SILENT will shut the whole build up.
#
# *******************************************************************************************

ifndef VERBOSE
.SILENT:
endif
