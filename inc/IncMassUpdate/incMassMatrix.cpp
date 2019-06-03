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
//vector< vector<vector<int> > >  leafElements;     //stores elements present in leaf node having height < max_height in genericArrayEstimation method

vector< vector<int> >treeNode;       //(i,j) cell indicates node number of min region where instance i lies in tree j otherwise 0.
vector< vector<int> >treeNodeMass;   //(i,j) cell indicates count of node j in tree i.
vector< vector<int> >LCA;            //(i,j) cell indicates lowest common ancestor node number of node i and node j.
vector< vector<double> > massMatrix;     //(i,j) cell indicates the dissimilarity value of two instances i and j.

double maxValue, minValue;



void forestCreation();
void generateRandIndex();
void maxMinValue(int tree, int node);
void commonPathMatrixLCA();
void genericArrayEstimation();
void dissScore();    //returns mass value of inst a and inst b except from tree t in order of number of itrees.



void incForestUpdate(int ins, int a ,double samplingFactor);
void incGenericArrayUpdate(int ins);
void incDissScore(int ins, string fname);


int parseLine(char *line);
int getValue(int );


double mmTime,tTime;  //time to update mass matrix and total inc time in sec

int ramUsed;


int main(int argc, char* argv[]){
    string inFileBin = argv[1];   //static input data
    string incFile = argv[2];     //inc input data
    double samplingFactor = atof(argv[3]);
    ntree = atoi(argv[4]);
    
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
    dissScore();
    //clock_t tEnd_iForest = clock();
    //cout<<"time till static mass matrix: "<<(double)(tEnd_iForest - tStart_iForest)/CLOCKS_PER_SEC<<" sec"<<endl;

    //string fname = "massStatic";
    //incDissScore(0, fname);
   

    /*---------------Incremental Mass Update Part -------------*/

  
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
  

    

    /* updates iForest incrementally */
    clock_t tStart_iForest_inc = clock();
    incForestUpdate(insts, attrs ,samplingFactor);

    //cout << (double)(clock() - tStart_iForest_inc)/CLOCKS_PER_SEC << endl;


    sampleLeafElements.clear();
    //sampleLeafElements.shrink_to_fit();
    vector< vector<vector<int> > >().swap(sampleLeafElements);    


    ramUsed = max(getValue(1), ramUsed);                         //------------------9----------------------------//
    /*memUsed = getValue(2);
    cout<<ramUsed << " "<< memUsed << endl;*/

    /* updates treeNode, treeNodeMass and mass Matrix incrementally*/
    clock_t tStart_generic_inc = clock();
    incGenericArrayUpdate(insts);
    //cout << (double)(clock() - tStart_generic_inc)/CLOCKS_PER_SEC << endl;
    //cout << "update" << endl;

    mmTime = (double)(clock() - tStart_iForest_inc)/CLOCKS_PER_SEC;
    ramUsed = max(getValue(1), ramUsed); 
    cout << mmTime << " " << ramUsed << endl;

    string fName = "massMatrix/massInc";
    incDissScore(insts, fName);

    

    splitAttribute.clear();
    //splitAttribute.shrink_to_fit();
    vector< vector<int> >().swap(splitAttribute);
    splitPoint.clear();
    //splitPoint.shrink_to_fit();
    vector< vector<double> >().swap(splitPoint);
    NodeSize.clear();    
    //NodeSize.shrink_to_fit(); 
    vector< vector<int> >().swap(NodeSize);
    //leafElements.clear();
    //leafElements.shrink_to_fit();
    //vector< vector<vector<int> > >().swap(leafElements);



   
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
    //leafElements.resize(ntree);   //will be used in genericArrayEstimation method. only initialization has been done here
    
    double dataDiff;

    while(tree < ntree){
      // cout<<"forestTree: "<<tree<<endl;
      sampleLeafElements[tree].resize(maxNumOfNodes);
      //leafElements[tree].resize(maxNumOfNodes);
      
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
            //if(heightNode < h_max){    //keep track of sample elements present in this leaf node (a single element will be there)
              //for(int j=0;j<dataDistribution[node].size();j++){
                //sampleLeafElements[tree][node].push_back(dataDistribution[node][j]);   //dataDistribution[node] will have just a single element
              //} 
            //}

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

  /*cout << "LCA" << endl;
  for(int i=0;i<maxNumOfNodes;i++){
    for(int j=0;j<=i;j++)
	cout << i << " " << j<< " " << LCA[i][j] << endl;
  }
  cout << "-------------------------" << endl;*/
  
  
}

//updates treeNode and TreeNodeMass Matrix
void genericArrayEstimation(){
  //int NumInst = ntree*nrow;
  int maxNumOfNodes = (int) pow(2.0, (double)(h_max+1))-1;
  //cout<<"Max Nodes: "<<maxNumOfNodes<<endl;
  

  treeNode.resize(nInst);
  for(int i=0;i<nInst;i++){
    treeNode[i].resize(ntree);
  }

  treeNodeMass.resize(ntree);
  for(int j=0;j<ntree;j++){
    treeNodeMass[j].resize(maxNumOfNodes);
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
	  //for(int i=0;i<dataDistribution[node].size();i++){
        	//leafElements[tree][node].push_back(dataDistribution[node][i]); 
          //}
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
   	
  /*cout << "TreeNode Static" << endl;
  for(int i=0;i<nInst; i++){
     cout << i << "   " ;
     for(int j=0;j<ntree;j++)
	cout << treeNode[i][j] << " ";
     cout << endl;
  }
  cout << "--------------------" << endl;


  cout << "TreeNodeMass Static" << endl;
  for(int i=0;i<maxNumOfNodes; i++){
        cout << i << "    " ;
	for(int j=0;j<ntree;j++)	
		cout << treeNodeMass[j][i] <<  " ";
	cout << endl;
  }
  cout << "--------------------" << endl;*/
 
}



void dissScore(){
  
  massMatrix.resize(nInst);  

  //#pragma omp parallel for schedule(dynamic,100) num_threads(4)
  double tempMass;
  for(int i=0;i<nInst;i++){
      //massMatrix[i].resize(nInst);
      //cout<<"mass: "<<massMatrix[i][i]<<endl;
      for(int j=0;j<=i;j++){
	tempMass = 0;
        for(int tree=0;tree<ntree;tree++){
	  if(treeNode[i][tree] >= treeNode[j][tree])
             tempMass += (double)treeNodeMass[tree][LCA[treeNode[i][tree]][treeNode[j][tree]]];
	  else
	     tempMass += (double)treeNodeMass[tree][LCA[treeNode[j][tree]][treeNode[i][tree]]];
        }
        //tempMass = tempMass/(maxIncRatio*nInst); 
        tempMass = tempMass/ntree;   //taking avg over all tree

	massMatrix[i].push_back(tempMass);
        
     }
    
  }
  //cout << massMatrix[1][1] << endl;
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
        if(heightNode < h_max){    //keep track of sample elements present in this leaf node (a single element will be there)
          for(int j=0;j<dataDistribution[node].size();j++){
            sampleLeafElements[tree][node].push_back(dataDistribution[node][j]);   //dataDistribution[node] will have just a single element
          } 
        }
	dataDistribution[node].clear();
	vector<int> ().swap(dataDistribution[node]);
        continue;
      }

      if(splitAttribute[tree][node] == -1){
        splitAttribute[tree][node] = rand()%nAttr;
        for(int s=0;s<sampleLeafElements[tree][node].size();s++){
          dataDistribution[node].push_back(sampleLeafElements[tree][node][s]);     //appends the previous sample elements into current list and it together gets splitted into new children
        }
        //NodeSize[tree][node] += sampleLeafElements[tree][node].size();
        sampleLeafElements[tree][node].resize(0);    //since this node is no longer a leaf node
        
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






//updates treeNode and TreeNodeMass Matrix
void incGenericArrayUpdate(int insts){
  //int NumInst = ntree*nrow;
  int maxNumOfNodes = (int) pow(2.0, (double)(h_max+1))-1;
  //cout<<"Max Nodes: "<<maxNumOfNodes<<endl;
  

  

  vector <int > treeNodeTemp;
  treeNodeTemp.resize(ntree);

  vector <double > massVectorTemp;

  for(int i=nInst;i<nInst+insts;i++){
    treeNode.push_back(treeNodeTemp);
    massVectorTemp.resize(i+1);
    massMatrix.push_back(massVectorTemp);
  }

  treeNodeTemp.clear();
  vector<int> ().swap(treeNodeTemp);

  massVectorTemp.clear();
  vector<double> ().swap(massVectorTemp);


  /*for(int i=0;i<nInst+insts;i++){
	cout << i << "     ";
	for(int j=0;j<ntree;j++){
		cout << treeNode[i][j] << "  "  ;
	}
	cout << endl;
  }*/
  
  
  //cout << massMatrix[3000][200] << endl;

  //cout << "Temp " << treeNode.size() << " " << treeNode[0][10] << " "<< treeNode[nInst+5][10] << endl;

  int heightNode;
  vector< int> oldTreeNodeMass;
  vector< int> oldTreeNode;

  double  newRatio, oldRatio;
  int  a, b;

 
  for(int tree = 0;tree<ntree;tree++){
     


    for(int n=0;n<maxNumOfNodes;n++){
  	oldTreeNodeMass.push_back(treeNodeMass[tree][n]);
    }
  	
    for(int i=0;i<nInst;i++){
  	oldTreeNode.push_back(treeNode[i][tree]);
    }
 
    //dataDistribution.clear();
    dataDistribution.resize(maxNumOfNodes);
    //dataDistribution[0].resize(nInst+insts);
    for(int i=0;i<nInst+insts;i++){
      dataDistribution[0].push_back(i);
    }  

    //cout << massMatrix[1][1] << endl;

    for(int node=0;node<maxNumOfNodes;node++){
      heightNode = (int)(log2((double)(node+1)));


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


      treeNodeMass[tree][node] = dataDistribution[node].size();
      newRatio = (double)treeNodeMass[tree][node]/ntree;

      //cout<<oldTreeNodeMass[node] << " " << treeNodeMass[tree][node]<<endl;

      //no new points entered into this node
      if(oldTreeNodeMass[node] == treeNodeMass[tree][node]){
	if(heightNode<h_max){
          dataDistribution[2*node+1].resize(0);
          dataDistribution[2*node+2].resize(0);
        }
	dataDistribution[node].clear();
        //dataDistribution[node].shrink_to_fit();
        //vector<int> ().swap(dataDistribution[node]);
	//cout << node << endl;
	continue;
      }


      //for(int i=0;i<dataDistribution[node].size();i++){
      //  leafElements[tree][node].push_back(dataDistribution[node][i]);    
      //}

      
      if(NodeSize[tree][node]==1 || heightNode == h_max ){    //this condition tells that the node is a leaf node
        //cout<<tree<<" "<<node<<" if"<<endl;
        for(int l=0;l<dataDistribution[node].size();l++){
        	//leafElements[tree][node].push_back(dataDistribution[node][i]); 
          	treeNode[dataDistribution[node][l]][tree] = node;    //stores the min region node of element in the node

		//if(dataDistribution[node][l]>=nInst)
            	//	continue;
            	for(int r=l;r<dataDistribution[node].size();r++){
                	//if(dataDistribution[node][r]<nInst){
			   a = dataDistribution[node][l];
			   b = dataDistribution[node][r];

			   


                    	   if(a >= nInst || b >= nInst){
				if(a >= b){
				   	massMatrix[a][b] += newRatio;
					
				}
       				else{
					massMatrix[b][a] += newRatio;
							
				}
			   }
			   else{
				if(oldTreeNode[a] >= oldTreeNode[b]) 
                    			oldRatio = oldTreeNodeMass[LCA[oldTreeNode[a]][oldTreeNode[b]]];
			   	else
					oldRatio = oldTreeNodeMass[LCA[oldTreeNode[b]][oldTreeNode[a]]];
                    	   	oldRatio = (double)oldRatio/ntree;

                    	  	if(a >= b){
				   	massMatrix[a][b] += (newRatio - oldRatio);
					
				}
       				else{
					massMatrix[b][a] += (newRatio - oldRatio);
							
				}
			   		 
                	   }
			
		}

        }
        if(heightNode<h_max){
	  //for(int i=0;i<dataDistribution[node].size();i++){
        	//leafElements[tree][node].push_back(dataDistribution[node][i]); 
          //}
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

	for(int l=0;l<dataDistribution[2*node+1].size();l++){
          	//if(dataDistribution[2*node+1][l]>=nInst)
            	//	continue;
          	for(int r=0;r<dataDistribution[2*node+2].size();r++){
            		//if(dataDistribution[2*node+2][r] < nInst){
			  a = dataDistribution[2*node+1][l];
  			  b = dataDistribution[2*node+2][r];
 
			  if(a >= nInst || b >= nInst){
				if(a >= b){
				   	massMatrix[a][b] += newRatio;
					
				}
       				else{
					massMatrix[b][a] += newRatio;
							
				}
			   }
			   else{
				if(oldTreeNode[a] >= oldTreeNode[b]) 
                    			oldRatio = oldTreeNodeMass[LCA[oldTreeNode[a]][oldTreeNode[b]]];
			   	else
					oldRatio = oldTreeNodeMass[LCA[oldTreeNode[b]][oldTreeNode[a]]];
                    	   	oldRatio = (double)oldRatio/ntree;

                    	  	if(a >= b){
				   	massMatrix[a][b] += (newRatio - oldRatio);
					
				}
       				else{
					massMatrix[b][a] += (newRatio - oldRatio);
							
				}
			   		 
                	   }       
		}
       	}

      }
      dataDistribution[node].clear();
      //dataDistribution[node].shrink_to_fit();
      //vector<int> ().swap(dataDistribution[node]);
    }
    dataDistribution.clear();
    //dataDistribution.shrink_to_fit();
    //vector< vector<int> > ().swap(dataDistribution);
    oldTreeNodeMass.clear();
    oldTreeNode.clear();

    
  }
   

  /*cout << "TreeNode Static" << endl;
  for(int i=0;i<nInst+insts; i++){
     cout << i << "   " ;
     for(int j=0;j<ntree;j++)
	cout << treeNode[i][j] << " ";
     cout << endl;
  }
  cout << "--------------------" << endl;


  cout << "TreeNodeMass Static" << endl;
  for(int i=0;i<maxNumOfNodes; i++){
        cout << i << "    " ;
	for(int j=0;j<ntree;j++)	
		cout << treeNodeMass[j][i] <<  " ";
	cout << endl;
  }
  cout << "--------------------" << endl;*/

  
}


void incDissScore(int ins, string massOutFile){


  //vector <double > massVectorTemp;
  //double mass=0;

  /*for(int i=0;i<nInst;i++){
    for(int j=0;j<=i;j++){
      mass = 0;
      for(int tree=0;tree<ntree;tree++){
	  if(treeNode[i][tree] >= treeNode[j][tree])
             mass += (double)treeNodeMass[tree][LCA[treeNode[i][tree]][treeNode[j][tree]]]/ntree;
	  else
	     mass += (double)treeNodeMass[tree][LCA[treeNode[j][tree]][treeNode[i][tree]]]/ntree;
      }
        
     
      massMatrix[i][j] = mass;
      
    }
  }*/


  /*for(int i=nInst;i<nInst+ins;i++){
    for(int j=0;j<=i;j++){
      mass = 0;
      for(int tree=0;tree<ntree;tree++){
	  if(treeNode[i][tree] >= treeNode[j][tree])
             mass += (double)treeNodeMass[tree][LCA[treeNode[i][tree]][treeNode[j][tree]]]/ntree;
	  else
	     mass += (double)treeNodeMass[tree][LCA[treeNode[j][tree]][treeNode[i][tree]]]/ntree;
      }
        
     
      massVectorTemp.push_back(mass);
      
    }
    
    massMatrix.push_back(massVectorTemp);  
    massVectorTemp.clear();
  }*/




  //string massOutFile = "massInc";
  ofstream massFile(massOutFile.c_str(),ios::out);
  massFile<<""; 
  massFile.close();
  //Write the root node to a file
  massFile.open(massOutFile.c_str(),ios::app);

  for(int i=0;i<nInst+ins;i++){
     for(int j=0;j<=i;j++){
	massFile << i << " " << j << " " << massMatrix[i][j] << endl;
     }
     //massFile << endl;

  }
  massFile.close();

//cout << "mass: " << massMatrix[1100][110] << endl;

}













