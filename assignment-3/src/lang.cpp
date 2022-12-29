#include "../src/fcm.cpp"
#include <iostream>
#include <map>
#include <string>
#include <ctype.h>
#include<iomanip>
using namespace std;

int main(int argc , char** argv){
	if(argc<2){
		cout << "ERROR : Must provide 2 files" << endl;
		return 0;
	}

	int k =-1;
	int a= -1;
	char option = 'o';
	string fileOut;

	do
	{
		cout << "Load Models: y-YES/ n- NO: ";
		cin >>  option;
	}while (('n'!= tolower(option)) & ('y'!= tolower(option)));
	
	if (option== 'n') //user can insert custom values
	{	
		if (argc <3)
		{
			cout << "Must provide 3 files"; //original file , new model file , file to be analised
			return 0;
		}
		
		while (a<=0)
		{
			cout << "Choose alpha value: ";
			cin >> a;
		}

		while (k <=0)
		{
			cout << "Choose context order value: ";
			cin >> k;
		}
	}

		map<string, map<char, int>> model;
		fcm f = fcm(k,a);

		if (option=='y')
		{	
			f.modelLoader(model,argv[1]); //load exisiting model
			f.estimateEntropy(model,argv[2]);
		}
		else{
			f.modelBuilder(model,argv[1],argv[2]);//create model with values defined above
			f.estimateEntropy(model,argv[3]);
		}
		cout << "Need " << setprecision(4) << scientific << f.distance << " bits to compress the file" << endl;
	return 0;
}
