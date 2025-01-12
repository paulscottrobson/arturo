# ***************************************************************************************
# ***************************************************************************************
#
#      Name :      t1000.py
#      Authors :   Paul Robson (paul@robsons.org.uk)
#      Date :      3rd January 2025
#      Purpose :   Calculate timer division approximation.
#
# ***************************************************************************************
# ***************************************************************************************

import random

random.seed(42)

def calcms(us):
	us = us >> 9
	return int(us * 131) >> 8


for i in range(0,30):
	timeus = random.randint(0,1000*1000*1000)
	timems = timeus // 1000
	calctimems = calcms(timeus)
	err = int(abs(calctimems-timems)/timems*100*100)/100
	print("{0:9} us ({1:6} ms) as {2:6} ms {3}%".format(timeus,timems,calctimems,err))
