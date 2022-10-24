#include <vector>
#include <sndfile.hh>
#include <math.h>
#include "BitStream.h"

using namespace std;

constexpr size_t ARRAY_SIZE = 8;

int main(int argc, char *argv[])
{

	if (argc < 4)
	{
		cerr << "Usage:  <original file> <encoded file> <decoded file>\n";
		return 1;
	}

	BitStream bs { };
	
	char bitArray = 'c';
		
	fstream ogfile;
  	ogfile.open(argv[argc -3]);
	
	fstream encodfile;
	encodfile.open(argv[argc - 2]);

	fstream decodfile;
	decodfile.open(argv[argc - 1]);

	bs.readNbit(&ogfile, &bitArray, ARRAY_SIZE);

	printf("%c", &bitArray);

	bs.writeNbits(&encodfile, &bitArray, ARRAY_SIZE);
	bs.readNbit(&encodfile, &bitArray, ARRAY_SIZE);
	bs.writeNbits(&decodfile, &bitArray, ARRAY_SIZE);

	ogfile.close();
	encodfile.close();
	decodfile.close();

	return 0;
}
