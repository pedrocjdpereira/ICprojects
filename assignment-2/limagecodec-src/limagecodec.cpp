#include <iostream>
#include "limagecodec.hpp"

using namespace std;
using namespace cv;

LosslessImageCodec::LosslessImageCodec(){}

LosslessImageCodec::LosslessImageCodec(const char *fname, char mode) {
	gol = Golomb();

	if(mode == 'e') {
		image = imread(fname, IMREAD_COLOR);
		if(image.empty()) {
			throw invalid_argument("Could not open image!");
		
		}
	} else if (mode == 'd') {
		ori = BitStream(fname, 'r');
	}
}

void LosslessImageCodec::encode(const char *fname) {
	/* Open bitstream for destination file */
	des = BitStream(fname, 'w');

	/* Split image into RGB channels */
	Mat channels[3];
	split(image, channels);
	Mat r = channels[2],
		g = channels[1],
		b = channels[0];

	/* Get prediction residuals */
	vector<int> r_res = residuals(r),
				g_res = residuals(g),
				b_res = residuals(b);
	
	/* TODO: Get best value of m */
	int m = calculateM(r_res, g_res, b_res);

	gol.setM(m);

	/* Store m */
	writeVal(m, false);
	/* Store number of rows */
	writeVal(image.rows, false);
	/* Store number of columns */
	writeVal(image.cols, false);
	/* Store R residuals */
	for(auto val : r_res) writeVal(val);
	/* Store G residuals */
	for(auto val : g_res) writeVal(val);
	/* Store B residuals */
	for(auto val: b_res) writeVal(val);

	/* Close bitstream */
	des.close();
}

void LosslessImageCodec::decode(const char *fname) {
	/* Get m */
	int m = readVal(32, false);
	gol.setM(m);
	/* Get number of rows */
	int rows = readVal(32, false);
	/* Get number of columns */
	int columns = readVal(32, false);
	
	/* Get prediction residuals */
	vector<int> r_res, g_res, b_res;
	for(int i = 0; i < rows * columns; i++) { r_res.push_back(readVal()); }
	for(int i = 0; i < rows * columns; i++) { g_res.push_back(readVal()); }
	for(int i = 0; i < rows * columns; i++) { b_res.push_back(readVal()); }

	/* Close origin bitstream */
	ori.close();

	/* Mats for each color channel */
	Mat r = restore(r_res, rows, columns),
		g = restore(g_res, rows, columns),
		b = restore(b_res, rows, columns);

	/* Put together the 3 mats */
	image = Mat{rows,columns,CV_8UC3,Scalar(0,0,0)};
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < columns; j++) {
			image.at<Vec3b>(i,j) = Vec3b(b.at<uchar>(i,j), g.at<uchar>(i,j), r.at<uchar>(i,j));
		}
	}

	/* Write mat to file */
	imwrite(fname, image);
}

Mat LosslessImageCodec::restore(vector<int> res, int rows, int columns) {
	Mat channel = Mat{rows,columns,CV_8UC1, Scalar(0)};
	int a,b,c,rn,xcn;
	int counter = 0;

	/* Write the values of the top row */
	for(int j = 0; j < columns; j++) {
		channel.at<uchar>(0,j) = (uchar)res.at(j);
		counter++;
	}

	/* Calculate the inverse operation of the residual calculations */
	for(int i = 1; i < rows; i++) {
		for(int j = 0; j < columns; j++) {
			if(j == 0) {
				channel.at<uchar>(i,j) = (uchar)res.at(counter++);
			} else {
				a = (int)channel.at<uchar>(i,j-1);
				b = (int)channel.at<uchar>(i-1,j);
				c = (int)channel.at<uchar>(i-1,j-1);

				rn = (int)res.at(counter++);
				xcn = predictorJLS(a,b,c);
				channel.at<uchar>(i,j) = (uchar)(rn + xcn);
			}
		}
	}

	return channel;
}

vector<int> LosslessImageCodec::residuals(Mat channel) {
	/* Stream of values after computations */
	vector<int> r;
	/* Values to feed the predictor */
	int a,b,c;

	/* Store the values of top row */
	for(int j = 0; j < channel.cols; j++) {
		r.push_back(channel.at<uchar>(0,j));
	}

	/* Calculate estimate and calculate residual to each possible color pixel */
	for(int i = 1; i < channel.rows; i++) {
		for(int j = 0; j < channel.cols; j++) {
			if(j == 0) {
				r.push_back(channel.at<uchar>(i,j));
			} else {
				a = channel.at<uchar>(i,j-1);
				b = channel.at<uchar>(i-1,j);
				c = channel.at<uchar>(i-1,j-1);
				
				r.push_back(channel.at<uchar>(i,j) - predictorJLS(a,b,c));
			}
		}
	}

	return r;
}

int LosslessImageCodec::predictorJLS(int a, int b, int c) {
	int max = std::max(a,b),
	min = std::min(a,b);
		
	/* Predictor arithmetic */
	if(c >= max) { return min; }
	if(c <= min) { return max; }
	return a + b - c;
}

void LosslessImageCodec::writeVal(int val, bool encode /*= true*/) {
	string enc;
	if(encode) {
		/* Get golomb encoded value */
		enc = gol.encode(val);
	} else {
		/* Convert value to binary */
		enc = gol.uns2bin(val);
		/* set size as 32 bits */
		while(enc.length() < 32) {
			enc = '0' + enc;
		}
	}

	/* Write to bitstream */
	des.writeNBits(enc);
}

int LosslessImageCodec::readVal(int size /*= 0*/, bool encoded /*= true*/) {
	/* If value was not encoded with golomb coding */	
	if(!encoded) {
		string bts = ori.readNBits(size);
		return gol.bin2uns(bts);
	}
	
	/* Get value of q */
	string bts = "";
	char tmp = 0x01;	
	while(tmp != 0x0) {
		tmp = ori.readBit();
		bts += (tmp + '0');
	}

	/* Check if m is a power of 2 (i.e. all values are enconded with the same amount of bits) */
	int m = gol.getM();
	int b = gol.getB();
	if(m > 0 && (m & (m-1)) == 0) {
		/* Read b bits of info, corresponding to r */
		bts += ori.readNBits(b);
		return gol.decode(bts);
	/* if not a power of 2 */
	} else {
		/* Read b-1 bits */
		string aux = ori.readNBits(b-1);
		/* Convert the b-1 bits to int */
		int r = gol.bin2uns(aux);
		/* Check if is rest is less than 2^b - m */
		if(r < pow(2, b) - m) {
			bts += aux;
			return gol.decode(bts);
		}
		/* Read another bit if r >= 2^b - m */
		tmp = ori.readBit() + '0';
		bts = bts + aux + tmp;
		return gol.decode(bts);
	}
}

int LosslessImageCodec::calculateM(vector<int> r_res, vector<int> g_res, vector<int> b_res){
	double avg = 0;
	for(int i = 0; i < (int) r_res.size(); i++){
		if(r_res[i] < 0) avg += 2*(-r_res[i]) - 1;
		else avg += 2*r_res[i];
	}
	for(int i = 0; i < (int) g_res.size(); i++){
		if(g_res[i] < 0) avg += 2*(-g_res[i]) - 1;
		else avg += 2*g_res[i];
	}
	for(int i = 0; i < (int) b_res.size(); i++){
		if(b_res[i] < 0) avg += 2*(-b_res[i]) - 1;
		else avg += 2*b_res[i];
	}

	int totalSize = (int) r_res.size() + g_res.size() + b_res.size();
	avg = avg/totalSize;

    int m = (int) ceil(-1/(log2(avg/(avg+1))));
	return m;
}
