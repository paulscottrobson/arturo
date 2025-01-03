# *******************************************************************************************
# *******************************************************************************************
#
#		Name : 		Makefile
#		Purpose :	BDF Font code
#		Date :		1st January 2025
#		Author : 	Paul Robson (paul@robsons.org.uk)
#
# *******************************************************************************************
# *******************************************************************************************


SRCFONTS1 = $(shell find . -name "*.ttf")
SRCFONTS2 = $(shell find . -name "*.otf")
CONVERT = fontconvert/fontconvert 
RANGE = 32 127
SYMRANGE = 32 255
TARGET = ../fontdata.h

fonts: convert clean $(SRCFONTS1) $(SRCFONTS2)

convert:
	make -C fontconvert

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

