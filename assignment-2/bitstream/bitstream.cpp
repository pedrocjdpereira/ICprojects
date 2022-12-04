#include <iostream>
#include <fstream>
#include "bitstream.hpp"

using namespace std;

BitStream::BitStream(){}

BitStream::BitStream(const char *fname, char mode) {
	if(mode == 'w')
		stream = fstream(fname, ios::binary | ios::out);
	else if (mode == 'r')
		stream = fstream(fname, ios::binary | ios::in);
	else
		throw runtime_error("Invalid opening mode for file!");
			
	if(!stream.is_open())
		throw runtime_error("Could not open given file!");
			
	BitStream::mode = (mode == 'w') ? 1 : 0;
	buffer = 0;
	pointer = (mode == 'w') ? 8 : 0;

	// get length of file in bytes
	stream.seekg(0, stream.end);
	size = stream.tellg();
	stream.seekg(0, stream.beg);
}

char BitStream::readBit() {
	if(mode != 0)
		throw runtime_error("File is not open on read mode!");
			
	// populate buffer if empty
	if(--pointer == -1) {
		stream.read(&buffer, 1);
		pointer = 7;
	}

	char res = (buffer >> pointer) & 0x01;

	return res;
}

void BitStream::readNBits(char *bits, int n) {
	if(mode != 0)
		throw runtime_error("File is not open on read mode!");
			
	for(int i = 0; i < n; i++)
		bits[i] = readBit();
}

string BitStream::readNBits(int n) {
	if(mode != 0)
		throw runtime_error("File is not open on read mode!");
			
	string res = "";
	for(int i = 0; i < n; i++)
		res += readBit() + '0';

	return res;
}

void BitStream::writeBit(char bit) {
	if(mode != 1)
		throw runtime_error("File is not open on write mode!");

	// If last bit in buffer is being populated, empty buffer to file
	if(--pointer == 0) {
		buffer |= (bit & 0x01);
		stream.write(&buffer, 1);
		pointer = 8;
		buffer = 0; // clean buffer to prevent writing wrong bits if file is closed before EOF
	} else
		buffer |= (bit & 0x01) << pointer;
}

void BitStream::writeNBits(char *bits, int n) {
	if(mode != 1)
		throw runtime_error("File is not open on write mode!");
			
	for(int i = 0; i < n; i++)
		writeBit(bits[i]);
}

void BitStream::writeNBits(string bits) {
	if(mode != 1)
		throw runtime_error("File is not open on write mode!");
	
	for(int i = 0; i < (int)bits.length(); i++)
		writeBit(bits.at(i) - '0');
}

long long BitStream::fileSize() {
	return size;
}

void BitStream::close() {
	// empty buffer to file if on writing mode and buffer not empty
	if(mode == 1 && pointer != 8)
		stream.write(&buffer, 1);

	stream.close();
}
