#Import Statements
from __future__ import print_function
import os, shutil, sys, random
import subprocess
import fnmatch
from shutil import copyfile
from subprocess import Popen, PIPE

#--------------------#
# Define RR function #
#--------------------#

def roundRobin(operation, top, sec, third):
	global j	
	global name

	if(operation == 0): #create case
		try:	
			if((sec == -1) and (third == -1)): #top level case
				out = subprocess.check_output("fallocate -l 262144 ./dir"+str(top)+"/"+str(name)+".txt", shell=True)
				name = name + 1
				out = subprocess.check_output("fallocate -l 262144 ./dir"+str(top)+"/"+str(name)+".txt", shell=True)
				name = name + 1
			elif((sec != -1) and (third == -1)): #mid level case
				out = subprocess.check_output("fallocate -l 262144 ./dir"+str(top)+"/dir"+str(sec)+"/"+str(name)+".txt", shell=True)
				name = name + 1
				out = subprocess.check_output("fallocate -l 262144 ./dir"+str(top)+"/dir"+str(sec)+"/"+str(name)+".txt", shell=True)
				name = name + 1
			else: #third level case
			 	out = subprocess.check_output("fallocate -l 262144 ./dir"+str(top)+"/dir"+str(sec)+"/dir"+str(third)+"/"+str(name)+".txt", shell=True)
                                name = name + 1
                                out = subprocess.check_output("fallocate -l 262144 ./dir"+str(top)+"/dir"+str(sec)+"/dir"+str(third)+"/"+str(name)+".txt", shell=True)
                                name = name + 1		
		#except:
		except Exception as e:
			print(e)
			print("DONE!\n")
			sys.exit()

	elif(operation == 1): #truncate case
		if((sec == -1) and (third == -1)): #top level case
			filename = random.choice(fnmatch.filter(os.listdir("./dir"+str(top)), "*.txt"))
			filesize = os.path.getsize("./dir"+str(top)+"/"+filename)
	                f=open("./dir"+str(top)+"/"+filename,"w+")
        	        f.truncate(filesize/2)
                	f.close()
		elif((sec != -1) and (third == -1)): #mid level case
                        filename = random.choice(fnmatch.filter(os.listdir("./dir"+str(top)+"/dir"+str(sec)), "*.txt"))
                        filesize = os.path.getsize("./dir"+str(top)+"/dir"+str(sec)+"/"+filename)
                        f=open("./dir"+str(top)+"/dir"+str(sec)+"/"+filename,"w+")
                        f.truncate(filesize/2)
                        f.close()
		else: #third level case
                        filename = random.choice(fnmatch.filter(os.listdir("./dir"+str(top)+"/dir"+str(sec)+"/dir"+str(third)), "*.txt"))
                        filesize = os.path.getsize("./dir"+str(top)+"/dir"+str(sec)+"/dir"+str(third)+"/"+filename)
                        f=open("./dir"+str(top)+"/dir"+str(sec)+"/dir"+str(third)+"/"+filename,"w+")
                        f.truncate(filesize/2)
                        f.close()

	elif(operation == 2): #delete case
		if((sec == -1) and (third == -1)): #top level case
                        filename = random.choice(fnmatch.filter(os.listdir("./dir"+str(top)), "*.txt"))
			os.remove("./dir"+str(top)+"/"+filename)
 		elif((sec != -1) and (third == -1)): #mid level case
                	filename = random.choice(fnmatch.filter(os.listdir("./dir"+str(top)+"/dir"+str(sec)), "*.txt"))
			os.remove("./dir"+str(top)+"/dir"+str(sec)+"/"+filename)
		else: #third level case
                        filename = random.choice(fnmatch.filter(os.listdir("./dir"+str(top)+"/dir"+str(sec)+"/dir"+str(third)), "*.txt"))
			os.remove("./dir"+str(top)+"/dir"+str(sec)+"/dir"+str(third)+"/"+filename)

	elif(operation == 3): #add case
		try:
			if((sec == -1) and (third == -1)): #top level case
				filename = random.choice(fnmatch.filter(os.listdir("./dir"+str(top)), "*.txt"))
				f=open("./dir"+str(top)+"/"+filename,"a+")
				for i in range(131072): #append 256kb to the file
					f.write("z")
				f.close()
			elif((sec != -1) and (third == -1)): #mid level case
				filename = random.choice(fnmatch.filter(os.listdir("./dir"+str(top)+"/dir"+str(sec)), "*.txt"))                
				f=open("./dir"+str(top)+"/dir"+str(sec)+"/"+filename,"a+")
				for i in range(131072): #append 256kb to the file
					f.write("z")
				f.close()
			else:
				filename = random.choice(fnmatch.filter(os.listdir("./dir"+str(top)+"/dir"+str(sec)+"/dir"+str(third)), "*.txt"))
				f=open("./dir"+str(top)+"/dir"+str(sec)+"/dir"+str(third)+"/"+filename,"a+")
				for i in range(131072): #append 256kb to the file
					f.write("z")
				f.close()
		#except:
		except Exception as e:
                	print(e)
			print("DONE!\n")
                        sys.exit()


	j = j + 1
        if(j == 4):
        	j = 0

###############################################
#         END ROUND ROBIN FUNCTION            #
###############################################



#
#---------Change to the correct directory--------
#
if (len(sys.argv) == 2):
        os.chdir(sys.argv[1])
#
#--------------
#

NUM_OF_TOP_LEVEL_FOLDERS = 100
NUM_OF_SECOND_LEVEL_FOLDERS = 40
NUM_OF_THIRD_LEVEL_FOLDERS = 30

#^^^ can be changed to at the user's discretion

#INITIAL CREATES
try:
	for i in range (NUM_OF_TOP_LEVEL_FOLDERS): 

		sys.stdout.write('\rCreating top level directory #'+str(i)+"...")
		sys.stdout.flush()

		os.mkdir("./dir"+str(i))	
		for k in range(2):
			output = subprocess.check_output("fallocate -l 262144 ./dir"+str(i)+"/"+str(k)+".txt", shell=True)		

		for k in range(NUM_OF_SECOND_LEVEL_FOLDERS):
			os.mkdir("./dir"+str(i)+"/dir"+str(k))
			for m in range(2):
				output = subprocess.check_output("fallocate -l 262144 ./dir"+str(i)+"/dir"+str(k)+"/"+str(m)+".txt", shell=True)

			for m in range(NUM_OF_THIRD_LEVEL_FOLDERS):
				os.mkdir("./dir"+str(i)+"/dir"+str(k)+"/dir"+str(m))
				for n in range(2):
					output = subprocess.check_output("fallocate -l 262144 ./dir"+str(i)+"/dir"+str(k)+"/dir"+str(m)+"/"+str(n)+".txt", shell=True)

except:
	print("---DEBUG INFO --- Out of space on initial create...\n")

print("\nFinshed making top level directories!\n")

#
#--ROUND ROBIN------------
#
whileRobinCounter = 0
name = 5
j = 0
while(1):
	stdout = Popen('df -h | grep /dev/sdb3', shell=True, stdout=PIPE).stdout
	output = stdout.read()
	first=output.split()
	sys.stdout.write('\rWorking...[|] Space used:  '+ first[2])
        sys.stdout.flush()

	j = whileRobinCounter

   for i in range (NUM_OF_TOP_LEVEL_FOLDERS):
		
		if(i<(NUM_OF_TOP_LEVEL_FOLDERS/4)):
			sys.stdout.write('\rWorking...[|]   Top level folder#'+str(i)+'   Space used:  '+ first[2])
        		sys.stdout.flush()
		elif(i<((NUM_OF_TOP_LEVEL_FOLDERS/4)*2)):
			sys.stdout.write('\rWorking...[/]   Top level folder#'+str(i)+'   Space used:  '+ first[2])
        		sys.stdout.flush()
		elif(i<((NUM_OF_TOP_LEVEL_FOLDERS/4)*3)):
			sys.stdout.write('\rWorking...[-]   Top level folder#'+str(i)+'   Space used:  '+ first[2])
        		sys.stdout.flush()
		else:
			sys.stdout.write('\rWorking...[\]   Top level folder#'+str(i)+'   Space used:  '+ first[2])
		        sys.stdout.flush()

		roundRobin(j,i,-1,-1)	

		for o in range(NUM_OF_SECOND_LEVEL_FOLDERS):
			roundRobin(j,i,o,-1)

			for p in range(NUM_OF_THIRD_LEVEL_FOLDERS):
				roundRobin(j,i,o,p)	

	whileRobinCounter = whileRobinCounter + 1
	if(whileRobinCounter==4):
		whileRobinCounter = 0

