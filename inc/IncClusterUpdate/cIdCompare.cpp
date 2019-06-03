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

int main(int argc, char* argv[]){
    string fClusterStatic = argv[1];   
    string fClusterInc = argv[2]; 

    

    ifstream in(fClusterStatic.c_str()); 
    if(!in)
    {
      cout<<"Cannot open the input file: "<<fClusterStatic<<endl;
      exit(0);
    } 

    
    

    ifstream in1(fClusterInc.c_str()); 
    if(!in1)
    {
      cout<<"Cannot open the input file: "<<fClusterInc<<endl;
      exit(0);
    } 

    
    int a=0,b=0;
    int a1,b1,c1,c2;

    while(in >> a1 >> b1 >> c1){
	in1 >> a1 >> b1 >> c2;

	if(c1 == c2)
            a++;
        else 
            b++;
       
    }


    double fMeasure = (double) a/(a+b);
    cout <<fClusterStatic << " " << fClusterInc << " " << fMeasure << endl;


    return 0;
}
