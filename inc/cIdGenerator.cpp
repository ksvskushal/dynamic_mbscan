#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char* argv[]){

	string fClusterStatic = argv[1]; 

	ifstream in(fClusterStatic.c_str()); 
    if(!in)
    {
      cout<<"Cannot open the input file: "<<fClusterStatic<<endl;
      exit(0);
    } 

   vector< vector< int> > staticCluster;
    	
    int nInst,nAttr;
    in >> nInst >> nAttr;
    //cout << nInst << " " << nAttr << endl;
	staticCluster.resize(nInst);

    for(int i=0;i<nInst;i++){
    	float temp;
    	int cId;
        for(int j=0;j<nAttr;j++)
            in >> temp;
        in >> cId;
        
        staticCluster[i].push_back(i);
        staticCluster[i].push_back(cId);
    }

    //cout << staticCluster[0][1] << " " << staticCluster[89][1]<< endl;

	  
    string compareFile = fClusterStatic+"_compareCIdOrig";
    ofstream compareoutFile(compareFile.c_str(),ios::out);
    compareoutFile<<""; 
    compareoutFile.close();
    //Write the root node to a file
    compareoutFile.open(compareFile.c_str(),ios::app);
    for(int i=0;i<nInst;i++){
      for(int j=i+1;j<nInst;j++){
        //if(clusterId[i] == -2 || clusterId[j] == -2)
        //  compareoutFile << i << " " << j << " " << "2" << endl;
        //else 
        if(staticCluster[i][1] == staticCluster[j][1])
          compareoutFile << i << " " << j << " " << "1" << endl; 
        else
          compareoutFile << i << " " << j << " " << "0" << endl; 
        }
    }
    compareoutFile.close();
    
}
