#!/bin/env python3

import sys
blob=open(sys.argv[1],"rb").read()
print("""
/* image_data.h
   FORTH dictionary image as data.
*/

unsigned char image_data[] = {
""")

for i in range(len(blob)):
    print("0x%02x, " % blob[i], end="")
    if i%8 == 7:
        print()

print("""
};
""")
