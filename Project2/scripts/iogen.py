import subprocess
import os
import shutil

print "======================================="
print "I/O gen script for hw2, group 12-08"
print "======================================="

if (os.path.isdir("./testdir")):
	shutil.rmtree("testdir")

os.mkdir("testdir")
os.chdir("testdir")

for i in range(0, 9):
	if (os.path.exists("test" + str(i))):
		shutil.rmtree("test" + str(i))

	os.mkdir("test" + str(i))

for i in range(0,9):
	os.chdir("test" + str(i)) 
	for j in range (0,9):
		tmpfile = open("test"+str(j)+".txt","w")
		tmpfile.write("Isaac is the best! ")
		tmpfile.close()
	os.chdir("..")


#os.chdir("..")
#shutil.rmtree("testdir")

