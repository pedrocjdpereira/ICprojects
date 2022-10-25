#include <vector>
#include <sndfile.hh>
#include <math.h>
#include "BitStream.h"

using namespace std;

constexpr int ARRAY_SIZE = 8;

int main(int argc, char *argv[])
{

	if (argc < 4)
	{
		cerr << "Usage:  <original file> <new file> <option>\nOptions:\n0-Text to Binary\n1-Binary to Text\n";
		return 1;
	}

	BitStream bs { };
	
	char* bitArray = (char*) malloc(sizeof(char));

	*bitArray = 0x00;

	fstream txtfile;
	fstream binfile;

	int option = atoi(argv[argc-1]);

	if(option < 0 | option > 1){
		return 0;
	}
	else if(option == 0){
		txtfile.open(argv[argc-3], ios::in);
		binfile.open(argv[argc-2], ios::out | ios::binary);
		while(*bitArray != '\n'){
			bs.readNbits(&txtfile, &binfile, bitArray, ARRAY_SIZE);
		}
	}
	else{
		binfile.open(argv[argc-3], ios::in | ios::binary);
		txtfile.open(argv[argc-2], ios::out);
		int i = 0;
		while(*bitArray != '\n'){
			i++;
			bs.writeNbits(&binfile, &txtfile, bitArray, ARRAY_SIZE);
		}
	}

	binfile.close();
	txtfile.close();

	return 0;
}
