#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <iostream>
#include <fstream>

using namespace std;

constexpr unsigned int BUFFER_SIZE = 8;

class BitStream {
	private:
		fstream stream;
		char buffer;
		int pointer;
		int mode;
		long long size;
	
	public:
	 	/**
	 	 * @brief Construct a new Bit Stream object
	 	 * 
	 	 */
		BitStream();
		
		/**
		 * @brief Construct a new Bit Stream object
		 * 
		 * @param fname Filename to read/write bits from/to.
		 * @param mode Mode of operation for the bitstream (read='r' or write='w')
		 */
		BitStream(const char *fname, char mode);

		/**
		 * @brief Read a bit from file.
		 * Read a bit from file. Only works if object in read mode.
		 * @return char Bit read.
		 */
		char readBit();

		/**
		 * @brief Read n bits from file.
		 * Read n bits from file. Only works if object in read mode.
		 * @param bits Array to store bits read.
		 * @param n Number of bits to read.
		 */
		void readNBits(char *bits, int n);

		/**
		 * @brief 
		 * 
		 * @param n 
		 * @return string 
		 */
		string readNBits(int n);

		/**
		 * @brief Write a bit to file.
		 * Write a bit to file. Only works if object in write mode.
		 * @param bit Bit to write.
		 */
		void writeBit(char bit);

		/**
		 * @brief Write n bits to file.
		 * Write n bits to file. Only works if object in write mode.
		 * Each element of the array passed as argument must represent only one bit.
		 * @param bits Array with each bit to be written to file.
		 * @param n Number of bits to write to file.
		 */
		void writeNBits(char *bits, int n);

		/**
		 * @brief 
		 * 
		 * @param bits 
		 * @param n 
		 */
		void writeNBits(string bits);

		/**
		 * @brief Get size of file in bytes.
		 * 
		 * @return long long Size of file.
		 */
		long long fileSize();

		/**
		 * @brief Close bitstream.
		 * 
		 */
		void close();
};

#endif
