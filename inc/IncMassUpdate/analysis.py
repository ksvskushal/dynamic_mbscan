import sys,os

dataset = str(sys.argv[1])
datasetS = str(sys.argv[2])
datasetI = str(sys.argv[3])


numOfRuns = 10
samplingFactor = str(sys.argv[4])
trees = str(sys.argv[5])

maxError = str(sys.argv[6])



#Static Runs
os.system("rm -f "+ dataset + "_resultOrig")
os.system("g++ --std=c++0x massMatrix.cpp")
for i in xrange(numOfRuns):
	os.system("./a.out " + dataset + " " + samplingFactor + " " + trees + " >> " + dataset + "_resultOrig")
	os.system("mv "+"massMatrix/massOrig"+ " massMatrix/massOrig"+str(i))
	print i


os.system("g++ --std=c++0x mmCompare.cpp")
for i in xrange(numOfRuns):
	for j in xrange(i):
		os.system("./a.out " + " massMatrix/massOrig"+str(i) + " " + " massMatrix/massOrig"+str(j) + " " + maxError  + " >> " + dataset + "_resultOrig")
		print i,j


#compute avg time and accuracy over 10 runs for orig method
lines = open(dataset+"_resultOrig").readlines()
accuSum = 0
timeSum = 0
spaceSum = 0
for i in xrange(numOfRuns):
	values = lines[i].split()
	timeSum += float(values[0])
	spaceSum += float(values[1])

for line in lines[numOfRuns:]:
	values = line.split()
	accuSum += float(values[2])
	

print 2*(accuSum/(numOfRuns*(numOfRuns-1))) , timeSum/numOfRuns, spaceSum/numOfRuns




#Inc vs static  Runs

os.system("rm -f "+ dataset + "_resultInc")
os.system("g++ --std=c++0x incMassMatrix.cpp")
for i in xrange(numOfRuns):
	os.system("./a.out " + datasetS + " " + datasetI + " " + samplingFactor + " " + trees + " >> " + dataset + "_resultInc")
	os.system("mv "+"massMatrix/massInc"+ " massMatrix/massInc"+str(i))
	print i


print ("Inc Vs Static accuracy")
os.system("g++ --std=c++0x mmCompare.cpp")
for i in xrange(numOfRuns):
	for j in xrange(i):
		os.system("./a.out " + " massMatrix/massInc"+str(i) + " " + " massMatrix/massOrig"+str(j) + " " + maxError  + " >> " + dataset + "_resultInc")
		print i,j


#compute avg time  over 10 runs for inc method and accuracy of inc vs static
lines = open(dataset+"_resultInc").readlines()
accuSum = 0
timeSum = 0
spaceSum = 0
for i in xrange(numOfRuns):
	values = lines[i].split()
	timeSum += float(values[0])
	spaceSum += float(values[1])

for line in lines[numOfRuns:]:
	values = line.split()
	accuSum += float(values[2])
	

print 2*(accuSum/(numOfRuns*(numOfRuns-1))) , timeSum/numOfRuns, spaceSum/numOfRuns



'''

#for inc vs inc 
os.system("rm -f "+ dataset + "_resultInc")
os.system("g++ --std=c++0x incvsincMM.cpp")
os.system("./a.out " + datasetS + " " + datasetI + " " + samplingFactor + " " + trees)
	

print ("Inc Vs Inc accuracy")
os.system("g++ --std=c++0x mmCompare.cpp")
for i in xrange(numOfRuns):
	for j in xrange(i):
		os.system("./a.out " + " massMatrix/massIncInc"+str(i) + " " + " massMatrix/massIncInc"+str(j) + " " + maxError  + " >> " + dataset + "_resultInc")
		print i,j

#prints F measure value of Static w.r.t. Orig Cluster and time till mass matrix
lines = open(dataset+"_resultInc").readlines()
accuSum = 0
for line in lines:
	values = line.split()
	accuSum += float(values[2])
	
print 2*(accuSum/(numOfRuns*(numOfRuns-1)))

'''



