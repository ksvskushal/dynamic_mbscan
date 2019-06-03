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
#include <unistd.h>
#include <ios>
#include <sstream>
#include <queue>
#include <sys/time.h>
#include <malloc.h>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include<sys/types.h>
#include<sys/sysinfo.h>
#include <stdio.h>
#include <string.h>
#include<bits/stdc++.h>


using namespace std;


//struct sysinfo meminfo;


vector< vector<double> >data_matrix;
int nInst,nAttr;

//int currRowIndex=0;   //keep tracks of global number of rows already stored in tree
int h_max;
int ntree;        //total number of trees to be formed
int subSampleSize;
int maxNumOfNodes;

vector< vector<int> >splitAttribute;   //(i,j) stores the randmly generated split attribute for node j of tree i
vector< vector<double> >splitPoint;
vector< vector<int> >NodeSize;

//vector< vector<int> > sampledIndices;
vector< vector<int> >dataDistribution;   //stores indices of each node in a tree
vector< vector<vector<int> > >  sampleLeafElements;   //stores elements present in leaf node having height < max_height in forestCreation method
vector< vector<vector<int> > >  leafElements;     //stores elements present in leaf node having height < max_height in genericArrayEstimation method

vector< vector<int> >treeNode;       //(i,j) cell indicates node number of min region where instance i lies in tree j otherwise 0.
vector< vector<int> >treeNodeMass;   //(i,j) cell indicates count of node j in tree i.
vector< vector<int> >LCA;            //(i,j) cell indicates lowest common ancestor node number of node i and node j.
vector< vector<double> > massMatrix;     //(i,j) cell indicates the dissimilarity value of two instances i and j.

double maxValue, minValue;

//for MBSCAn code
vector<int> clusterId;
#define UNCLASSIFIED -1   //Any non-negative value identifies the cluster to which the data point belongs.
#define NOISE -2
#define CORE_POINT 1
#define NON_CORE_POINT 0
double mu;
int minpts;  
double maxIncRatio = 1.1;   //maximum 10% additional points can be added in incremental
vector< vector<int> > muNeighbor;  //each row contains the point belonging to Mu(row);
vector< vector<int> > NodeMassPairsList; 

void forestCreation();
void generateRandIndex();
void maxMinValue(int tree, int node);
void commonPathMatrixLCA();
void genericArrayEstimation();
double dissScore(int a, int b, int t);    //returns mass value of inst a and inst b except from tree t in order of number of itrees.

void muNeighborUpdate(int n);
void mbscanImpl(int n);
int expandCluster(int i, int cId);
void spreadCluster(int i, int index,int cId);
void writeCluster(int n, string s);


void incForestUpdate(int ins, int a ,double samplingFactor);
void incGenericArrayUpdate(int ins);
////void incDissScore(int ins);
void incMuNeighborUpdate(int insts);
//int posNeighElement(int l, int r);    //returns index of element r in l^th row of muNeighbor matrix

void fMeasureValue(int n, string inFile);
void preProcessNMPL(int n);

int parseLine(char *line);
int getValue(int );


double delta = 0;          //threshold to decide whether to update mass value of its child instance or not
double lowerLimit, upperLimit;
int nCluster;
double mmTime,tTime;  //time to update mass matrix and total inc time in sec

int ramUsed;


int main(int argc, char* argv[]){
    string inFileBin = argv[1];   //static input data
    string incFile = argv[2];     //inc input data
    double samplingFactor = atof(argv[3]);
    ntree = atoi(argv[4]);
    mu = (double) atof(argv[5]);
    //mu = mu*maxIncRatio;
    minpts = atoi(argv[6]);
   

    srand(time(NULL));

    ramUsed = max(getValue(1), ramUsed);          //-------------1---------------------//
    /*int memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/

    //storing data into data_matrix 
    ifstream in(inFileBin.c_str()); 
    if(!in)
    {
      cout<<"Cannot open the input file: "<<inFileBin<<endl;
      exit(0);
    }     
    
    in >> nInst >> nAttr;

    subSampleSize = nInst * samplingFactor;
    h_max =  (int)(log10((double)(subSampleSize))/log10(2.0));

    data_matrix.resize(nInst);
    //for(int i=0;i<nInst;i++)
    //  data_matrix[i].resize(nAttr); 
        
    //Read the rest of the values from the input file
    double temp;
    for(int i=0;i<nInst;i++)
    {
      data_matrix[i].resize(nAttr);
      for(int j=0;j<nAttr;j++)
      {
        in >> temp;
        data_matrix[i][j] = temp;
      }

      in >> temp;  //comment it if dataset doesn't contain class label
    }


    //sysinfo(&meminfo);
    ramUsed = max(getValue(1), ramUsed);        //------------2-----------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/
  

    /*for(int i=0;i<nInst;i++){
      for(int j=0;j<nAttr;j++){
        cout << data_matrix[i][j] << " ";
      }
      cout << endl;
    }*/

    /*********** Data stored in data_matrix *********/

    //cout<<"Rows:"<<nInst<< "  " << data_matrix[0][1] <<endl;

    /* updates LCA matrix */
    commonPathMatrixLCA();

    //cout << LCA[3][2] << endl;

  
    //lowerLimit = (double)(maxIncRatio - 1)/maxIncRatio;
    //cout << lowerLimit << endl;


    //clock_t tStart_iForest = clock();
    forestCreation();
    /*********** iForest Created *********/


    ramUsed = max(getValue(1), ramUsed);            //----------------3-----------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/

    /* updates treeNode, treeNodeMass  */
    genericArrayEstimation();
    


    ramUsed = max(getValue(1), ramUsed);               //---------------4--------------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " " << memUsed << endl;*/


    /*updates massMatrix */
    //dissScore();
    //clock_t tEnd_iForest = clock();
    //cout<<"time till static mass matrix: "<<(double)(tEnd_iForest - tStart_iForest)/CLOCKS_PER_SEC<<" sec"<<endl;

  
    //ramUsed = max(getValue(1), ramUsed);               //------------------5---------------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/


    /*updates muNeighbor matrix*/
    //clock_t tStart_muNeigh = clock();
    muNeighborUpdate(nInst);
    //clock_t tEnd_muNeigh = clock();
    //double extraT = (double)(tEnd_muNeigh - tStart_muNeigh)/CLOCKS_PER_SEC;


   


    ramUsed = max(getValue(1), ramUsed);           //------------------6--------------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/


    //upadtes clusterId matrix
    mbscanImpl(nInst);
    //cout<<"total running time static: "<<(double)(clock() - tStart_iForest)/CLOCKS_PER_SEC<<" sec"<<endl;

    //writeCluster(nInst, "finalClusterStatic");

    ramUsed = max(getValue(1), ramUsed);             //------------------7------------------------//
    /*int memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/

    /*---------------Incremental MBSCAN Part -------------*/

  
  ifstream inFile(incFile.c_str()); 
  if(!inFile){
    cout << "Cannot open the input file: " << incFile << endl;
    exit(0);
  }
  int insts,attrs;
  inFile >> insts >> attrs;
  //cout << insts << " " << attrs << endl;
  //inserting new instances into data_matrix
  vector<double> t;
  for(int i=nInst;i<nInst+insts;i++){
      for(int j=0;j<attrs;j++)
      {
        inFile >> temp;
        t.push_back(temp);
      }
      inFile >> temp;
    data_matrix.push_back(t);
    t.clear();
    
  }
  vector<double>().swap(t);//t.shrink_to_fit();

  /*for(int i=nInst;i<nInst+insts;i++){
    for(int j=0;j<attrs;j++){
      cout << data_matrix[i][j] << " ";
    }
    cout << endl;
  }*/
  

    lowerLimit = (double)mu*(nInst+insts) - (double)insts;
    upperLimit = (double)mu*(nInst+insts);

    ramUsed = max(getValue(1), ramUsed);               //--------------------8----------------------- //
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/

    clock_t tStart_preProcess = clock();
    preProcessNMPL(nInst);     //appends all pairs whose mass values lie in a range specified;
    //cout << "preProcess: " << (double)(clock() - tStart_preProcess)/CLOCKS_PER_SEC << endl;
    

    /* updates iForest incrementally */
    clock_t tStart_iForest_inc = clock();
    incForestUpdate(insts, attrs ,samplingFactor);

    //cout << (double)(clock() - tStart_iForest_inc)/CLOCKS_PER_SEC << endl;


    sampleLeafElements.clear();
   

    /* updates treeNode and treeNodeMass Matrix incrementally*/
    clock_t tStart_generic_inc = clock();
    incGenericArrayUpdate(insts);
    //cout << (double)(clock() - tStart_generic_inc)/CLOCKS_PER_SEC << endl;
    //cout << "update" << endl;

    //mmTime = (double)(clock() - tStart_iForest_inc)/CLOCKS_PER_SEC;    


    splitAttribute.clear();
    //splitAttribute.shrink_to_fit();
    vector< vector<int> >().swap(splitAttribute);
    splitPoint.clear();
    //splitPoint.shrink_to_fit();
    vector< vector<double> >().swap(splitPoint);
    NodeSize.clear();    
    //NodeSize.shrink_to_fit(); 
    vector< vector<int> >().swap(NodeSize);
    leafElements.clear();
    //leafElements.shrink_to_fit();
    vector< vector<vector<int> > >().swap(leafElements);



    ramUsed = max(getValue(1), ramUsed);                       //-----------------10----------------------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/

    
    /*updates dissimilarity score incrementally*/
    //incDissScore(insts);
    //clock_t tEnd_iForest_inc = clock();
    //mmTime = (double)(tEnd_iForest_inc - tStart_iForest_inc)/CLOCKS_PER_SEC;
    //cout<<"time till inc mass matrix: "<<(double)(tEnd_iForest_inc - tStart_iForest_inc)/CLOCKS_PER_SEC<<" sec"<<endl;
   

    

    ////ramUsed = max(getValue(1), ramUsed);                      //-----------------11-------------------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/

    
    //cout << "before" << endl;
    /*updates muNeighbor matrix*/
    clock_t tStart_muNeigh_inc = clock();    

    incMuNeighborUpdate(insts);    

    //cout << (double)(clock() - tStart_muNeigh_inc)/CLOCKS_PER_SEC << endl;

    mmTime = (double)(clock() - tStart_preProcess)/CLOCKS_PER_SEC;  

   
    treeNodeMass.clear();    
    //treeNodeMass.shrink_to_fit();
    vector< vector<int> >().swap(treeNodeMass);
 
    treeNode.clear();
    //treeNode.shrink_to_fit();
    vector< vector<int> >().swap(treeNode);

    LCA.clear();
    //LCA.shrink_to_fit();
    vector< vector<int> >().swap(LCA);
    massMatrix.clear();
    //massMatrix.shrink_to_fit();
    vector< vector<double> >().swap(massMatrix);

    


    ramUsed = max(getValue(1), ramUsed);                   //-----------------------12--------------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/


    //upadtes clusterId matrix
    mbscanImpl(nInst+insts);
    clock_t tEnd_cluster = clock();
    tTime = (double)(tEnd_cluster - tStart_preProcess)/CLOCKS_PER_SEC ;
    //cout<<"total running time inc: "<<(double)(clock() - tStart_iForest_inc)/CLOCKS_PER_SEC<<" sec"<<endl;

    muNeighbor.clear();
    //muNeighbor.shrink_to_fit();
    vector< vector<int> >().swap(muNeighbor);


    ramUsed = max(getValue(1), ramUsed);                            //--------------------13----------------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/


    writeCluster(nInst+insts, incFile);

    cout << mmTime << " " << tTime << " " << ramUsed << endl;

    data_matrix.clear();
    //data_matrix.shrink_to_fit();
    vector< vector<double> >().swap(data_matrix);


    //fMeasureValue(nInst + insts, incFile);

    ramUsed = max(getValue(1), ramUsed);                           //-------------------14----------------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/

    return 0;  
}


int parseLine(char *line){
    int i = strlen(line);
    const char *p = line;
    while(*p <'0' || *p >'9')
	p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

int getValue(int par){
    FILE *file = fopen("/proc/self/status","r");
    int result = -1;
    char line[128];

    while(fgets(line,128,file) != NULL){
	if(par == 1){               //Ram used
   	    if(strncmp(line, "VmRSS:", 6) == 0){
		result = parseLine(line);
		break;
	    }
	}
	if(par == 2){               //virtual memory used
   	    if(strncmp(line, "VmSize:", 7) == 0){
		result = parseLine(line);
		break;
	    }
	}
    }
    fclose(file);
    return result;
}


void forestCreation(){
    maxNumOfNodes = (int) pow(2.0, (double)(h_max+1))-1;
    if(subSampleSize > nInst)
      subSampleSize = nInst;

    int tree = 0;
       

    splitAttribute.resize(ntree);
    splitPoint.resize(ntree);
    NodeSize.resize(ntree);
    sampleLeafElements.resize(ntree);
    leafElements.resize(ntree);   //will be used in genericArrayEstimation method. only initialization has been done here
    
    double dataDiff;

    while(tree < ntree){
      // cout<<"forestTree: "<<tree<<endl;
      sampleLeafElements[tree].resize(maxNumOfNodes);
      leafElements[tree].resize(maxNumOfNodes);
      
      splitAttribute[tree].resize(maxNumOfNodes);
      for(int i=0;i<splitAttribute[tree].size();i++){
        splitAttribute[tree][i] = -1;             //at the end, if node has splitattribute value -1, it means this node has no child
        //sampleLeafElements[tree][i].resize(0);
        //leafElements[tree][i].resize(0);
      }
      splitPoint[tree].resize(maxNumOfNodes);
      NodeSize[tree].resize(maxNumOfNodes);
      NodeSize[tree][0] = subSampleSize;
      //cout<<NodeSize[tree][0]<<endl;


      dataDistribution.resize(maxNumOfNodes);
      generateRandIndex();   //stores random indices from dataset into dataDistibution[0]

      
      
      int heightNode;
      for(int node=0;node<maxNumOfNodes;node++){
          heightNode = (int)(log2((double)(node+1)));
          int leftChildIndex = 2*node + 1;
          int rightChildIndex = 2*node + 2;

          if(NodeSize[tree][node] <= 1 || heightNode == h_max){
            if(heightNode < h_max){    //keep track of sample elements present in this leaf node (a single element will be there)
              for(int j=0;j<dataDistribution[node].size();j++){
                sampleLeafElements[tree][node].push_back(dataDistribution[node][j]);   //dataDistribution[node] will have just a single element
              } 
            }

            dataDistribution[node].clear();
            //dataDistribution[node].shrink_to_fit();
	    vector<int> ().swap(dataDistribution[node]);
            continue;
          }
          
          splitAttribute[tree][node] = rand()%nAttr;
          maxMinValue(tree, node);   //sets global variable maxValue and minValue
          dataDiff = maxValue-minValue;
          splitPoint[tree][node] = minValue + ((double)rand()/RAND_MAX)*dataDiff;
          
         
          for(int j=0;j<dataDistribution[node].size();j++){
            if(data_matrix[dataDistribution[node][j]][splitAttribute[tree][node]]< splitPoint[tree][node]){
              dataDistribution[leftChildIndex].push_back(dataDistribution[node][j]);
            }
            else{
              dataDistribution[rightChildIndex].push_back(dataDistribution[node][j]);
            }
          }
          NodeSize[tree][leftChildIndex] = dataDistribution[leftChildIndex].size();
          NodeSize[tree][rightChildIndex] = dataDistribution[rightChildIndex].size();
          if(NodeSize[tree][leftChildIndex] == 0)
            NodeSize[tree][leftChildIndex] = 1;
          if(NodeSize[tree][rightChildIndex] == 0)
            NodeSize[tree][rightChildIndex] = 1; 

	  dataDistribution[node].clear(); 
          //dataDistribution[node].shrink_to_fit();
	  vector<int> ().swap(dataDistribution[node]);
      }
      
      tree++;    

    }
    dataDistribution.clear();
    //dataDistribution.shrink_to_fit();
    vector< vector<int> > ().swap(dataDistribution);
    //cout << "sampleLeaf: "<<sizeof(sampleLeafElements) << endl;
    //cout << dataDistribution.capacity() << endl;
}

void generateRandIndex(){
  
  for(int i=0;i<nInst;i++){
    dataDistribution[0].push_back(i);
  }
  random_shuffle(dataDistribution[0].begin(), dataDistribution[0].end());
  
  dataDistribution[0].erase(dataDistribution[0].begin()+subSampleSize, dataDistribution[0].begin()+nInst);
  //for(int i=subSampleSize;i<nInst;i++){
  //  dataDistribution[0].remove(dataDistribution[0].size()-1);
  //}
}

void maxMinValue(int tree, int node){
  maxValue = -999999;
  minValue = 999999;
  for(int i=0;i<dataDistribution[node].size();i++){
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
  
  for(int i=0;i<maxNumOfNodes;i++){
    for(int j=0;j<=i;j++){
      if(i==j){
        LCA[i].push_back(i);
        continue;
      }
      int height_i = (int)(log10((double)(i+1))/log10((double)2)); 
      int height_j = (int)(log10((double)(j+1))/log10((double)2)); 
      int root_i = i;
      int root_j = j;
      
      while (root_i != root_j){ 
          if(root_i > root_j)
            root_i = (root_i%2==0)?root_i/2 - 1: (root_i-1)/2;
          else
            root_j = (root_j%2==0)?root_j/2 - 1: (root_j-1)/2;
      }
      LCA[i].push_back(root_i);
      
    }
  }
  
}

//updates treeNode and TreeNodeMass Matrix
void genericArrayEstimation(){
  //int NumInst = ntree*nrow;
  int maxNumOfNodes = (int) pow(2.0, (double)(h_max+1))-1;
  //cout<<"Max Nodes: "<<maxNumOfNodes<<endl;
  

  treeNode.resize(nInst);
  for(int i=0;i<nInst;i++){
    treeNode[i].resize(ntree);
    //for(int j=0;j<ntree;j++){
    //  treeNode[i][j] = 0;
    //}
  }

  treeNodeMass.resize(ntree);
  for(int j=0;j<ntree;j++){
    treeNodeMass[j].resize(maxNumOfNodes);
    //for(int i=0;i<maxNumOfNodes;i++){
    //  treeNodeMass[j][i] = 0;
    //}
  }


  int heightNode;
  for(int tree = 0;tree<ntree;tree++){
    // cout<<"Estimation: "<<tree<<endl;
    //dataDistribution.clear();
    dataDistribution.resize(maxNumOfNodes);
    dataDistribution[0].resize(nInst);
    for(int i=0;i<nInst;i++){
      dataDistribution[0][i] = i;
    }  

    for(int node=0;node<maxNumOfNodes;node++){
      heightNode = (int)(log2((double)(node+1)));


      if(dataDistribution[node].size() == 0 || NodeSize[tree][node] == 0){
        if(heightNode<h_max){
          dataDistribution[2*node+1].resize(0);
          dataDistribution[2*node+2].resize(0);
        }
	dataDistribution[node].clear();
        //dataDistribution[node].shrink_to_fit();
        vector<int> ().swap(dataDistribution[node]);
        continue;
      }

      treeNodeMass[tree][node] = dataDistribution[node].size();

      //for(int i=0;i<dataDistribution[node].size();i++){
      //  leafElements[tree][node].push_back(dataDistribution[node][i]);    
      //}

      
      if(NodeSize[tree][node]==1 || heightNode == h_max ){    //this condition tells that the node is a leaf node
        //cout<<tree<<" "<<node<<" if"<<endl;
        for(int i=0;i<dataDistribution[node].size();i++){
        	//leafElements[tree][node].push_back(dataDistribution[node][i]); 
          	treeNode[dataDistribution[node][i]][tree] = node;    //stores the min region node of element in the node
        }
        if(heightNode<h_max){
	  for(int i=0;i<dataDistribution[node].size();i++){
        	leafElements[tree][node].push_back(dataDistribution[node][i]); 
          }
          dataDistribution[2*node+1].resize(0);
          dataDistribution[2*node+2].resize(0);
        }
      }
      else{
        //cout<<tree<<" "<<node<<" else"<<endl;
        for(int j=0;j<dataDistribution[node].size();j++){
          if(data_matrix[dataDistribution[node][j]][splitAttribute[tree][node]]< splitPoint[tree][node]){
            dataDistribution[2*node+1].push_back(dataDistribution[node][j]);
          }
          else{
            dataDistribution[2*node+2].push_back(dataDistribution[node][j]);
          }
        }
      }
      dataDistribution[node].clear();
      //dataDistribution[node].shrink_to_fit();
      vector<int> ().swap(dataDistribution[node]);
    }
    dataDistribution.clear();
    //dataDistribution.shrink_to_fit();
    vector< vector<int> > ().swap(dataDistribution);
  }
   	
}



//returns average of mass of their LCA node over all trees 
double dissScore(int a, int b, int t){
  
  double tempMass;
  tempMass = 0;
  for(int tree=0;tree<ntree;tree++){
	 if(tree == t)
		continue;
 	 if(treeNode[a][tree] >= treeNode[b][tree])
             tempMass += (double)treeNodeMass[tree][LCA[treeNode[a][tree]][treeNode[b][tree]]];
	 else
	     tempMass += (double)treeNodeMass[tree][LCA[treeNode[b][tree]][treeNode[a][tree]]];
  }
  //tempMass =tempMass/(maxIncRatio*nInst); 
  tempMass = tempMass/ntree;   //taking avg over all tree
 
  return tempMass;
}



void muNeighborUpdate(int n){
  muNeighbor.resize(n);
  clusterId.resize(n);  //value of index i indicates the cluster id of point i

  

  massMatrix.resize(nInst);

  //vector<int> q ;
  //q.resize(2);

  double mass=0;
  for(int i=0;i<n;i++){
    clusterId[i] = UNCLASSIFIED;
   
    for(int j=0;j<i;j++){
      mass = dissScore(i,j, -1);    //passing t=-1 because we want contribution from all trees
      massMatrix[i].push_back(mass);
      if(mass/n > mu){
        continue;
      }
      muNeighbor[i].push_back(j);
      muNeighbor[j].push_back(i);
      //if(mass/n >= lowerLimit){
	//q[0] = i;
	//q[1] = j;
	//NodeMassPairsList.push_back(q);
      //}
    }
  }


  

  //for(int i = 0;i<nInst;i++){
	//cout <<i << " " <<  muNeighbor[i].size();
	//for(int j=0;j<muNeighbor[i].size();j++)
	//	cout << muNeighbor[i][j] << " ";
	//cout << endl;
  //}

   //cout << "---------------------------------------------" << endl;
}



void preProcessNMPL(int n){
  vector<int> q ;
  q.resize(2);
 
  for(int i=0;i<n;i++){
    for(int j=0;j<i;j++){
	if(massMatrix[i][j] >= lowerLimit && massMatrix[i][j] <= upperLimit){
	    q[0] = i;
	    q[1] = j;
	    NodeMassPairsList.push_back(q);
	}
    }
  }
}



void incForestUpdate(int ins, int a,double samplingFactor){
  
  int insts,attrs;
  insts = ins;
  attrs = a;
  int sampleSize = (int) (insts * samplingFactor);
  
  
  for(int tree=0; tree<ntree; tree++){
    //cout << "incTree: "<< tree<<endl;

    dataDistribution.resize(maxNumOfNodes);
    for(int i=0;i<insts;i++){
      dataDistribution[0].push_back(nInst+i);
    }
    random_shuffle(dataDistribution[0].begin(),dataDistribution[0].end());
    dataDistribution[0].erase(dataDistribution[0].begin()+sampleSize, dataDistribution[0].begin()+insts);
   

    NodeSize[tree][0] += dataDistribution[0].size();

     
    int heightNode;
    for(int node=0;node<maxNumOfNodes;node++){
      heightNode = (int)(log2((double)(node+1)));
      int leftChildIndex = 2*node + 1;
      int rightChildIndex = 2*node + 2;

      if(NodeSize[tree][node] <= 1 || heightNode == h_max){
        //if(heightNode < h_max){    //keep track of sample elements present in this leaf node (a single element will be there)
          //for(int j=0;j<dataDistribution[node].size();j++){
           // sampleLeafElements[tree][node].push_back(dataDistribution[node][j]);   //dataDistribution[node] will have just a single element
          //} 
        //}

	vector<int> ().swap(dataDistribution[node]);
        continue;
      }

      if(splitAttribute[tree][node] == -1){
        splitAttribute[tree][node] = rand()%nAttr;
        for(int s=0;s<sampleLeafElements[tree][node].size();s++){
          dataDistribution[node].push_back(sampleLeafElements[tree][node][s]);     //appends the previous sample elements into current list and it together gets splitted into new children
        }
        //NodeSize[tree][node] += sampleLeafElements[tree][node].size();
        //sampleLeafElements[tree][node].resize(0);    //since this node is no longer a leaf node
        
        maxMinValue(tree, node);   //sets global variable maxValue and minValue, earlier this node was either empty or contained single element. 
        double dataDiff = maxValue-minValue;
        splitPoint[tree][node] = minValue + ((double)rand()/RAND_MAX)*dataDiff;
      }

      	 

      for(int j=0;j<dataDistribution[node].size();j++){
        if(data_matrix[dataDistribution[node][j]][splitAttribute[tree][node]] < splitPoint[tree][node]){
          dataDistribution[leftChildIndex].push_back(dataDistribution[node][j]);
        }
        else{
          dataDistribution[rightChildIndex].push_back(dataDistribution[node][j]);
        }
      }
      NodeSize[tree][leftChildIndex] += dataDistribution[leftChildIndex].size();
      NodeSize[tree][rightChildIndex] += dataDistribution[rightChildIndex].size();

      if(NodeSize[tree][leftChildIndex] == 0)
        NodeSize[tree][leftChildIndex] = 1;
      if(NodeSize[tree][rightChildIndex] == 0)
        NodeSize[tree][rightChildIndex] = 1;

      dataDistribution[node].clear();
      //dataDistribution[node].shrink_to_fit();
      vector<int> ().swap(dataDistribution[node]);
    }
    
    dataDistribution.clear();
    //dataDistribution.shrink_to_fit();
    vector< vector<int> > ().swap(dataDistribution);
  }
}





void incGenericArrayUpdate(int ins){
  //cout<<"Max Nodes: "<<maxNumOfNodes<<endl;
  int maxNumOfNodes = (int) pow(2.0, (double)(h_max+1))-1;
  int insts = ins;
  
  vector <int > treeNodeTemp;
  treeNodeTemp.resize(ntree);
  for(int i=nInst;i<nInst+insts;i++){
    treeNode.push_back(treeNodeTemp);
  }
  treeNodeTemp.clear();
  vector<int> ().swap(treeNodeTemp);

  //cout << "Temp " << treeNode.size() << " " << treeNode[0][10] << " "<< treeNode[nInst+5][10] << endl;

  int heightNode;
  //vector< int> oldTreeNodeMass;
  //vector< int> oldTreeNode;

  double newRatio,  oldRatio;
  int pos, a, b;

 

  for(int tree = 0;tree<ntree;tree++){

  	
  	//for(int n=0;n<maxNumOfNodes;n++){
  	//	oldTreeNodeMass.push_back(treeNodeMass[tree][n]);
  	//}
  	
  	//for(int i=0;i<nInst;i++){
  	//	oldTreeNode.push_back(treeNode[i][tree]);
  	//}


    

    
    //dataDistribution.clear();
    dataDistribution.resize(maxNumOfNodes);
    //dataDistribution[0].resize(insts);
    for(int i=0;i<insts;i++){
      dataDistribution[0].push_back(nInst + i);
    }  

    int nodeUpdated = 0;

    for(int node=0;node<maxNumOfNodes;node++){
    	heightNode = (int)(log2((double)(node+1)));

    	//cout << node << " " << oldTreeNodeMass[node] << " " << dataDistribution[node].size() << endl;

      	if(dataDistribution[node].size() == 0 || NodeSize[tree][node] == 0){
        	if(heightNode<h_max){
          		dataDistribution[2*node+1].resize(0);
          		dataDistribution[2*node+2].resize(0);
        	}
		dataDistribution[node].clear();
		//dataDistribution[node].shrink_to_fit();
	        //vector<int> ().swap(dataDistribution[node]);
        	continue;
    	  }

      	if(heightNode<h_max && leafElements[tree][node].size()>0){//oldTreeNodeMass[2*node+1]==0 && oldTreeNodeMass[2*node+2]==0){
      //if(heightNode < h_max && NodeSize[tree][node] == 1)
        	for(int s=0;s<leafElements[tree][node].size();s++){
          		dataDistribution[node].push_back(leafElements[tree][node][s]);
        	}
        	nodeUpdated = 1;
        	leafElements[tree][node].resize(0); //since this node is no longer a leaf node

      	}

      

    	
        
        if(nodeUpdated == 1){
        	treeNodeMass[tree][node] = dataDistribution[node].size();
		nodeUpdated = 0;
	}
        else{
        	treeNodeMass[tree][node] += dataDistribution[node].size();
        }

    	//newRatio = (double)treeNodeMass[tree][node]/ntree;



       /* if(nodeUpdated == 1 && (NodeSize[tree][node]==1 || heightNode == h_max)){
       	    for(int l=0;l<dataDistribution[node].size();l++){
            	if(dataDistribution[node][l]>=nInst)
            		break;
            	for(int r=l+1;r<dataDistribution[node].size();r++){
                	if(dataDistribution[node][r]<nInst){
			   a = dataDistribution[node][l];
			   b = dataDistribution[node][r];

			   if(oldTreeNode[a] >= oldTreeNode[b]) 
                    		oldRatio = oldTreeNodeMass[LCA[oldTreeNode[a]][oldTreeNode[b]]];
			   else
				oldRatio = oldTreeNodeMass[LCA[oldTreeNode[b]][oldTreeNode[a]]];
                    	   oldRatio = (double)oldRatio/ntree;


                    	
                    	   if(a >= b){
                    		massMatrix[a][b] += (newRatio - oldRatio);
				//newMass = massMatrix[a][b];
			   }
                    	   else{
                    		massMatrix[b][a] += (newRatio - oldRatio);
				//newMass = massMatrix[b][a];
                	   }


			   		 
                	}
			else
				break;
            	}
            }
    	}*/
      

      

      
    	if(NodeSize[tree][node]==1 || heightNode == h_max ){    //this condition tells that the node is a leaf node
        	//cout << tree << " " << node << endl; 
        	//cout << node << " " << oldTreeNodeMass[node] << " "  ;
        	for(int i=0;i<dataDistribution[node].size();i++){
    	    		treeNode[dataDistribution[node][i]][tree] = node;    //stores the min region node of element in the node
    	    	//leafElements[tree][node].push_back(dataDistribution[node][i]);
    	    	
        	}
        	//cout << treeNodeMass[tree][treeNode[dataDistribution[node][0]][tree]] << endl;
        	if(heightNode<h_max){
			for(int i=0;i<dataDistribution[node].size();i++){
    	    			leafElements[tree][node].push_back(dataDistribution[node][i]);
    	    	
        		}
        		dataDistribution[2*node+1].resize(0);
        		dataDistribution[2*node+2].resize(0);
          		//for(int i=0;i<dataDistribution[node].size();i++){
          		//  leafElements[tree][node].push_back(dataDistribution[node][i]);    
          		//}
        	}
    	}
    	else{
        	//cout<<tree<<" "<<node<<" else"<<endl;
        	for(int j=0;j<dataDistribution[node].size();j++){
    	    		if(data_matrix[dataDistribution[node][j]][splitAttribute[tree][node]]< splitPoint[tree][node]){
        	    		dataDistribution[2*node+1].push_back(dataDistribution[node][j]);
          		}
          		else{
            			dataDistribution[2*node+2].push_back(dataDistribution[node][j]);
          		}
        	}
          
    	}

      

      
    	/*if(nodeUpdated == 1){
        	nodeUpdated = 0;
		if(heightNode == h_max){
          		continue;
        	}
        	for(int l=0;l<dataDistribution[2*node+1].size();l++){
          		if(dataDistribution[2*node+1][l]>=nInst)
            			break;
          		for(int r=0;r<dataDistribution[2*node+2].size();r++){
            			if(dataDistribution[2*node+2][r] < nInst){
					a = dataDistribution[2*node+1][l];
					b = dataDistribution[2*node+2][r];


					if(oldTreeNode[a] >= oldTreeNode[b])
              					oldRatio = oldTreeNodeMass[LCA[oldTreeNode[a]][oldTreeNode[b]]];
					else 
						oldRatio = oldTreeNodeMass[LCA[oldTreeNode[b]][oldTreeNode[a]]];
					oldRatio = oldRatio/ntree;


              				if(a >= b){
                    				massMatrix[a][b] += (newRatio - oldRatio);
						//newMass = massMatrix[a][b];
					}
              				else{
               					massMatrix[b][a] += (newRatio - oldRatio);
						//newMass = massMatrix[b][a];
					}

					
              				
            			} 
				else
					break;           
			}
        	}
	}*/

	dataDistribution[node].clear();
	//dataDistribution[node].shrink_to_fit();
	//vector<int> ().swap(dataDistribution[node]);
    }
    dataDistribution.clear();
    //dataDistribution.shrink_to_fit();
    //vector< vector<int> > ().swap(dataDistribution);
    //oldTreeNodeMass.clear();
    //oldTreeNodeMass.shrink_to_fit();
    //vector<int> ().swap(oldTreeNodeMass);
    //oldTreeNode.clear();
    //oldTreeNode.shrink_to_fit();
    //vector<int> ().swap(oldTreeNode);
  }
  	
}



void incMuNeighborUpdate(int insts){

  muNeighbor.clear();
  muNeighbor.resize(nInst+insts);
  clusterId.clear();
  clusterId.resize(nInst+insts);  //value of index i indicates the cluster id of point i


   

  //updating mass value for pairs in NMPL list
  int a, b;
  //cout << "NMPL Size: " << NodeMassPairsList.size() << endl;
  for(int i=0;i<NodeMassPairsList.size();i++){
	a = NodeMassPairsList[i][0];
 	b = NodeMassPairsList[i][1];
        //cout << massMatrix[a][b] << " " ;
	massMatrix[a][b] = dissScore(a, b, -1);
	//cout << massMatrix[a][b] << endl;
  }
 
  NodeMassPairsList.clear();
  
 

  vector <double > massVectorTemp;
  vector<int> muNeighTemp;
  double mass=0;

  for(int i=nInst;i<nInst+insts;i++){
    clusterId[i] = UNCLASSIFIED;
    //massVectorTemp.resize(i); 
    for(int j=0;j<i;j++){
      mass = dissScore(i,j,-1);
      massVectorTemp.push_back(mass);
      //if(mass/(nInst+insts) > mu){
        //continue;
      //}
      //muNeighbor[i].push_back(j);
      //muNeighbor[j].push_back(i); 

      
    }
    
    massMatrix.push_back(massVectorTemp);  
    massVectorTemp.clear();
  }


  string massOutFile = "massCluster/massInc";
  ofstream massFile(massOutFile.c_str(),ios::out);
  massFile<<""; 
  massFile.close();
  //Write the root node to a file
  massFile.open(massOutFile.c_str(),ios::app);

  for(int i=0;i<nInst+insts;i++){
	clusterId[i] = UNCLASSIFIED;
	//cout << i << " " << muNeighbor[i].size() << endl;
	for(int j=0;j<i;j++){
      		massFile << i << " " << j << " " << massMatrix[i][j]/(nInst+insts) << endl ;
      		if(massMatrix[i][j]/(nInst+insts) > mu){
       			continue;
      		}
      		muNeighbor[i].push_back(j);
      		muNeighbor[j].push_back(i);

		
	}

  }
  massFile.close();

//cout << "mass: " << massMatrix[1100][110]/(nInst+insts) << endl;
  
  
}





void mbscanImpl(int n){
  int cId=0;
  for(int i=0;i<n;i++){
    if(clusterId[i] == UNCLASSIFIED){
      if(expandCluster(i, cId) == CORE_POINT){   //new cluster is to be formed for next point
        cId++;

      }
      // cout<<i<<" "<<cId<<" "<<muNeighbor[i].size()<<endl;
    }
  }
  nCluster = cId;
  //cout<<cId<<endl;
  //writeCluster();  
}

int expandCluster(int index, int cId){
  int retValue = NON_CORE_POINT;
  /*if(muNeighbor[index].size() == 0){
  	clusterId[index] = NOISE;
    return retValue;
  }*/
  if(muNeighbor[index].size() < minpts){
    clusterId[index] = NOISE;
  }
  else{
    clusterId[index] = cId;
    //add all the neighbors to same clusters
    for(int j=0;j<muNeighbor[index].size();j++){
      clusterId[muNeighbor[index][j]] = cId;
    }
    //see how far the cluster spreads
    for(int j=0;j<muNeighbor[index].size();j++){
      spreadCluster(muNeighbor[index][j], index, cId);
    }
    retValue = CORE_POINT;
  }
  return retValue;
}

void spreadCluster(int index, int src,int cId){
  /*if(muNeighbor[index].size() == 0){
  	clusterId[index] = NOISE;
    return;
  }*/
  //process muNeighbors of neighbour
  if(muNeighbor[index].size() >= minpts){
    for(int j=0;j<muNeighbor[index].size();j++){
      if(clusterId[muNeighbor[index][j]] == UNCLASSIFIED || clusterId[muNeighbor[index][j]] == NOISE){
        if(clusterId[muNeighbor[index][j]] == UNCLASSIFIED ){
          muNeighbor[src].push_back(muNeighbor[index][j]);
        }
        clusterId[muNeighbor[index][j]] = cId;
      }
      
    }
  }
}




void writeCluster(int n, string cfile){
    /*string clusterOutFile = cfile.substr(0, cfile.find("_")) + "_finalClusterInc";
    ofstream clusterFile(clusterOutFile.c_str(),ios::out);
    clusterFile<<""; 
    clusterFile.close();
    //Write the root node to a file
    clusterFile.open(clusterOutFile.c_str(),ios::app);
    for(int j=0;j<nAttr;j++)
      clusterFile << "Point_" << j << ",";
    clusterFile << "ClusterId" << endl;
    for(int i=0;i<n;i++){
      for(int j=0;j<nAttr;j++)
        clusterFile << data_matrix[i][j] << ",";
      clusterFile << clusterId[i] << endl; 
    }*/

    //cout << n <<endl;
    //used for comparing result between static and orig clustering
    //string compareFile = cfile.substr(0, cfile.find("_")) + "_compareCIdInc";
    string compareFile = "massCluster/clusterInc";
    ofstream compareoutFile(compareFile.c_str(),ios::out);
    compareoutFile<<""; 
    compareoutFile.close();
    //Write the root node to a file
    compareoutFile.open(compareFile.c_str(),ios::app);
    for(int i=0;i<n;i++){
      for(int j=i+1;j<n;j++){
      	//if(clusterId[i] == -2 || clusterId[j] == -2)
        //  compareoutFile << i << " " << j << " " << "2" << endl;
        //else
        if(clusterId[i] == clusterId[j])
          compareoutFile << i << " " << j << " " << "1" << endl; 
        else
          compareoutFile << i << " " << j << " " << "0" << endl; 
        }
    }
    compareoutFile.close();
}



void fMeasureValue(int insts, string incFile){
    string fClusterInc = incFile.substr(0, incFile.find("_")) + "_compareCIdInc";   
    string fClusterOrig = incFile.substr(0, incFile.find("_")) + "_compareCIdOrig";
    //int nInst = 150; 

    //vector< vector< int> > incCluster;
    //vector< vector< int> > origCluster;
    long long int n = (insts*(insts-1))/2;
    //incCluster.resize(n);
    //origCluster.resize(n);
    //int temp;

    ifstream in(fClusterInc.c_str()); 
    if(!in)
    {
      cout<<"Cannot open the input file: "<<fClusterInc<<endl;
      exit(0);
    } 

    ifstream in1(fClusterOrig.c_str()); 
    if(!in1)
    {
      cout<<"Cannot open the input file: "<<fClusterOrig<<endl;
      exit(0);
    } 


    int a=0,b=0,c=0;

    for(int i=0;i<n;i++){
        int a1,b1,c1,c2;
        in >> a1 >> b1 >> c1;
	in1 >> a1 >> b1 >> c2;

	if((c1 == 1 && c2 == 1) || (c1 == 0 && c2 == 0))
            a++;
        else if(c1 == 1 && c2 == 0)
            b++;
        else if(c1 == 0 && c2 == 1)
            c++;
        
    }


    

    double fMeasure = (double) a/(a+b+c);

    //cout<<"F-Measure = "<<fMeasure<<endl;

    cout << mu <<" "<<minpts << " "<< nCluster << " " << fMeasure << " " << mmTime << " " << tTime << " " << ramUsed << endl;
    /*int nInters = 0;
    int nUnion = 0;

    for(int i=0;i<n;i++){
        if(staticCluster[i][2] != incCluster[i][2]){
            //cout << staticCluster[i][0]<<" "<<staticCluster[i][1]<<" "<<staticCluster[i][2]<<" "<<incCluster[i][2]<<endl;
            nUnion += 2;
        }
        else{
            nUnion += 1;
            nInters += 1;
        }
    }

    cout<<"Total match = "<<nInters<<endl;
    cout<<"Accuracy (intersection/union) = "<<((double)nInters/nUnion)*100<<endl;*/
 
}

