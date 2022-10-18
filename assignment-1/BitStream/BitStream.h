#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

constexpr size_t BUFFER_SIZE = 8;

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
    if(size % 8 != 0){
      return;
    }
    int i = 0;
    while(i < size/8){
      int j = (i-1)*8;
      f->read(reinterpret_cast<char*>(&buffer), 1);
      while(j < i*8){
        bitArray[j] = readBit();
      }
      buffer = 0;
      i++;
    }
  }

  void writeNbits(fstream* f, char* bitArray, int size){
    if(size % 8 != 0){
      return;
    }
    int i = 0;
    while(i < size/8){
      int j = (i-1)*8;
      buffer = 0;
      while(j < i*8){
        writeBit(bitArray[j]);
      }
      f->write(reinterpret_cast<char*>(&buffer), 1);
      i++;
    }
  }
};