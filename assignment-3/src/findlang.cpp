#include "../src/fcm.cpp"
#include <iostream>
#include <map>
#include <string>
#include <ctype.h>
#include <limits>
using namespace std;

int main(int argc , char** argv){
	if(argc<2){
		cout << "ERROR : Must provide at least 2 files" << endl;
		return 0;
	}

	int k =-1;
	int a= 0;
	char option = 'o';

	do
	{
		cout << "Load Models: y-YES/ n- NO: ";
		cin >>  option;
	}while (('n'!= tolower(option)) & ('y'!= tolower(option)));
	
	if (option== 'n') //user can insert custom values
	{
		if (argc <3)
		{
			cout << "Must provide 3 files"; //file to be analised, new model file , file to be analised
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

	fcm f = fcm(k,a);
	int dist = INT32_MAX;
	string language;
	int i =2;
	while (i < argc -1)
	{	
		cout <<"Model" << argv[i] <<endl;
		map<string, map<char, int>> model;

		if (option=='y')
		{
			f.modelLoader(model,argv[i]);//load exisiting model(model)
			f.estimateEntropy(model,argv[1]);
			if (f.distance < dist)
			{
				language = argv[i];
				dist = f.distance;
			}	
			i++;
		}
		else{
			f.modelBuilder(model,argv[i],argv[i+1]);//create model with values defined above
			f.estimateEntropy(model,argv[1]);
			if (f.distance < dist)
			{
				language = argv[i+1];
				dist = f.distance;
			}
			i+=2;	
		}
		cout <<"Loaded successfully" <<endl;
	}
	cout << "Text was written in: " <<language <<endl;
	return 0;
}
	