# *******************************************************************************************
# *******************************************************************************************
#
#		Name : 		makeindex.py
#		Purpose :	Create index for fonts
#		Date :		3rd January 2025
#		Reviewed :	No
#		Author : 	Paul Robson (paul@robsons.org.uk)
#
# *******************************************************************************************
# *******************************************************************************************

import os,sys,re

fonts = {}
for s in open("fontdata.h").readlines():
	if s.find("FONTInfo") >= 0:
		id = s.split()[2].lower()
		m = re.match("^\\_?(.*?)\\_?(\\d+)pt[678]b",id)
		fonts[id] = { "name":m.group(1),"size":int(m.group(2)) }

keys = [x for x in fonts.keys()]
keys.sort(key = lambda x:"{0}{1:03}".format(fonts[x]["name"],fonts[x]["size"]))

h = open("fontindex.h","w")
h.write("static const FONTInfo *_dictionary[] = {{ {0} }};\n\n".format(",".join(["&"+x for x in keys])))
h.close()

h = open("fontinclude.h","w")
h.write("#pragma once\n\n")
h.write("#define FONT_COUNT ({0})\n".format(len(keys)))
n = 0
for k in keys:
	h.write("#define FONT_{0}_{1} ({2})\n".format(fonts[k]["name"].upper(),fonts[k]["size"],n))
	n += 1
h.close()
