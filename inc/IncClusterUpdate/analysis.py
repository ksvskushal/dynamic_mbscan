import sys,os

dataset = str(sys.argv[1])
datasetS = str(sys.argv[2])
datasetI = str(sys.argv[3])


numOfRuns = 10
samplingFactor = str(sys.argv[4])
trees = str(sys.argv[5])

mu = str(sys.argv[6])
minpts = str(sys.argv[7])

'''
#Static Runs
os.system("rm -f "+ dataset + "_resultStatic")
os.system("g++ --std=c++0x mbScan.cpp")
for i in xrange(numOfRuns):
	os.system("./a.out " + dataset + " " + samplingFactor + " " + trees + " " + mu + " " + minpts + " >> " + dataset + "_resultStatic")
	os.system("mv "+"massCluster/massStatic"+ " massCluster/massStatic"+str(i))
	os.system("mv "+"massCluster/clusterStatic"+ " massCluster/clusterStatic"+str(i))
	print i

#SvS MM
os.system("g++ --std=c++0x mmCompare.cpp")
for i in xrange(numOfRuns):
	for j in xrange(i):
		os.system("./a.out " + " massCluster/massStatic"+str(i) + " " + " massCluster/massStatic"+str(j) + " " + mu  + " >> " + dataset + "_resultStatic")
		print "SvS MM", i,j


#SvS cId
os.system("g++ --std=c++0x cIdCompare.cpp")
for i in xrange(numOfRuns):
	for j in xrange(i):
		os.system("./a.out " + " massCluster/clusterStatic"+str(i) + " " + " massCluster/clusterStatic"+str(j) + " >> " + dataset + "_resultStatic")
		print "SvS cId", i,j


#SvG cId
os.system("g++ --std=c++0x cIdCompare.cpp")
for i in xrange(numOfRuns):
	os.system("./a.out " + " massCluster/clusterStatic"+str(i) + " " + dataset + "_compareCIdOrig" + " >> " + dataset + "_resultStatic")
	print "SvG cId", i


#compute avg time, space and accuracy over 10 runs for SvS
lines = open(dataset+"_resultStatic").readlines()
accuSvSMM = 0
accuSvScId = 0
accuSvGcId = 0
mmTimeSum = 0
tTimeSum = 0
spaceSum = 0

lineNum = 0
for i in xrange(numOfRuns):           #mmTime, tTime, ramUsed
	values = lines[lineNum].split()
	mmTimeSum += float(values[0])
	tTimeSum += float(values[1])
	spaceSum += float(values[2])
	lineNum += 1

for i in xrange(numOfRuns):
	for j in xrange(i):
		values = lines[lineNum].split()
		accuSvSMM += float(values[2])
		lineNum += 1


for i in xrange(numOfRuns):
	for j in xrange(i):
		values = lines[lineNum].split()
		accuSvScId += float(values[2])
		lineNum += 1

for i in xrange(numOfRuns):
	values = lines[lineNum].split()
	accuSvGcId += float(values[2])
	lineNum += 1


print 2*(accuSvSMM/(numOfRuns*(numOfRuns-1))) ,2*(accuSvScId/(numOfRuns*(numOfRuns-1))) , accuSvGcId/numOfRuns , mmTimeSum/numOfRuns, tTimeSum/numOfRuns, spaceSum/numOfRuns
'''

#---------------------------------------------------------------------IvS-----------------------------------------------------------------------------------#

#Inc Runs
os.system("rm -f "+ dataset + "_resultInc")
os.system("g++ --std=c++0x incMbScan.cpp")
for i in xrange(numOfRuns):
	os.system("./a.out " + datasetS + " " + datasetI+ " " + samplingFactor + " " + trees + " " + mu + " " + minpts + " >> " + dataset + "_resultInc")
	os.system("mv "+"massCluster/massInc"+ " massCluster/massInc"+str(i))
	os.system("mv "+"massCluster/clusterInc"+ " massCluster/clusterInc"+str(i))
	print i

#IvS MM
os.system("g++ --std=c++0x mmCompare.cpp")
for i in xrange(numOfRuns):
	for j in xrange(i):
		os.system("./a.out " + " massCluster/massInc"+str(i) + " " + " massCluster/massStatic"+str(j) + " " + mu  + " >> " + dataset + "_resultInc")
		print "IvS MM", i,j


#IvS cId
os.system("g++ --std=c++0x cIdCompare.cpp")
for i in xrange(numOfRuns):
	for j in xrange(i):
		os.system("./a.out " + " massCluster/clusterInc"+str(i) + " " + " massCluster/clusterStatic"+str(j) + " >> " + dataset + "_resultInc")
		print "IvS cId", i,j


#IvG cId
os.system("g++ --std=c++0x cIdCompare.cpp")
for i in xrange(numOfRuns):
	os.system("./a.out " + " massCluster/clusterInc"+str(i) + " " + dataset + "_compareCIdOrig" + " >> " + dataset + "_resultInc")
	print "IvG cId", i


#compute avg time, space and accuracy over 10 runs for SvS
lines = open(dataset+"_resultInc").readlines()
accuIvSMM = 0
accuIvScId = 0
accuIvGcId = 0
mmTimeSum = 0
tTimeSum = 0
spaceSum = 0

lineNum = 0
for i in xrange(numOfRuns):           #mmTime, tTime, ramUsed
	values = lines[lineNum].split()
	mmTimeSum += float(values[0])
	tTimeSum += float(values[1])
	spaceSum += float(values[2])
	lineNum += 1

for i in xrange(numOfRuns):
	for j in xrange(i):
		values = lines[lineNum].split()
		accuIvSMM += float(values[2])
		lineNum += 1


for i in xrange(numOfRuns):
	for j in xrange(i):
		values = lines[lineNum].split()
		accuIvScId += float(values[2])
		lineNum += 1

for i in xrange(numOfRuns):
	values = lines[lineNum].split()
	accuIvGcId += float(values[2])
	lineNum += 1


print 2*(accuIvSMM/(numOfRuns*(numOfRuns-1))) ,2*(accuIvScId/(numOfRuns*(numOfRuns-1))) , accuIvGcId/numOfRuns , mmTimeSum/numOfRuns, tTimeSum/numOfRuns, spaceSum/numOfRuns


#--------------------------------------------------------------IvI-----------------------------------------------------------------------------#

#for inc vs inc 
os.system("rm -f "+ dataset + "_resultIncInc")
os.system("g++ --std=c++0x incVsIncMbscan.cpp")
os.system("./a.out " + datasetS + " " + datasetI + " " + samplingFactor + " " + trees + " " + mu + " " + minpts)
	

#IvI MM
os.system("g++ --std=c++0x mmCompare.cpp")
for i in xrange(numOfRuns):
	for j in xrange(i):
		os.system("./a.out " + " massCluster/massIncInc"+str(i) + " " + " massCluster/massIncInc"+str(j) + " " + mu  + " >> " + dataset + "_resultIncInc")
		print "IvI MM", i,j


#IvI cId
os.system("g++ --std=c++0x cIdCompare.cpp")
for i in xrange(numOfRuns):
	for j in xrange(i):
		os.system("./a.out " + " massCluster/clusterIncInc"+str(i) + " " + " massCluster/clusterIncInc"+str(j) + " >> " + dataset + "_resultIncInc")
		print "IvI cId", i,j


#compute accuracy over 10 runs for IvI
lines = open(dataset+"_resultIncInc").readlines()
accuIvIMM = 0
accuIvIcId = 0


lineNum = 0
for i in xrange(numOfRuns):
	for j in xrange(i):
		values = lines[lineNum].split()
		accuIvIMM += float(values[2])
		lineNum += 1


for i in xrange(numOfRuns):
	for j in xrange(i):
		values = lines[lineNum].split()
		accuIvIcId += float(values[2])
		lineNum += 1



print 2*(accuIvIMM/(numOfRuns*(numOfRuns-1))) ,2*(accuIvIcId/(numOfRuns*(numOfRuns-1))) 





