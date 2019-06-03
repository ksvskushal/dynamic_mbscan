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

using namespace std;


vector< vector<double> >data_matrix;
int nInst,nAttr;

//int currRowIndex=0;   //keep tracks of global number of rows already stored in tree
int h_max;
int ntree;        //total number of trees to be formed
int subSampleSize;
int maxNumOfNodes;

//vector<int> randIndex;  //stores subSample number of index from 1 to nInst randomly without replacement

vector< vector<int> >splitAttribute;   //(i,j) stores the randmly generated split attribute for node j of tree i
vector< vector<double> >splitPoint;
vector< vector<int> >NodeSize;

vector< vector<double> >dataDistribution;   //stores indices of each node in a tree

vector< vector<int> >treeNode;       //(i,j) cell indicates node number of min region where instance i lies in tree j otherwise 0.
vector< vector<int> >treeNodeMass;   //(i,j) cell indicates count of node j in tree i.
vector< vector<int> >LCA;            //(i,j) cell indicates lowest common ancestor node number of node i and node j.
vector< vector<double> >massMatrix;     //(i,j) cell indicates the dissimilarity value of two instances i and j.

double maxValue, minValue;
double mmTime, tTime;   //time to compute mass matrix and total time in seconds


void forestCreation();
void generateRandIndex();
void maxMinValue(int tree, int node);
void commonPathMatrixLCA();
void genericArrayEstimation();
void dissScore();  


int parseLine(char *line);
int getValue(int );

int ramUsed; 



int main(int argc, char* argv[])
{
    //input parameters
    //cout<<"start"<<endl;
    string inFile = argv[1];   //static file
    //cout<<inFileBin;
    double samplingFactor = atof(argv[2]);
    
    ntree = atoi(argv[3]);
    

    srand(time(NULL));


    ramUsed = max(getValue(1), ramUsed);          //-------------1---------------------//
    /*int memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/


    //storing data into data_matrix 
    ifstream in(inFile.c_str()); 
    if(!in)
    {
      cout<<"Cannot open the input file: "<<inFile<<endl;
      exit(0);
    }     
    
    in >> nInst >> nAttr;

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

    subSampleSize = nInst * samplingFactor;
    h_max =  (int)(log10((double)(subSampleSize))/log10(2.0));


    ramUsed = max(getValue(1), ramUsed);      //------------2-----------------//
    //memUsed = getValue(2);
    //cout<<ramUsed << endl;
   
    /*********** Data stored in data_matrix *********/

    //cout<<"Rows:"<<nInst << " Attr: " << nAttr << "  "<< data_matrix[0][1]<<endl;

     /* updates LCA matrix */
    commonPathMatrixLCA();

    clock_t tStart_iForest = clock();
    forestCreation();
    //cout << (double)(clock() - tStart_iForest)/CLOCKS_PER_SEC << endl;

    /*********** iForest Created *********/
   
    ramUsed = max(getValue(1), ramUsed);           //----------------3-----------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/


    /* updates treeNode, treeNodeMass  */
    clock_t tStart_gen = clock();
    genericArrayEstimation();    
    //cout << (double)(clock() - tStart_gen)/CLOCKS_PER_SEC << endl;

    ramUsed = max(getValue(1), ramUsed);               //---------------4--------------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/


    /*updates massMatrix */
    clock_t tStart_diss = clock();
    dissScore();
    //cout << (double)(clock() - tStart_diss)/CLOCKS_PER_SEC << endl;


    mmTime = (double)(clock() - tStart_iForest)/CLOCKS_PER_SEC;
    
    cout << mmTime << " " << ramUsed<< endl;
    
    
    splitAttribute.clear();
    //splitAttribute.shrink_to_fit();
    vector< vector<int> >().swap(splitAttribute);
    splitPoint.clear();
    //splitPoint.shrink_to_fit();
    vector< vector<double> >().swap(splitPoint);
    NodeSize.clear();    
    //NodeSize.shrink_to_fit(); 
    vector< vector<int> >().swap(NodeSize);
    treeNodeMass.clear();    
    //treeNodeMass.shrink_to_fit();
    vector< vector<int> >().swap(treeNodeMass);
    treeNode.clear();
    //treeNode.shrink_to_fit();
    vector< vector<int> >().swap(treeNode);
    LCA.clear();
    //LCA.shrink_to_fit();
    vector< vector<int> >().swap(LCA);
    
    data_matrix.clear();
    //data_matrix.shrink_to_fit();
    vector< vector<double> >().swap(data_matrix);


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

    

    while(tree < ntree){
      // cout<<"forestTree: "<<tree<<endl;
      splitAttribute[tree].resize(maxNumOfNodes);
      splitPoint[tree].resize(maxNumOfNodes);
      NodeSize[tree].resize(maxNumOfNodes);
      NodeSize[tree][0] = subSampleSize;
      //cout<<NodeSize[tree][0]<<endl;



      dataDistribution.resize(maxNumOfNodes);
      generateRandIndex();   //stores random indexes from data_matrix into dataDistribution[0] vector

      


      /*writing iTrees into a file*/
      /*
      ostringstream str1;
      str1<<tree;
      string output_file = "itrees/itree_"+str1.str();
      ofstream iso_tree(output_file.c_str(),ios::out);
      iso_tree<<""; 
      iso_tree.close();

      //Write the root node to a file
      iso_tree.open(output_file.c_str(),ios::app); */    



      int heightNode;
      for(int node=0;node<maxNumOfNodes;node++){
          //writing node indices into a file
          /*if(dataDistribution[node].size()>0)
            iso_tree<<node<<" ";   //storing the node count at each line
          
          for(int i=0;i<dataDistribution[node].size();i++)
          {
            iso_tree<<dataDistribution[node][i]<<" ";
          }
          if(dataDistribution[node].size()>0)
            iso_tree<<endl;*/
          
          //cout<<splitAttribute[tree][node]<<" "<<splitPoint[tree][node]<<endl;

          heightNode = (int)(log2((double)(node+1)));
          //cout<<heightNode<<endl;
          //cout<<"size : "<<NodeSize[tree].size()<<endl;

          int leftChildIndex = 2*node + 1;
          int rightChildIndex = 2*node + 2;

          if(NodeSize[tree][node] <= 1 || heightNode == h_max){
            //if(NodeSize[tree][node] == 1)
              //iso_tree<<node<<" "<<1<<endl;
	    dataDistribution[node].clear();               //-----------------------------//
	    dataDistribution[node].shrink_to_fit();
            continue;
          }
          
          splitAttribute[tree][node] = rand()%nAttr;
          maxMinValue(tree, node);   //sets global variable maxValue and minValue
          double dataDiff = maxValue-minValue;

          splitPoint[tree][node] = minValue + ((double)rand()/RAND_MAX)*dataDiff;

          
          //iso_tree<<node<<" "<<splitAttribute[tree][node]<<" "<<splitPoint[tree][node]<<endl;  //indicates node contains q, and p
          

          //dataDistribution[leftChildIndex].resize(subSampleSize);
          //dataDistribution[rightChildIndex].resize(subSampleSize);
          for(int j=0;j<dataDistribution[node].size();j++){
            //cout<<j<<endl;

            if(data_matrix[dataDistribution[node][j]][splitAttribute[tree][node]]< splitPoint[tree][node]){
              dataDistribution[leftChildIndex].push_back(dataDistribution[node][j]);
            }
            else{
              dataDistribution[rightChildIndex].push_back(dataDistribution[node][j]);
            }
          }

          

          NodeSize[tree][leftChildIndex] = dataDistribution[leftChildIndex].size();
          NodeSize[tree][rightChildIndex] = dataDistribution[rightChildIndex].size();
          //NodeSize[tree][node] = 0;

          dataDistribution[node].clear();    //--------------------------------//
	  dataDistribution[node].shrink_to_fit();

      }

      //iso_tree.close();
      tree++;    

    }
    dataDistribution.clear();        //-----------------------------------//
    dataDistribution.shrink_to_fit();
}



void generateRandIndex(){           //--------------------------------//
  for(int i=0;i<nInst;i++){
    dataDistribution[0].push_back(i);
  }
  random_shuffle(dataDistribution[0].begin(), dataDistribution[0].end());
  
  dataDistribution[0].erase(dataDistribution[0].begin()+subSampleSize, dataDistribution[0].begin()+nInst);
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
    //dataDistribution.clear();             //-----------------//
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
	dataDistribution[node].clear();    //--------------------------//
	//dataDistribution[node].shrink_to_fit();        
	continue;
      }

      treeNodeMass[tree][node] = dataDistribution[node].size();

      
      if(NodeSize[tree][node]==1 || heightNode == h_max ){
        //cout<<tree<<" "<<node<<" if"<<endl;
        for(int i=0;i<dataDistribution[node].size();i++){
          treeNode[dataDistribution[node][i]][tree] = node;
        }
        if(heightNode<h_max){
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
     dataDistribution[node].clear();     //-----------------------// 
     //dataDistribution[node].shrink_to_fit();
    }
  }
  dataDistribution.clear();
  //dataDistribution.shrink_to_fit();
}


void dissScore(){
  //cout<<"mass"<<endl;
  ////massMatrix.clear();

  //massMatrix.resize(nInst);


  string massOutFile = "massMatrix/massOrig";
  ofstream massFile(massOutFile.c_str(),ios::out);
  massFile<<""; 
  massFile.close();
  //Write the root node to a file
  massFile.open(massOutFile.c_str(),ios::app);


  //#pragma omp parallel for schedule(dynamic,100) num_threads(4)
  double tempMass;
  for(int i=0;i<nInst;i++){
      //massMatrix[i].resize(nInst);
      //cout<<"mass: "<<massMatrix[i][i]<<endl;
      for(int j=0;j<=i;j++){
	tempMass = 0;
        for(int tree=0;tree<ntree;tree++){
	  if(treeNode[i][tree] >= treeNode[j][tree])
             tempMass += (double)treeNodeMass[tree][LCA[treeNode[i][tree]][treeNode[j][tree]]]/ntree;
	  else
	     tempMass += (double)treeNodeMass[tree][LCA[treeNode[j][tree]][treeNode[i][tree]]]/ntree;
        }
        //tempMass = tempMass/(maxIncRatio*nInst); 
        //tempMass = tempMass/ntree;   //taking avg over all tree

	massFile << i << " " << j << " " << tempMass << endl ;
	//if(i ==1100 && j == 110)
	//	cout << "mass: " << tempMass << endl;

	//massMatrix[i].push_back(tempMass);
        
     }
     //massFile << endl;
  }

  massFile.close();

  

}










