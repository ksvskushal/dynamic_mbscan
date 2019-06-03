import matplotlib.pyplot as plt
import numpy as np

plt.close('all')

def datasetVsAccuracyWithGold():
	staticVSgoldCid = [0.9216,0.9362,0.8342,0.7955,0.8666,0.9822,0.9712]
	incVSgoldCid = [0.933,0.9435,0.853,0.7715,0.8766,0.9952,0.9939]
	datasets = ["Aggregation", "D31" , "Iris", "MovememntLibras" ,"Segment" ,"S1", "S2"]

	bar_width = 0.3
	index = np.arange(len(datasets))+0.5

	plt.bar(index,staticVSgoldCid,bar_width,color='b', label='Static vs Gold')
	plt.bar(index+bar_width,incVSgoldCid,bar_width,color='r', label='Incremental vs Gold')


	plt.xlabel("Datasets")
	plt.ylabel("Accuracy")
	plt.title('Accuracy of Clustering vs Datasets')
	plt.ylim(0.6,1)
	plt.xticks(index + bar_width, datasets)
	plt.legend(loc=9)

	plt.show()

def datasetVsAccuracyWithEachother():
	staticVSstaticMass = [0.9914,0.9964,0.9881,0.9962,0.9938,0.9981,0.9897]
	incVSstaticMass = [0.9912,0.9965,0.9639,0.9957,0.9937,0.9981,0.9859]
	incVsIncMass = [0.9999,0.9999,0.9999,1,0.9999,1,1]
	datasets = ["Aggregation", "D31" , "Iris", "MovememntLibras" ,"Segment" ,"S1", "S2"]

	bar_width = 0.25
	index = np.arange(len(datasets))+0.5

	plt.bar(index,staticVSstaticMass,bar_width,color='b', label='Static vs Static Mass matrix accuracy')
	plt.bar(index+bar_width,incVSstaticMass,bar_width,color='r', label='Incremental vs Static mass matrix accuracy')
	plt.bar(index+2*bar_width,incVsIncMass,bar_width,color='y', label='Incremental vs Incremental mass matrix accuracy')


	plt.xlabel("Dataset")
	plt.ylabel("Accuracy")
	plt.title('Accuracy of mass matrix vs datasets')
	plt.xticks(index + 1.5*bar_width, datasets)
	plt.legend(loc=0)

	plt.show()

def datasetVsTimeSpace():
	incmmTimespeedupRatio = [2.6628122109,2.8896360759,2.5106382979,2.9763837638,2.846645544,3.2618395004,2.9603174603]
	inctTimespeedupRatio = [2.6609673444,2.8891594855,2.498489426,2.972733972,2.8458889605,3.2613558324,2.9585053226]
	incSpaceUseRatio = [1.9795918367,5.7281553398,1.1315789474,1.2954545455,3.875,7.7763371151,1.9166666667]

	datasets = ["Aggregation", "D31" , "Iris", "MovememntLibras" ,"Segment" ,"S1", "S2"]

	bar_width = 0.25
	index = np.arange(len(datasets))+0.5

	plt.bar(index,incmmTimespeedupRatio,bar_width,color='b', label='Mass Matrix Computation Speedup')
	plt.bar(index+bar_width,inctTimespeedupRatio,bar_width,color='r', label='Cluster Computation Speedup')
	plt.bar(index+2*bar_width,incSpaceUseRatio,bar_width,color='y', label='Extra Memory Used')


	plt.xlabel("Datasets")
	plt.ylabel("Speedup")
	plt.title('Speedup vs Datasets')
	plt.xticks(index + 1.5*bar_width, datasets)
	plt.legend(loc=9)

	plt.show()

def incPercentageVsspeedup():
	D31 = [3.6282753014,3.2397734424,3.342419419,2.8896360759,2.5884813262,1.9546079981,1.5021958122]
	S1 = [4.0546776815,3.7885693265,3.6568160331,3.2618395004,2.6643408258,2.06742309,1.5776942279]

	incPercentage = [1,2,3,5,10,20,40]

	index = np.arange(len(incPercentage))+0.25

	plt.plot(index, D31, 'b', marker='o',label='D31')
	plt.plot(index, S1, 'r', marker='^', label='S1')

	plt.xlabel("Incremental Percentage")
	plt.ylabel("Speedup")
	plt.title('Speedup vs Incremental Percentage')
	plt.xticks(index,incPercentage)
	plt.ylim(0,5)
	plt.legend(loc=0)

	plt.show()

def incPercentageVsAccuracy():
	svgD31 = [0.929,0.9392,0.943,0.9362,0.9468,0.944,0.9405]
	ivgD31 = [0.9473,0.9415,0.9379,0.9435,0.9473,0.9476,0.8837]
	iviD31 = [1,1,1,1,1,1,1]

	svgS1 = [0.9946,0.9949,0.9949,0.9822,0.9947,0.9949,0.9949]
	ivgS1 = [0.9943,0.9949,0.995,0.9952,0.9951,0.9948,0.9943]
	iviS1 = [1,1,1,1,0.999,1,1]

	incPercentage = [1,2,3,5,10,20,40]

	index = np.arange(len(incPercentage))+0.25

	plt.plot(index, svgD31, 'b', marker='o',label='Static vs Gold (D31)')
	plt.plot(index, ivgD31, 'b', marker='^',label='Incremental vs Gold (D31)')
	plt.plot(index, iviD31, 'b', marker='s',label='Incremental vs Incremental (D31)')
	plt.plot(index, svgS1, 'r', marker='o', label='Static vs Gold (S1)')
	plt.plot(index, ivgS1, 'r', marker='^', label='Incremental vs Gold (S1)')
	plt.plot(index, iviS1, 'r', marker='>', label='Incremental vs Incremental (S1)')

	plt.xlabel("Incremental Percentage")
	plt.ylabel("Accuracy")
	plt.title('Accuracy vs Incremental Percentage')
	plt.xticks(index,incPercentage)
	plt.ylim(0.85,1.02)
	plt.legend(loc=0)

	plt.show()

def regionVsTime():
	ratioS1 = [0.5539792388,0.6349233743,0.7325501037,0.7177000346]
	ratioS2 = [0.7010729409,0.7708732899,0.8285714286,0.8593900482]

	numberOfRegions = [1,2,3,4]

	index = np.arange(len(numberOfRegions))

	plt.plot(index, ratioS1, 'b', marker='o',label='S1')
	plt.plot(index, ratioS2, 'r', marker='^', label='S2')

	plt.xlabel("Number of regions for incremental data")
	plt.ylabel("Ratio of Inc time with static time")
	plt.title('Time ratio vs number of regions')
	plt.xticks(index,numberOfRegions)
	plt.ylim(0,1)
	plt.legend(loc=0)

	plt.show()

# datasetVsAccuracyWithGold()
# datasetVsTimeSpace()
# incPercentageVsspeedup()
incPercentageVsAccuracy()