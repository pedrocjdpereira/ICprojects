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
		buffer = 0x00;
		rdpointer = 0;
		wrpointer = 0;
	}

	void readBit(unsigned char bit){
		rdpointer--;
		if(rdpointer == -1){
			rdpointer = 7;
		}
		buffer = (buffer | ((bit & 0x01) << rdpointer));
	}

	unsigned char writeBit(){
		rdpointer--;
		if(rdpointer == -1){
			rdpointer = 7;
		}
		return ((buffer >> rdpointer) & 0x01);
	}

	void readNbits(fstream* ft, fstream* fb, char* bitArray, int size){
		unsigned char c;
		if(size != 8){
			return;
		}
		if(ft->peek() == EOF){
			*bitArray = '\n';
			return;
		}
		int i = 0;
		buffer = 0x00;
		while(i < size){
			ft->read(reinterpret_cast<char*>(&c), 1);
			readBit(c);
			i++;
		}
		*bitArray = buffer;
		fb->write(reinterpret_cast<char*>(&buffer), 1);
	}

	void writeNbits(fstream* fb, fstream* ft, char* bitArray, int size){
		unsigned char c;
		if(size != 8){
			return;
		}
		if(fb->peek() < 0){
			*bitArray = '\n';
			return;
		}
		int i = 0;
		fb->read(reinterpret_cast<char*>(&buffer), 1);
		while(i < size){
			*ft << (writeBit() & 0x01);
			i++;
		}
		buffer = 0x00;
	}
};
