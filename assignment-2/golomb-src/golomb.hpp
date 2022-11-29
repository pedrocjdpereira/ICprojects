/**
 * @file golomb.hpp
 * @author Diogo Jesus (diogopjesus@ua.pt)
 * @author Pedro Pereira (pedrocjdpereira@ua.pt)
 * @author David Bicho (david13122000@ua.pt)
 * @brief Golomb coding implementation
 * @date 2022-11-21
 */

#ifndef GOLOMB_H
#define GOLOMB_H

#include <string>

using namespace std;

class Golomb {
	private:
	 	int i;
		unsigned m=0, q, r, b;

	public:
		/**
		 * @brief Construct a new Golomb object
		 * 
		 */
		Golomb();
		
		/**
		 * @brief Construct a new Golomb object
		 * 
		 * @param m tunable parameter
		 */
		Golomb(unsigned m);
		
		/**
		 * @brief Encode an integer using golomb coding
		 * 
		 * @param num integer to be enconded
		 * @return string
		 */
		string encoder(int num);
		
		/**
		 * @brief Decode a binary string obtained using golomb coding 
		 * 
		 * @param bits string of bits to be decoded
		 * @return int 
		 */
		int decoder(string bits);

		/**
		 * @brief Set the value of m
		 * 
		 * @param m tunable parameter
		 */
		void setM(unsigned m);

		/**
		 * @brief Convert an unsigned integer to its binary representation
		 *
		 * @param num value to be converted
		 * @return string
		 */
		string uns2bin(unsigned num);

		/**
		 * @brief Convert an unsigned integer to its unary representation
		 * 
		 * @param num value to be converted
		 * @return string
		 */
		string uns2una(unsigned num);

		/**
		 * @brief Convert a binary string to an unsigned integer
		 * 
		 * @param bin binary string to be converted
		 * @return unsigned
		 */
		unsigned bin2uns(string bin);

		/**
		 * @brief Convert an unary string to an unsigned integer 
		 * 
		 * @param una unary string to be converted
		 * @return unsigned
		 */
		unsigned una2uns(string una);
};

#endif // GOLOMB_H
