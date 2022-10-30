#include <iostream>
#include <fstream>

using namespace std;

class BitStream {
  private:
	unsigned char buffer;
	int pointer;

  public:
	BitStream() {
		buffer = 0x00;
		pointer = 0;
	}

	void readBit(unsigned char bit){
		pointer--;
		if(pointer == -1){
			pointer = 7;
		}
		buffer = (buffer | ((bit & 0x01) << pointer));
	}

	unsigned char writeBit(fstream* fb){
		pointer--;
		if(pointer == -1){
			fb->read(reinterpret_cast<char*>(&buffer), 1);
			pointer = 7;
		}
		return ((buffer >> pointer) & 0x01);
	}

	void readNbits(fstream* ft, fstream* fb, char* bitArray, int size){
		unsigned char c;
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
		unsigned char* c = (unsigned char*) malloc(sizeof(char));
		int i = 0;
		while(i < size){
			*c = (writeBit(fb) & 0x01);
			if(fb->peek() < 0){
				*bitArray = '\n';
				return;
			}
			*ft << (*c & 0x01);
			i++;
		}
		free(c);
	}
};
