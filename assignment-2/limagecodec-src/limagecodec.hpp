/**
 * @file limagecodec.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-01
 */

#ifndef LIMAGECODEC_H
#define LIMAGECODEC_H

#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "../golomb/golomb.cpp"
#include "../bitstream/bitstream.cpp"

using namespace std;
using namespace cv;

class LosslessImageCodec {
	private:
		Mat image;
		Golomb gol;
		BitStream ori, des;

	public:
		/**
		 * @brief Construct a new Lossless Image Codec object.
		 * 
		 */
		LosslessImageCodec();

		/**
		 * @brief Construct a new Lossless Image Codec object.
		 * Construct a new Lossless Image Codec object.
		 * This codec supports ppm files as source to be encoded.
		 * @param fname filepath to an image to be encoded/decoded.
		 * @param mode mode to execute ('e' for encoding and 'd' for decoding)
		 */
		LosslessImageCodec(const char *fname, char mode);

		/**
		 * @brief Encode an image file based on golomb coding of the prediction residuals
		 * 
		 * @param fname filepath to the encoded image.
		 */
		void encode(const char *fname);

		/**
		 * @brief Decode an image file.
		 * 
		 * @param fname filepath to the decoded image.
		 */
		void decode(const char *fname);

		/**
		 * @brief JPEG-LS predictor implementation
		 * 
		 * @param a 
		 * @param b 
		 * @param c 
		 * @return int 
		 */
		int predictorJLS(int a, int b, int c);

		/**
		 * @brief 
		 * 
		 * @param channel 
		 * @return vector<int> 
		 */
		vector<int> residuals(Mat channel);

		/**
		 * @brief 
		 * 
		 * @param val 
		 * @param encode 
		 */
		void writeVal(int val, bool encode = true);

		/**
		 * @brief 
		 * 
		 * @param size 
		 * @param encoded 
		 * @return int 
		 */
		int readVal(int size = 0, bool encoded = true);

		/**
		 * @brief 
		 * 
		 * @param res 
		 * @param rows 
		 * @param columns 
		 * @return Mat 
		 */
		Mat restore(vector<int> res, int rows, int columns);
};

#endif // LIMAGECODEC_H
