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


using namespace std;


double fMatrix[10][10]; // 10*10 matrix where each cell (i,j) indicates f value of inc run i w.r.t. static run j
void fMeasureValue(string incFile, int i, int j);

int main(int argc, char* argv[]){
    string incFile = argv[1]; 
    double fAvg = (double)0;
    for(int i=0;i<10;i++){
	for(int j=0;j<10;j++){
	    fMeasureValue(incFile, i, j);
	    //cout << fMatrix[i][j] << endl;
	    fAvg += fMatrix[i][j];
	}
    }
    fAvg = (double)fAvg;///100;
    cout << fAvg << endl;
}


void fMeasureValue(string incFile, int i, int j){
    stringstream ss1, ss2;
    ss1 << i;
    ss2 << j;
    string fClusterInc = incFile.substr(0, incFile.find("_")) + "_compareCIdInc"+ss1.str();   
    string fClusterStatic = incFile.substr(0, incFile.find("_")) + "_compareCId"+ss2.str();
    //int nInst = 150; 

    //vector< vector< int> > incCluster;
    //vector< vector< int> > staticCluster;
    
    

    ifstream in(fClusterInc.c_str()); 
    if(!in)
    {
      cout<<"Cannot open the input file: "<<fClusterInc<<endl;
      exit(0);
    } 

     ifstream in1(fClusterStatic.c_str()); 
    if(!in1)
    {
      cout<<"Cannot open the input file: "<<fClusterStatic<<endl;
      exit(0);
    } 


    
    int a=0,b=0,c=0;
    int a1,b1,c1,c2;

    while(in >> a1 >> b1 >> c1){
	in1 >> a1 >> b1 >> c2;

	if((c1 == 1 && c2 == 1) || (c1 == 0 && c2 == 0))
            a++;
        else if(c1 == 1 && c2 == 0)
            b++;
        else if(c1 == 0 && c2 == 1)
            c++;       
                       
        
    }


    double fMeasure = (double) a/(a+b+c);

    fMatrix[i][j] = fMeasure;
 
}

