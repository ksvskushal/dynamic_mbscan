import matplotlib.pyplot as plt
import numpy as np

plt.close('all')

def datasetVsAccuracy():
	staticVSstatic = [0.6126, 0.6159 ,0.7604 ,0.6498 ,0.6392, 0.6451, 0.6317]
	staticVSinc = [0.6407, 0.6136, 0.6716, 0.6011, 0.6322, 0.6554, 0.6274]
	incVSinc = [1 ,1 ,0.9999987 ,1 ,1 , 1, 1]
	datasets = ["Aggregation", "D31" , "Iris", "MovememntLibras" ,"Segment" ,"S1", "S2"]

	bar_width = 0.3
	index = np.arange(len(datasets))

	plt.bar(index,staticVSstatic,bar_width,color='b', label='Static vs Static')
	plt.bar(index+bar_width,staticVSinc,bar_width,color='r', label='Static vs Incremental')
	plt.bar(index+(2*bar_width),incVSinc,bar_width,color='y',label='Incremental vs Incremental')


	plt.xlabel("Datasets")
	plt.ylabel("Accuracy")
	plt.title('Accuracy vs Datasets')
	plt.xticks(index + bar_width, datasets)
	plt.ylim(0,1.02)
	plt.legend(loc=0)

	plt.show()

def datasetVsTimeSpace():
	incTimespeedupRatio = [1.2876516774,1.0639431616,1.6369565217,1.5672,1.1002031144,1.0654148415,1.3933397683]
	incSpaceUseRatio = [1.7291666667,5.4,1,1.1590909091,3.5,8.1247892074,1.6949152542]

	datasets = ["Aggregation", "D31" , "Iris", "MovememntLibras" ,"Segment" ,"S1", "S2"]

	bar_width = 0.3
	index = np.arange(len(datasets))+0.25

	plt.bar(index,incTimespeedupRatio,bar_width,color='b', label='Speedup')
	plt.bar(index+bar_width,incSpaceUseRatio,bar_width,color='r', label='Extra Memory Used')


	plt.xlabel("Datasets")
	plt.ylabel("Speedup")
	plt.title('Speedup vs Datasets')
	plt.xticks(index + bar_width, datasets)
	plt.legend(loc=9)

	plt.show()

def errorVsAccuracy():
	svsD31 = [0.183,0.3148,0.448,0.6159,0.8624,0.972]
	svsS1 = [0.1932,0.35,0.4637,0.6317,0.8845,0.9714]
	sviD31 = [0.17,0.339,0.459,0.6136,0.8418,0.9711]
	sviS1 = [0.2079,0.37,0.4284,0.6274,0.8856,0.9716]
	iviD31 = [1,1,1,0.998,1,1]
	iviS1 = [1,1,1,1,1,1]

	errors = [1,2,3,5,10,20]

	index = np.arange(len(errors))

	plt.plot(index, svsD31, 'b', marker='o', label='Static vs Static (D31)')
	plt.plot(index, svsS1, 'r', marker='o', label='Static vs Static (S1)')
	plt.plot(index, sviD31, 'b', marker='^', label='Static vs Incremental (D31)')
	plt.plot(index, sviS1, 'r', marker='^', label='Static vs Incremental (S1)')
	plt.plot(index, iviD31, 'b', marker='s', label='Incremental vs Incremental (D31)')
	plt.plot(index, iviS1, 'r',marker='s', label='Incremental vs Incremental (S1)')

	plt.xlabel("Error Percentage")
	plt.ylabel("Accuracy")
	plt.title('Accuracy vs Error Percentage')
	plt.xticks(index,errors)
	plt.legend(loc=0)
	plt.ylim(0,1.02)

	plt.show()

def incPercentageVsAccuracy():
	svs = [0.5822,0.6664,0.5572,0.6317,0.6028,0.6265]
	svi = [0.5938,0.6703,0.606,0.6274,0.6065,0.6367]
	ivi = [1,1,1,1,1,1]

	errors = [1,2,3,5,10,20]

	index = np.arange(len(errors))

	plt.plot(index, svs, 'b', marker='o',label='Static vs Static')
	plt.plot(index, svi, 'r', marker='^', label='Static vs Incremental')
	plt.plot(index, ivi, 'g', marker='s', label='Incremental vs Incremental')

	plt.xlabel("Incremental Percentage")
	plt.ylabel("Accuracy")
	plt.title('Accuracy vs Incremntal Percentage')
	plt.xticks(index,errors)
	plt.ylim(0,1.02)
	plt.legend(loc=4)

	plt.show()

def regionVsTime():
	ratioS1 = [1.4263851045,1.2972300547,1.2068965517,1.163615988]
	ratioS2 = [1.8051217989,1.5749932047,1.3650943396,1.3933397683]

	numberOfRegions = [1,2,3,4]

	index = np.arange(len(numberOfRegions))+0.25

	plt.plot(index, ratioS1, 'b', marker='o',label='S1')
	plt.plot(index, ratioS2, 'r', marker='^', label='S2')

	plt.xlabel("Number of Regions for Incremental Data")
	plt.ylabel("Speedup")
	plt.title('Speedup vs Number of Regions')
	plt.xticks(index,numberOfRegions)
	plt.ylim(0,2)
	plt.legend(loc=0)

	plt.show()

def incVsTime():
	speedupS1 = [0.9909027504,1.0203904016,1.0204208691,1.3933397683,1.1673619458,1.2308590806]
	incPercentage = [1,2,3,5,10,20]

	index = np.arange(len(incPercentage))+0.25

	plt.plot(index, speedupS1, 'b', marker='o',label='S1')

	plt.xlabel("Incremental Percentage")
	plt.ylabel("Speedup")
	plt.title('Speedup vs Incremental Percentage')
	plt.xticks(index,incPercentage)
	plt.ylim(0,2)
	plt.legend(loc=0)

	plt.show()

# datasetVsAccuracy()
# datasetVsTimeSpace()
# regionVsTime()
# incVsTime()
# errorVsAccuracy()
incPercentageVsAccuracy()