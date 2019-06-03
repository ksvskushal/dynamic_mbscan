#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <map>
#include <cmath>
#include <cstring>
#include <cassert>
#include <string>
#include <queue>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <unordered_set> 
#include <unistd.h>
#include <ios>
#include <sstream>
#include <queue>
#include <sys/time.h>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include<sys/types.h>
#include <stdio.h>
#include <string.h>


using namespace std;

vector< vector<double> >data_matrix;
int nInst,nAttr;

int h_max;
int ntree;
int subSampleSize;
int maxNumOfNodes;

vector< vector<int> >splitAttribute;
vector< vector<double> >splitPoint;
vector< vector<int> >NodeSize;

vector< vector<int> >dataDistribution;

unordered_set<int > removedPoints;

vector< vector<int> >treeNode;
vector< vector<int> >treeNodeMass;
vector< vector<int> >LCA;
vector< vector<double> > massMatrix;

double maxValue, minValue;

void forestCreation();
void generateRandIndex();
void maxMinValue(int tree, int node);
void commonPathMatrixLCA();
void genericArrayEstimation();
void dissScore();

void printFiles(int i);

void decGenericArrayUpdate(int ins);
void decDissScore(int ins, string fname);

double mmTime;

int ramUsed;

int main(int argc, char* argv[]){

    string inFileBin = argv[1];
    string decFile = argv[2];
    double samplingFactor = atof(argv[3]);
    ntree = atoi(argv[4]);

    ifstream in(inFileBin.c_str()); 

    srand(time(NULL));
    
    if(!in){
      cout<<"Cannot open the input file: "<<inFileBin<<endl;
      exit(0);
    }     
    
    in >> nInst >> nAttr;

    subSampleSize = nInst * samplingFactor;
    h_max =  (int)(log10((double)(subSampleSize))/log10(2.0));

    data_matrix.resize(nInst);
        
    double temp;
    for(int i = 0; i < nInst; i++){

    	data_matrix[i].resize(nAttr);

      	for(int j = 0; j < nAttr; j++){
			in >> temp;
			data_matrix[i][j] = temp;
		}

        in >> temp;
    }

    commonPathMatrixLCA();

    clock_t tStart_iForest = clock();

    forestCreation();

    genericArrayEstimation();

    dissScore();

    ifstream inFile(decFile.c_str()); 

	if(!inFile){
		cout << "Cannot open the input file: " << decFile << endl;
		exit(0);
	}

	int insts,attrs;
	inFile >> insts >> attrs;

	vector<double> t;
	int ind;

	for(int i = 0; i < insts; i++){
	  	for(int j = 0; j < attrs; j++){
	    	inFile >> temp;
	    	t.push_back(temp);
	  	}

        in >> temp;

	  	ind = find(data_matrix.begin(), data_matrix.end(), t) - data_matrix.begin();

		removedPoints.insert(ind);

		t.clear();
	}

	vector<double>().swap(t);

   	decGenericArrayUpdate(insts);

    printFiles(2);

    mmTime = (double)(clock() - tStart_iForest)/CLOCKS_PER_SEC;

    cout << mmTime << " " << ramUsed << endl;

    splitAttribute.clear();
    splitPoint.clear();
    NodeSize.clear();
    treeNodeMass.clear();
    treeNode.clear();
    LCA.clear();
	massMatrix.clear();

    vector< vector<int> >().swap(splitAttribute);
    vector< vector<double> >().swap(splitPoint);
    vector< vector<int> >().swap(NodeSize);
    vector< vector<int> >().swap(treeNodeMass);
    vector< vector<int> >().swap(treeNode);
    vector< vector<int> >().swap(LCA);
    vector< vector<double> >().swap(massMatrix);

    return 0;  
}

void forestCreation(){

    maxNumOfNodes = (int) pow(2.0, (double)(h_max+1))-1;

    if(subSampleSize > nInst){
    	subSampleSize = nInst;
    }

    int tree = 0;
       
    splitAttribute.resize(ntree);
    splitPoint.resize(ntree);
    NodeSize.resize(ntree);
    
    double dataDiff;

    while(tree < ntree){
      	splitAttribute[tree].resize(maxNumOfNodes);

      	for(int i = 0; i < splitAttribute[tree].size(); i++){
        	splitAttribute[tree][i] = -1;
      	}

	    splitPoint[tree].resize(maxNumOfNodes);
      	dataDistribution.resize(maxNumOfNodes);
      	NodeSize[tree].resize(maxNumOfNodes);
	    NodeSize[tree][0] = subSampleSize;

      	generateRandIndex();
      
      	int heightNode;
      	for(int node = 0; node < maxNumOfNodes; node++){
          	heightNode = (int)(log2((double)(node+1)));

          	int leftChildIndex = 2*node + 1;
          	int rightChildIndex = 2*node + 2;

          	if(NodeSize[tree][node] <= 1 || heightNode == h_max){
				dataDistribution[node].clear();
	    		vector<int> ().swap(dataDistribution[node]);
            	continue;
          	}
          
          	splitAttribute[tree][node] = rand()%nAttr;

          	maxMinValue(tree, node);

          	dataDiff = maxValue-minValue;
          	splitPoint[tree][node] = minValue + ((double)rand()/RAND_MAX)*dataDiff;
          
         
          	for(int j = 0; j < dataDistribution[node].size(); j++){
	            if(data_matrix[dataDistribution[node][j]][splitAttribute[tree][node]] < splitPoint[tree][node]){
	            	dataDistribution[leftChildIndex].push_back(dataDistribution[node][j]);
	            }else{
	              dataDistribution[rightChildIndex].push_back(dataDistribution[node][j]);
	            }
          	}

          	NodeSize[tree][leftChildIndex] = dataDistribution[leftChildIndex].size();
          	NodeSize[tree][rightChildIndex] = dataDistribution[rightChildIndex].size();

	  		dataDistribution[node].clear(); 
	  		vector<int> ().swap(dataDistribution[node]);
      }
      
      tree++;    
    }

    dataDistribution.clear();
    vector< vector<int> > ().swap(dataDistribution);
}

void generateRandIndex(){
  
  	for(int i = 0; i < nInst; i++){
		dataDistribution[0].push_back(i);
  	}

  	random_shuffle(dataDistribution[0].begin(), dataDistribution[0].end());
  
  	dataDistribution[0].erase(dataDistribution[0].begin()+subSampleSize, dataDistribution[0].begin()+nInst);
}

void maxMinValue(int tree, int node){

	maxValue = -999999;
	minValue = 999999;

	for(int i = 0; i < dataDistribution[node].size(); i++){
		if(maxValue < data_matrix[dataDistribution[node][i]][splitAttribute[tree][node]]){
	  		maxValue = data_matrix[dataDistribution[node][i]][splitAttribute[tree][node]];
		}
		if(minValue > data_matrix[dataDistribution[node][i]][splitAttribute[tree][node]]){
	  		minValue = data_matrix[dataDistribution[node][i]][splitAttribute[tree][node]];
		}
  	}
}

void commonPathMatrixLCA(){

  	maxNumOfNodes = (int) pow(2.0, (double)(h_max+1))-1;

  	LCA.resize(maxNumOfNodes);
  
  	for(int i = 0; i < maxNumOfNodes; i++){
    	for(int j = 0; j <= i; j++){
	      	if(i == j){
	        	LCA[i].push_back(i);
	        	continue;
	      	}

      		int height_i = (int)(log10((double)(i+1))/log10((double)2)); 
      		int height_j = (int)(log10((double)(j+1))/log10((double)2)); 
      		int root_i = i;
      		int root_j = j;
      
	      	while (root_i != root_j){ 
	          	if(root_i > root_j){
	            	root_i = (root_i%2==0)?root_i/2 - 1: (root_i-1)/2;
	          	}else{
	            	root_j = (root_j%2==0)?root_j/2 - 1: (root_j-1)/2;
	          	}
	      	}

     		LCA[i].push_back(root_i);
    	}
    }
}

void genericArrayEstimation(){
	
	int maxNumOfNodes = (int) pow(2.0, (double)(h_max+1))-1;  

  	treeNode.resize(nInst);
  	for(int i = 0; i < nInst; i++){
    	treeNode[i].resize(ntree);
  	}

    treeNodeMass.resize(ntree);
    for(int j = 0; j < ntree; j++){
    	treeNodeMass[j].resize(maxNumOfNodes);
    }

  	int heightNode;

  	for(int tree = 0; tree < ntree; tree++){
		dataDistribution.resize(maxNumOfNodes);
		dataDistribution[0].resize(nInst);

    	for(int i = 0; i < nInst; i++){
      		dataDistribution[0][i] = i;
    	}  

    	for(int node = 0; node < maxNumOfNodes; node++){
      		heightNode = (int)(log2((double)(node+1)));

	       	if(dataDistribution[node].size() == 0 || NodeSize[tree][node] == 0){
	        	if(heightNode < h_max){
	          		dataDistribution[2*node+1].resize(0);
	          		dataDistribution[2*node+2].resize(0);
	        	}

				dataDistribution[node].clear();
	        	vector<int> ().swap(dataDistribution[node]);
	        	continue;
	      	}

	      	treeNodeMass[tree][node] = dataDistribution[node].size();
	      
	      	if(NodeSize[tree][node] == 1 || heightNode == h_max ){
	        	for(int i = 0; i < dataDistribution[node].size(); i++){
	          		treeNode[dataDistribution[node][i]][tree] = node;
	        	}

	        	if(heightNode < h_max){
	          		dataDistribution[2*node+1].resize(0);
	          		dataDistribution[2*node+2].resize(0);
	        	}
	      	}else{
	        	for(int j = 0; j < dataDistribution[node].size(); j++){
	          		if(data_matrix[dataDistribution[node][j]][splitAttribute[tree][node]] < splitPoint[tree][node]){
	            		dataDistribution[2*node+1].push_back(dataDistribution[node][j]);
	          		}else{
	            		dataDistribution[2*node+2].push_back(dataDistribution[node][j]);
	          		}
	        	}
	      	}

	      	dataDistribution[node].clear();
	      	vector<int> ().swap(dataDistribution[node]);
    	}

    	dataDistribution.clear();
    	vector< vector<int> > ().swap(dataDistribution);
  	}
}

void dissScore(){
  
  massMatrix.resize(nInst);  

  double tempMass;

  	for(int i = 0; i < nInst; i++){
      	for(int j = 0; j <= i; j++){
			tempMass = 0;
        	for(int tree = 0; tree < ntree; tree++){
			  	if(treeNode[i][tree] >= treeNode[j][tree]){
		        	tempMass += (double)treeNodeMass[tree][LCA[treeNode[i][tree]][treeNode[j][tree]]];
		     	}else{
			    	tempMass += (double)treeNodeMass[tree][LCA[treeNode[j][tree]][treeNode[i][tree]]];
			  	}
		    }

        	tempMass = tempMass/ntree;

			massMatrix[i].push_back(tempMass);
     	}
  	}
}

void printFiles(int i){
	// ofstream myfile;

	// string fname = "out/splitPoint_" + to_string(i) + ".txt";
	// myfile.open(fname);
	// for (int i = 0; i < ntree; ++i){
	// 	for (int j = 0; j < maxNumOfNodes; ++j){
	// 		myfile << splitPoint[i][j] << " ";
	// 	}

	// 	myfile << endl;
	// }
	// myfile.close();

	// fname = "out/treeNode_" + to_string(i) + ".txt";
	// myfile.open(fname);
	// for (int i = 0; i < nInst; ++i){
	// 	for (int j = 0; j < ntree; ++j){
	// 		myfile << treeNode[i][j] << " ";
	// 	}

	// 	myfile << endl;
	// }
	// myfile.close();

	// fname = "out/treeNodeMass_" + to_string(i) + ".txt";
	// myfile.open(fname);
	// for (int i = 0; i < ntree; ++i){
	// 	for (int j = 0; j < maxNumOfNodes; ++j){
	// 		myfile << treeNodeMass[i][j] << " ";
	// 	}

	// 	myfile << endl;
	// }
	// myfile.close();

	string fname = "out/massInc";

    ofstream myfile(fname,ios::out);
    myfile<<""; 
    myfile.close();


	myfile.open(fname);
	if(i == 1){
		for(int i = 0; i < nInst; ++i){
			for(int j = 0; j <= i; j++){
				myfile << massMatrix[i][j] << " ";
			}

			myfile << endl;
		}
	}else{
		for(int i = 0; i < nInst; ++i){
			if(removedPoints.find(i) != removedPoints.end()){
				continue;
			}

			for(int j = 0; j <= i; j++){
				if(removedPoints.find(j) != removedPoints.end()){
					continue;
				}

				myfile << i << " " << j << " " << massMatrix[i][j] << endl;
			}
		}
	}
	myfile.close();
}

void decGenericArrayUpdate(int insts){

  	int maxNumOfNodes = (int) pow(2.0, (double)(h_max+1))-1;

  	int heightNode;

  	int a, b;
  	double exRatio;

	vector<int> oldTreeNodeMass;

  	for(int tree = 0; tree < ntree; tree++){
  		for(int i = 0; i < maxNumOfNodes; i++){
	  		oldTreeNodeMass.push_back(treeNodeMass[tree][i]);
	    }

 		dataDistribution.resize(maxNumOfNodes);

		for(int i = 0; i < nInst; i++){
			if(removedPoints.find(i) != removedPoints.end()){
				continue;
			}
      		dataDistribution[0].push_back(i);
    	}

      	for(int node = 0; node < maxNumOfNodes; node++){
      		heightNode = (int)(log2((double)(node+1)));

      		if(dataDistribution[node].size() == oldTreeNodeMass[node]){
      			if(heightNode < h_max){
          			dataDistribution[2*node+1].resize(0);
          			dataDistribution[2*node+2].resize(0);
        		}

        		dataDistribution[node].clear();
      			continue;
      		}

      		treeNodeMass[tree][node] = dataDistribution[node].size();

      		if(dataDistribution[node].size() == 0){
      			if(heightNode < h_max){
          			dataDistribution[2*node+1].resize(0);
          			dataDistribution[2*node+2].resize(0);
        		}

        		dataDistribution[node].clear();
      			continue;
      		}

      		if(NodeSize[tree][node] == 1 || heightNode == h_max){
      			for(int l = 0; l < dataDistribution[node].size(); l++){
      				a = dataDistribution[node][l];
      				
            		for(int r = l; r < dataDistribution[node].size(); r++){
					   	b = dataDistribution[node][r];

                    	exRatio = (double)(oldTreeNodeMass[node] - treeNodeMass[tree][node])/ntree;

                	  	if(a >= b){
			   				massMatrix[a][b] -= exRatio;
						}else{
							massMatrix[b][a] -= exRatio;
						}
					}
				}

        		if(heightNode < h_max){
          			dataDistribution[2*node+1].resize(0);
          			dataDistribution[2*node+2].resize(0);
        		}
      		}else{
      			for(int j = 0; j < dataDistribution[node].size(); j++){
          			if(data_matrix[dataDistribution[node][j]][splitAttribute[tree][node]] < splitPoint[tree][node]){
            			dataDistribution[2*node+1].push_back(dataDistribution[node][j]);
          			}else{
            			dataDistribution[2*node+2].push_back(dataDistribution[node][j]);
          			}
        		}

      			for(int l = 0; l < dataDistribution[2*node+1].size(); l++){
      				a = dataDistribution[2*node+1][l];

            		for(int r = l; r < dataDistribution[2*node+2].size(); r++){
					   	b = dataDistribution[2*node+2][r];

					  	exRatio = (double)(oldTreeNodeMass[node] - treeNodeMass[tree][node])/ntree;

                	  	if(a >= b){
			   				massMatrix[a][b] -= exRatio;
						}else{
							massMatrix[b][a] -= exRatio;
						}
					}
       			}
      		}

      		dataDistribution[node].clear();
      	}

      	dataDistribution.clear();
    	oldTreeNodeMass.clear();
  	}
}
