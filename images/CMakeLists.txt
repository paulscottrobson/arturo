# ***************************************************************************************
# ***************************************************************************************
#
#		Name : 		CMakeLists.txt
#		Author :	Paul Robson (paul@robsons.org.uk)
#		Date : 		18th December 2024
#		Purpose :	Outer level CMake configuration
#
# ***************************************************************************************
# ***************************************************************************************

# ***************************************************************************************
#
#										Set up
#
# ***************************************************************************************

cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

# ***************************************************************************************
# 
# 								  Set up the Pico SDK
#
# ***************************************************************************************

if(NOT DEFINED ENV{PICO_SDK_FETCH_FROM_GIT})
    set(PICO_SDK_FETCH_FROM_GIT on)
    message("Defaulting PICO_SDK_FETCH_FROM_GIT")
endif()
include(pico_sdk_import.cmake)

# ***************************************************************************************
#
#								Project configuration
#
# ***************************************************************************************

project(firmware C CXX ASM)

add_compile_options(-Wall -Werror -O2 -Wno-unused-function)

include_directories(
	include
)

pico_sdk_init()

# ***************************************************************************************
#
#									Install tinyUSB
#
# ***************************************************************************************

include(FetchContent)

# if((NOT DEFINED ENV{PICO_TINYUSB_FETCH_FROM_GIT}) OR ENV{PICO_TINYUSB_FETCH_FROM_GIT})
if(DEFINED ENV{PICO_TINYUSB_PATH})
    set(tinyusb_SOURCE_DIR $ENV{PICO_TINYUSB_PATH})
else()
    FetchContent_Declare(
        tinyusb
        GIT_REPOSITORY https://github.com/hathach/tinyusb.git
        GIT_TAG        0.17.0
    )
    FetchContent_MakeAvailable(tinyusb)
endif()
set(PICO_TINYUSB_PATH ${tinyusb_SOURCE_DIR})

# ***************************************************************************************
#
#							 	Access libdvi from PicoDVI
#
# ***************************************************************************************

add_subdirectory($ENV{PICO_DVI_PATH}/software/libdvi picodvi-build)

# ***************************************************************************************
#
#										Access FatFS
#
# ***************************************************************************************

add_library(fatfs INTERFACE)
target_sources(fatfs INTERFACE
    ${tinyusb_SOURCE_DIR}/lib/fatfs/source/ff.c
    ${tinyusb_SOURCE_DIR}/lib/fatfs/source/ffsystem.c
    ${tinyusb_SOURCE_DIR}/lib/fatfs/source/ffunicode.c
)
target_include_directories(fatfs INTERFACE ${tinyusb_SOURCE_DIR}/lib/fatfs/source/)

# ***************************************************************************************
#
#								Added the sources subdirectory
#
# ***************************************************************************************

add_subdirectory(firmware)


