# *******************************************************************************************
# *******************************************************************************************
#
#		Name : 		convbmf.py
#		Purpose :	Convert BMF to Adafruit format.
#		Date :		4th January 2025
# 		Reviwed: 	No
#		Author : 	Paul Robson (paul@robsons.org.uk)
#
# *******************************************************************************************
# *******************************************************************************************

import os,sys,re

# *******************************************************************************************
#
#								Represents a BMF font
#
# *******************************************************************************************

class BMFFont(object):
	#
	#		Initialise new object and read data in from file
	#
	def __init__(self,fileName,width,height):
		self.width = width
		self.height = height
		self.currentChar = None
		self.glyphs = {}
		self.first = 9999
		self.last = -9999
		self.identifier = "_"+fileName.lower().replace(".bmf","") + "_" + str(self.height)+"pt7b"

		s = open(fileName).read(-1).split()  													# Get all the parts
		while len(s) != 0:  																	# While not done
			if s[0].startswith("|"):  															# New character
				self.currentChar = self.currentChar+1 if s[0]=='|' else self.evaluate(s[0][1:]) # get new character number
				assert self.currentChar not in self.glyphs,"Dup {0}".format(self.currentChar)  	# already defined.
				self.glyphs[self.currentChar] = []  											# clear list of bytes
				self.first = min(self.first,self.currentChar)
				self.last = max(self.last,self.currentChar)
			else:
				assert self.currentChar is not None,"Initial current char is not set"
				self.glyphs[self.currentChar].append(self.evaluate(s[0]))
			s = s[1:]
	#
	#		Evaluate simple term
	#
	def evaluate(self,s):
		return int(s[1:],16) if s.startswith("$") else int(s)
	#
	#		Process each glyph, building up the data and offset tables.
	#
	def process(self):
		self.glyphOffsets = {}  																# Offset to each glyph.
		self.data = []   																		# pixel data.
		for c in range(self.first,self.last+1):  												# for each character
			if c in self.glyphs:
				self.glyphOffsets[c] = len(self.data)    										# start at the new place.
				self.convertBitmap(self.glyphs[c])    											# add the glyph bitmap data
			else:  																				# character does not exist.
				self.glyphOffsets[c] = 0xFFFF   								
	#
	#		Do one glyph, converting the bitmap to pixel bits
	#
	def convertBitmap(self,byteData):
		byteData = byteData + [0] * self.height   												# make sure it is big enough
		self.data.append(0)  																	# goes in a new byte
		bitMask = 0x80   																		# start with MSB
		for y in range(0,self.height):   														# scan the bitmap data.
			for x in range(0,self.width):
				if (byteData[y] & (0x80 >> x)) != 0:  											# bit set ?
					self.data[-1] |= bitMask  													# set output stream bit
				bitMask >>= 1  																	# shift bitmask right
				if bitMask == 0:  																# need a new byte
					self.data.append(0)
					bitMask = 0x80
	#
	#		Render the new font
	#
	def render(self):
		bdata = ",".join([str(x) for x in self.data])   										# Output the bitmap data
		print("const uint8_t {0}Bitmaps[] = {{ {1} }};\n\n".format(self.identifier,bdata))

		print("const FONTGlyph {0}Glyphs[] = {{\n".format(self.identifier))  					# output the glyphs
		for c in range(self.first,self.last+1):   												# Note : 8x8 fonts are output as advance/depth 8 not size+1
			gd = [ self.glyphOffsets[c],self.width,self.height,self.width+1 if self.width != 8 else 8,0,-self.height ]
			gd = "{"+(",".join([str(c) for c in gd]))+"}" + ("," if c < self.last else "")
			print("\t\t{0}".format(gd))
		print("};\n\n")

		print("const FONTInfo {0} = {{".format(self.identifier))
		print("\t\t(uint8_t  *){0}Bitmaps,".format(self.identifier))
		print("\t\t(FONTGlyph *){0}Glyphs,".format(self.identifier))
		print("\t\t{0},{1},{2} }};\n\n".format(self.first,self.last,self.height+1 if self.height != 8 else 8))

f = BMFFont(sys.argv[3],int(sys.argv[2]),int(sys.argv[1]))
f.process()
f.render()

