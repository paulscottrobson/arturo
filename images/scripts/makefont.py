# ***************************************************************************************
# ***************************************************************************************
#
#      Name :      makefont.py
#      Authors :   Paul Robson (paul@robsons.org.uk)
#      Date :      29th January 2025
#      Purpose :   Extract BBC Micro font.
#
# ***************************************************************************************
# ***************************************************************************************

osRom = [x for x in open("os12.rom","rb").read(-1)]

print("#pragma once")
print("#ifndef _IMG_ASSET_SECTION")
print("#define _IMG_ASSET_SECTION \".data\"")
print("#endif")
print("static const char __attribute__((aligned(4), section(_IMG_ASSET_SECTION \".font_8x8\"))) font_8x8[] = {")
print(",".join([str(x) for x in osRom[:96*8]]))
print("};")