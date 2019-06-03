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
    string massFile1 = argv[1]; 
    string massFile2 = argv[2];
    //double fAvg = (double)0;
    
    ifstream in(massFile1.c_str()); 
    if(!in)
    {
      cout<<"Cannot open the input file: "<<massFile1<<endl;
      exit(0);
    } 

     ifstream in1(massFile2.c_str()); 
    if(!in1)
    {
      cout<<"Cannot open the input file: "<<massFile2<<endl;
      exit(0);
    } 

    double maxError = atof(argv[3]);
    
    int a=0,b=0;
    double a1,b1,c1,c2;

    while(in >> a1 >> b1 >> c1){
	in1 >> a1 >> b1 >> c2;

	if( c2 >= (double)(1-maxError)*c1  && c2 <= (double)(1+maxError)*c1 )
            a++;
        else 
            b++;
       
    }


    double fMeasure = (double) a/(a+b);
    cout <<massFile1 << " " << massFile2 << " " << fMeasure << endl;
}




