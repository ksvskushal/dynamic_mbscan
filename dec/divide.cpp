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

using namespace std;

void divide(string fileName, int per){
	int n, a;

	string fname = fileName;
	ifstream in(fname);

	in >> n >> a;

	int n1 = (per * n)/ 100;
	int n2 = n - n1;

	double temp;
	
	fname = fileName + to_string(100-per);
	ofstream o2(fname);
	o2 << n2 << " " << a << endl;

	for(int i = 0; i < n2; i++){
		for(int j = 0; j < a+1; j++){
			in >> temp;
			o2 << temp << " ";
		}
		o2 << endl;
	}

	fname = fileName + to_string(per);
	ofstream o1(fname);
	o1 << n1 << " " << a << endl;

	for(int i = 0; i < n1; i++){
		for(int j = 0; j < a+1; j++){
			in >> temp;
			o1 << temp << " ";
		}
		o1 << endl;
	}

	
	in.close();
	o1.close();
	o2.close();
}

int main(int argc, char const *argv[])
{
	int per = atoi(argv[1]);

	string fname = "a";
	divide(fname, per);

	fname = "b";
	divide(fname, per);

	fname = "c";
	divide(fname, per);

	fname = "d";
	divide(fname, per);

	fname = "e";
	divide(fname, per);
	
	return 0;
}