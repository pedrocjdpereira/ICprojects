#include <iostream>
#include <fstream>

using namespace std;

class BitStream {
  private:
	unsigned char buffer;
	int rdpointer;
	int wrpointer;

  public:
	BitStream() {
	buffer = 0;
	rdpointer = 8;
	wrpointer = 8;
	}

	unsigned char readBit(){
		rdpointer++;
		if(rdpointer == 8){
		rdpointer = 0;
		}
		return (buffer >> rdpointer) & 1;
	}

	void writeBit(char bit){
		wrpointer++;
		if(wrpointer == 8){
		wrpointer = 0;
		}
		buffer = buffer | (bit << wrpointer);
	}

	void readNbit(fstream* f, char* bitArray, int size){
		if(size != 8){
			return;
		}
		int i = 0;
		f->read(reinterpret_cast<char*>(&buffer), 1);
		while(i < size){
			bitArray[i] = readBit();
			i++;
		}
		buffer = 0;
	}

	void writeNbits(fstream* f, char* bitArray, int size){
		if(size != 8){
			return;
		}
		int i = 0;
		buffer = 0;
		while(i < size){
			writeBit(bitArray[i]);
			i++;
		}
		f->write(reinterpret_cast<char*>(&buffer), 1);
	}
};
