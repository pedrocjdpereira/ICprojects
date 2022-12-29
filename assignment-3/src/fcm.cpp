#include<string>
#include<sstream>
#include<fstream>
#include<iostream>
#include<cmath>
#include "fcm.h"

using namespace std;


// int main(void){
// 	cout << "Hello!\n";

// 	int k = 6;
// 	double alfa = 10;
// 	map<string,map<char,int>> model;
// 	const char* fileInName = "models/OsLusiadas.txt";
// 	const char* fileOutName = "models/OsLusiadas_model.txt";
// 	const char* exampleTextFile = "models/example.txt";

// 	fcm obj = fcm(k, alfa);

// 	cout << "Building model...\n";
// 	obj.modelBuilder(model, fileInName, fileOutName);

// 	map<string,map<char,int>> modelCopy;
// 	cout << "Loading model...\n";
// 	obj.modelLoader(modelCopy, fileOutName);
// 	cout << "Are they equal? -> " << (equal(model.begin(), model.end(), modelCopy.begin()) == 1 ? "Yes" : "No") << "\n";
	
// 	obj.calcEntropy(model);
// 	cout << "Entropy of the model: " << obj.modelEntropy << "\n";

// 	obj.estimateEntropy(model,exampleTextFile);
// 	cout << "Entropy of the example text: " << obj.textEntropy << "\n";
// 	cout << "Num of chars: " << obj.numChars << "\n";
// 	return 0;
// }


void readChar(ifstream &InFile, char* c){
	char aux;
	InFile.get(aux);
	*c = aux;
	while((aux == '\n') || (aux == '\t')){
		if(InFile.eof()){
			*c = '\n';
			return;
		}
		InFile.get(aux);
		*c = aux;
	}
}

fcm::fcm(int kIn, double alfaIn){
	k = kIn;
	alfa = alfaIn;
}

void fcm::modelBuilder(map<string,map<char,int>> &model, const char* fileInName, const char* fileOutName){
	ifstream InFile;
	InFile.open(fileInName,ios::in);
	if(!InFile.is_open()) throw runtime_error("Invalid file");

	string context;
	char c;

	int i = 0;
	while(i < k){
		readChar(InFile, &c);
		context.append(1,c);
		i++;
	}

	while(!InFile.eof()){
		readChar(InFile, &c);
		if(c == '\n'){
			break;
		}
		if(model.find(context) != model.end()) model[context][c]++;
		else{
			map<char,int> newmap;
			model[context] = newmap;
			model[context][c]++;
		}
		context.erase(0,1);
		context.append(1,c);
	}
	
	ofstream OutFile;
	OutFile.open(fileOutName, ios::out);

	OutFile << k << ',';
	OutFile << alfa << '\n';

	for(auto it1: model){
		OutFile << it1.first << '\t';
		for(auto it2: it1.second){
			OutFile << it2.first << '\t' << it2.second << '\t';
		}
		OutFile << '\n';
	}
	
	OutFile.close();
}

void fcm::modelLoader(map<string,map<char,int>> &model, const char* fileInName){
	ifstream InFile;
	InFile.open(fileInName,ios::in);
	if(!InFile.is_open()) throw runtime_error("Invalid file");

	string line;

	getline(InFile,line);
	istringstream my_stream(line);

	getline(my_stream, line, ',');
	k = stoi(line);
	getline(my_stream, line, '\t');
	alfa = stod(line);

	while(getline(InFile,line)){
		istringstream my_stream(line);
		getline(my_stream, line, '\t');
		while(line == ""){
			getline(my_stream, line, '\t');
		}
		string context = line;
		while(getline(my_stream, line, '\t')){
			char c = line[0];
			getline(my_stream, line, '\t');	
			int n = stoi(line);
			model[context][c] = n;
		}
	}
}

void fcm::calcEntropy(map<string,map<char,int>> &model){
	modelEntropy = 0;
	int totalCharCount = 0;
	map<string,int> charsInContext;


	for(auto it1: model){
		string context = it1.first;
		int count = 0;
		for(auto it2: it1.second){
			count += it2.second;
		}
		charsInContext[context] = count;
		totalCharCount += count;
	}

	for(auto it1: model){
		string context = it1.first;
		double probContextInModel = (double) charsInContext.at(context)/totalCharCount;
		
		double contextEntropy = 0;
		for(auto it2: it1.second){
			double probCharInContext = (double) it2.second/charsInContext.at(context);
			contextEntropy -= probCharInContext*log2(probCharInContext);
		}
		modelEntropy += contextEntropy*probContextInModel;
	}
}

void fcm::estimateEntropy(map<string,map<char,int>> &model, const char* fileInName){
	ifstream InFile;
	InFile.open(fileInName,ios::in);
	if(!InFile.is_open()) throw runtime_error("Invalid file");
	
	textEntropy = 0;
	numChars = 0;
	string context;
	char c;

	int i = 0;
	while(i < k){
		readChar(InFile, &c);
		context.append(1,c);
		i++;
	}

	while(!InFile.eof()){
		readChar(InFile, &c);
		numChars++;

		int charsInContext = 0;
		int n = 0;

		if(model.find(context) != model.end()){
			if(model.at(context).find(c) != model.at(context).end()){
				n = model.at(context).at(c);
			}
			for(auto it: model.at(context)){
				charsInContext += it.second;
			}
		}

		textEntropy += -log2( (n + alfa) / (charsInContext + alfa*27) );

		context.erase(0,1);
		context.append(1,c);
	}

	distance = textEntropy;

	textEntropy = distance/numChars;
}
