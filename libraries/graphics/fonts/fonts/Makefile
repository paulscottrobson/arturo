# *******************************************************************************************
# *******************************************************************************************
#
#		Name : 		Makefile
#		Purpose :	BDF Font code
#		Reviewed :	No
#		Date :		1st January 2025
#		Author : 	Paul Robson (paul@robsons.org.uk)
#
# *******************************************************************************************
# *******************************************************************************************

include ../../../../build_env/common.make

SRCFONTS1 = $(shell find . -name "*.ttf")
SRCFONTS2 = $(shell find . -name "*.otf")
CONVERT = fontconvert/fontconvert 
RANGE = 32 127
SYMRANGE = 32 255
TARGET = ../fontdata.h

fonts: convert clean $(SRCFONTS1) $(SRCFONTS2) bitmaps

convert:
	make -C fontconvert

bitmaps:
	$(PYTHON) convbmf.py 3 5 small3x5.bmf >>$(TARGET)
	$(PYTHON) convbmf.py 4 6 small4x6.bmf >>$(TARGET)
	$(PYTHON) convbmf.py 5 5 small5x5.bmf >>$(TARGET)
	$(PYTHON) convbmf.py 5 7 small5x7.bmf >>$(TARGET)
	$(PYTHON) convbmf.py 8 8 system.bmf >>$(TARGET)


clean:
	rm -f $(TARGET)
	
%.ttf:
	$(CONVERT) $@ 7 $(RANGE) >>$(TARGET)
	$(CONVERT) $@ 9 $(RANGE) >>$(TARGET)
	$(CONVERT) $@ 12 $(RANGE) >>$(TARGET)
	$(CONVERT) $@ 18 $(RANGE) >>$(TARGET)

%.otf:
	$(CONVERT) $@ 7 $(RANGE) >>$(TARGET)
	$(CONVERT) $@ 9 $(RANGE) >>$(TARGET)
	$(CONVERT) $@ 12 $(RANGE) >>$(TARGET)
	$(CONVERT) $@ 18 $(RANGE) >>$(TARGET)

symbols.otf:
	$(CONVERT) $@ 7 $(SYMRANGE) >>$(TARGET)
	$(CONVERT) $@ 9 $(SYMRANGE) >>$(TARGET)
	$(CONVERT) $@ 12 $(SYMRANGE) >>$(TARGET)
	$(CONVERT) $@ 18 $(SYMRANGE) >>$(TARGET)

