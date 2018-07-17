#Import Statements

from __future__ import print_function
import os, shutil, sys, random
import subprocess
import fnmatch
from shutil import copyfile
from subprocess import Popen, PIPE


if (len(sys.argv) == 1):
	print("\nUsage: python holePuncher.py /path/where/you/want/aging/to/happen\n")
	exit()

if (len(sys.argv) > 1):
        os.chdir(sys.argv[1])


deviceInfo = subprocess.check_output("mount | grep "+sys.argv[1],shell=True)
device = deviceInfo.split()

sizeInfo = subprocess.check_output("df -B1 | grep "+device[0],shell=True)
sizeList = sizeInfo.split()

size = sizeList[1]

chunk = int((float(size) * 0.8)/float(4))
size = int(float(size) * 0.81) 

offset1 = 2049
offset2 = offset1 + chunk + 2049
offset3 = offset2 + chunk + 2049
offset4 = offset3 + chunk + 2049

last = size - offset4

for i in range(1000000):
	sys.stdout.write('\rCreating file #'+str(i)+"...")
        sys.stdout.flush()
	try:
		out = subprocess.check_output("fallocate -l "+str(size) + " "+str(i),shell=True)
		out = subprocess.check_output("fallocate -p --offset "+str(offset1)+" --length "+str(chunk)+" "+str(i),shell=True)
		out = subprocess.check_output("fallocate -p --offset "+str(offset2)+" --length "+str(chunk)+" "+str(i),shell=True)
		out = subprocess.check_output("fallocate -p --offset "+str(offset3)+" --length "+str(chunk)+" "+str(i),shell=True)
		out = subprocess.check_output("fallocate -p --offset "+str(offset4)+" --length "+str(last)+" "+str(i),shell=True)
	except Exception as e:
			print(e)
			print("\nDONE!\n")
			sys.exit()

print("Done!\n")

