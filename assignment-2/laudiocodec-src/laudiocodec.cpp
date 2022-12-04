#include <iostream>
#include <stdio.h>
#include <sndfile.h>
#include <math.h>
#include <vector>
#include "bitstream.h"
#include "laudiocodec.hpp"
#include "../golomb/golomb.cpp"

using namespace std;

void intToBin(char* binvalue, int value, int nbits){
	for (int i = nbits-1; i > -1; i--){
		if(value == 0){
			binvalue[i] = '0';
			nbits--;
			continue;
		}
		binvalue[i] = (value % 2 == 0 ? '0' : '1');
		value = value/2;
		nbits--;
	}
}

int binToInt(char* binvalue, int nbits){
	int val = 0;
	for(int i = 0; i < nbits; i++){
		if((binvalue[i] & 0x01) != 0){
			val = val + pow(2 , (nbits-1-i) );
		}
	}
	return val;
}

void stringtochar(string s, char* c){
	for(int i = 0; i < (int) s.length(); i++){
		c[i] = s[i];
	}
}

int main(int argc, char *argv[]) {

	if(argc < 4) {
		cerr << "Usage: laudiocodec <option> <original file> <new file>\n";
		cerr << "Options:\n";
		cerr << "c - compress\n";
		cerr << "d - decompress\n";
		exit(1);
	}
	char* option = argv[argc-3];
	const char* InFileName = argv[argc-2];
	const char* OutFileName = argv[argc-1];


	AudioCodec ac = AudioCodec(InFileName, OutFileName, *option);

	if(*option == 'c'){
		ac.compress();
	}
	else{
		ac.decompress();
	}
};

AudioCodec::AudioCodec(){}

AudioCodec::AudioCodec(const char* infname, const char* outfname, char opt){
	mode = opt;

	if(mode == 'd'){
		file = BitStream(infname, 'r');
		fileOutName = outfname;
	}
	else if(mode == 'c'){
		cout << "Which do you wish to perform?\n";
		cout << "Lossless compression (0)\n";
		cout << "Lossy compression (1)\n";
		cout << "Enter your option:";
		cin >> lossy;
		SNDFILE* fileIn; 
		short s[2];

		fileIn = sf_open(infname, SFM_READ, &info);

		if(fileIn == NULL){
			cout << "ERROR OPENING FILE";
			exit(1);
		}

		int framesRead;
		do{
			framesRead = sf_readf_short(fileIn, s, 1);
			if(framesRead > 0){
				xn.push_back(s[0]);
				xn.push_back(s[1]);
			}
		}
		while(framesRead > 0);

		sf_close(fileIn);

		file = BitStream(outfname, 'w');
	}
}

void AudioCodec::compress() {
	if(mode != 'c'){
		cout << "INVALID OPERATION MODE";
		exit(1);
	}

	predictorLossless();

	if(lossy == 1){
		cout << "Introduce the amount of bits to be eliminated: ";
		int reduceBits;
		cin >> reduceBits;
		for(int i = 0; i < (int) rn.size(); i++){
			rn[i] = ((rn[i] >> reduceBits));
		}
	}
	
	double avg = 0;
    for(int i = 0; i < (int) rn.size(); i++) {
        if(rn[i] < 0) avg += 2*(-rn[i]) - 1;
		else avg += 2*rn[i];
    }
    avg = avg/rn.size();

    m = (int) ceil(-1/(log2(avg/(avg+1))));

	Golomb g = Golomb(m);

	encodeMandPred(m, predictorN);
	encodeHeader(info.frames, info.samplerate, info.format, info.channels, lossy);

	//encodeShamt

	for(int i = 0; i < (int) rn.size(); i++){
		string s = g.encode(rn[i]);
		char c[s.length()];
		stringtochar(s, c);
		file.writeNBits(c, s.length());
	}
}

void AudioCodec::predictorLossless(){
	vector<short> leftxn;
	vector<short> rightxn;

	for(int i = 0; i < (int) xn.size(); i++){
		if(i % 2 == 0){
			leftxn.push_back(xn[i]);
		}
		else{
			rightxn.push_back(xn[i]);
		}
	}
	
	double avg1 = 0;
	double avg2 = 0;
	double avg3 = 0;
	vector<short> rn1;
	vector<short> rn2;
	vector<short> rn3;

	for(int i = 0; i < (int) leftxn.size(); i++) {
		if(i < 1){
			int s[2] = {leftxn[i], rightxn[i]};
			rn1.push_back(s[0]);
			rn1.push_back(s[1]);
			avg1 += s[0] + s[1];
			continue;
		}
		int s[2] = {leftxn[i] - leftxn[i-1], 
						rightxn[i] - rightxn[i-1]};
		rn1.push_back(s[0]);
		rn1.push_back(s[1]);
		avg1 += s[0] + s[1];
	}

    for(int i = 0; i < (int) leftxn.size(); i++) {
		if(i < 2){
			int s[2] = {leftxn[i], rightxn[i]};
			rn2.push_back(s[0]);
			rn2.push_back(s[1]);
			avg2 += s[0] + s[1];
			continue;
		}
		int s[2] = {leftxn[i] - 2*leftxn[i-1] + leftxn[i-2], 
						rightxn[i] - 2*rightxn[i-1] + rightxn[i-2]};
		rn2.push_back(s[0]);
		rn2.push_back(s[1]);
		avg2 += s[0] + s[1];
 	}

	for(int i = 0; i < (int) leftxn.size(); i++) {
		if(i < 3){
			int s[2] = {leftxn[i], rightxn[i]};
			rn3.push_back(s[0]);
			rn3.push_back(s[1]);
			avg3 += s[0] + s[1];
			continue;
		}
		int s[2] = {leftxn[i] - 3*leftxn[i-1] + 3*leftxn[i-2] - leftxn[i-3],
						rightxn[i] - 3*rightxn[i-1] + 3*rightxn[i-2] - rightxn[i-3]};
		rn3.push_back(s[0]);
		rn3.push_back(s[1]);
		avg3 += s[0] + s[1];
	}

	avg1 = abs(avg1/xn.size());
	avg2 = abs(avg2/xn.size());
	avg3 = abs(avg3/xn.size());

	if( (avg1 < avg2) & (avg1 < avg3) ){
		predictorN = 1;
		rn = rn1;
	}
	else if( (avg2 < avg1) & (avg2 < avg3) ){
		predictorN = 2;
		rn = rn2;
	}
	else if( (avg3 < avg1) & (avg3 < avg2) ){
		predictorN = 3;
		rn = rn3;
	}
}

void AudioCodec::decompress(){
	if(mode != 'd'){
		cout << "INVALID OPERATION MODE";
		exit(1);
	}

	decodeMandPred();

	int b = (int) ceil(log2(m));

	int header[5];
	decodeHeader(header);
	int lossy = header[0];
	int frames = header[1];
	int samplerate = header[2];
	int format = header[3];
	int channels = header[4];

	Golomb g = Golomb(m);

	for(int i = 0; i < frames*channels; i++){
		string s = getCodeword(m, b);
		rn.push_back(g.decode(s));
	}

	if(lossy == 1){
		for(int i = 0; i < (int) rn.size(); i++){
			rn[i] = ((rn[i] >> 1));
		}
	}
	
	decompPredictorLossless();

	SF_INFO fileOutInfo;
	fileOutInfo.frames = frames;
	fileOutInfo.samplerate = samplerate;
	fileOutInfo.format = format;
	fileOutInfo.channels = channels;

	SNDFILE* decompFile = sf_open(fileOutName, SFM_WRITE, &fileOutInfo);
	sf_write_raw(decompFile, &xn[0], xn.size()*2);
	sf_write_sync(decompFile);
	sf_close(decompFile);
}

void AudioCodec::decompPredictorLossless(){
	vector<short> leftrn;
	vector<short> rightrn;

	for(int i = 0; i < (int) rn.size(); i++){
		if(i % 2 == 0){
			leftrn.push_back(rn[i]);
		}
		else{
			rightrn.push_back(rn[i]);
		}
	}

	vector<short> leftxn;
	vector<short> rightxn;

	if(predictorN == 1){
		for(int i = 0; i < (int) leftrn.size(); i++) {
			if(i < 1){
				leftxn.push_back(leftrn[i]);
				rightxn.push_back(rightrn[i]);
				continue;
			}
			leftxn.push_back(leftrn[i] + leftxn[i-1]);
			rightxn.push_back(rightrn[i] + rightxn[i-1]);
		}
	}
	else if(predictorN == 2){
		for(int i = 0; i < (int) leftrn.size(); i++) {
			if(i < 2){
				leftxn.push_back(leftrn[i]);
				rightxn.push_back(rightrn[i]);
				continue;
			}
			leftxn.push_back(leftrn[i] + 2*leftxn[i-1] - leftxn[i-2]);
			rightxn.push_back(rightrn[i] + 2*rightxn[i-1] - rightxn[i-2]);
		}
	}
	else{
		for(int i = 0; i < (int) leftrn.size(); i++) {
			if(i < 3){
				leftxn.push_back(leftrn[i]);
				rightxn.push_back(rightrn[i]);
				continue;
			}
			leftxn.push_back(leftrn[i] + 3*leftxn[i-1] - 3*leftxn[i-2] + leftxn[i-3]);
			rightxn.push_back(rightrn[i] + 3*rightxn[i-1] - 3*rightxn[i-2] + leftxn[i-3]);
		}
	}

	for(int i = 0; i < (int) leftxn.size(); i++){
		xn.push_back(leftxn[i]);
		xn.push_back(rightxn[i]);
	}
}

string AudioCodec::getCodeword(int m, int b){
	string q = "";
	char c;
	while(true){
		c = file.readBit();
		if(c == 0x01){
			q += '1';
		}
		else{
			q += '0';
			break;
		}
	}

	string r = "";
	char binRem[b-1];
	file.readNBits(binRem, b-1);

	for(int i = 0; i < b-1; i++){
		if(binRem[i] == 0x01){
			r += '1';
		}
		else{
			r += '0';
		}	
	}

	int rvalue = binToInt(binRem, b-1);

	if(rvalue >= pow(2,b)-m){
		char c = file.readBit();
		if(c == 0x01){
			r += '1';
		}
		else{
			r += '0';
		}
	}

	return q + r;
}

void AudioCodec::encodeMandPred(int m, int predictorN){
	char binM[32];
	intToBin(binM, m, 32);
	file.writeNBits(binM, 32);
	char binPred[2];
	intToBin(binPred, predictorN, 2);
	file.writeNBits(binPred, 2);
}

void AudioCodec::decodeMandPred(){
	char binM[32];
	file.readNBits(binM, 32);
	m = binToInt(binM, 32);
	char binPred[2];
	file.readNBits(binPred, 2);
	predictorN = binToInt(binPred, 2);
}

void AudioCodec::encodeHeader(int frames, int samplerate, int format, int channels, int lossy){
	if(lossy == 0){
		file.writeBit('0');
	}
	else{
		file.writeBit('1');
	}
	
	char framesBin[32];
	char samplerateBin[32];
	char formatBin[32];
	char channelsBin[4];

	intToBin(framesBin, frames, 32);
	intToBin(samplerateBin, samplerate, 32);
	intToBin(formatBin, format, 32);
	intToBin(channelsBin, channels, 4);

	file.writeNBits(framesBin, 32);
	file.writeNBits(samplerateBin, 32);
	file.writeNBits(formatBin, 32);
	file.writeNBits(channelsBin, 4);
}

void AudioCodec::decodeHeader(int header[]){
	char lossy = file.readBit();
	header[0] = int(lossy&0x01);

	char framesBin[32];
	char samplerateBin[32];
	char channelsBin[4];
	char formatBin[32];

	file.readNBits(framesBin, 32);
	file.readNBits(samplerateBin, 32);
	file.readNBits(formatBin, 32);
	file.readNBits(channelsBin, 4);

	header[1] = binToInt(framesBin, 32);
	header[2] = binToInt(samplerateBin, 32);
	header[3] = binToInt(formatBin, 32);
	header[4] = binToInt(channelsBin, 4);
}
