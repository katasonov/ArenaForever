import os
import re
import datetime

mypath = os.path.dirname(os.path.abspath(__file__)) + '/'

with open(mypath + '../src/App.cpp', 'r') as f:
    content = f.readlines()
# you may also want to remove whitespace characters like `\n` at the end of each line

with open(mypath + '../src/App.cpp', 'w') as f:
	for line in content:
		if "timestamp:=[" in line:
			line = "//timestamp:=[" + str(datetime.datetime.now()) + "]\n"
		f.write(line)