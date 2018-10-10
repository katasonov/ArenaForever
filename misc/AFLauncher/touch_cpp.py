import os
import re
import datetime
import sys


mypath = os.path.dirname(os.path.abspath(__file__)) + '/'

with open(mypath + sys.argv[1], 'r') as f:
    content = f.readlines()
# you may also want to remove whitespace characters like `\n` at the end of each line

with open(mypath + sys.argv[1], 'w') as f:
	for line in content:
		if "timestamp:=[" in line:
			line = "//timestamp:=[" + str(datetime.datetime.now()) + "]\n"
		f.write(line)
	
