import sys
import numpy as np

assert len(sys.argv) == 2

pts = np.unique(np.load(sys.argv[1]), axis=0)

for pt in pts:
    print(str(pt[0]) + ',' + str(pt[1]) + ',' + str(pt[2]) + ',' + str(pt[3]))
