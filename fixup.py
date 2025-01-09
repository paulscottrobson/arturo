import os

def fixup(fileName):
	h = open(fileName)
	source = [x.rstrip() for x in h.readlines()]
	h.close()

	if source[0][:3] != "/**":
		return
	print(fileName,len(source[1]))
	if len(source[1]) > 8:
		return

	p = (fileName.split(os.sep)[-1])
	source[1] += "   "+p

	p = 0
	while not (source[p].startswith("//") and source[p].find("Purpose :") >= 0):
		p += 1

	s = source[p][source[p].find(":")+1:].strip()
	source[3] += "      "+s

	h = open(fileName,"w")
	h.write("\n".join(source))
	h.close()

	print("Fixed up "+fileName)


for root,dirs,files in os.walk("."):
	for f in files:
		if f.endswith(".c"):
			tgt = root + os.sep+f
			fixup(tgt)

