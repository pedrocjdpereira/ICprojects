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
		cerr << "Usage: img_cpy <option> <original file> <new file>\n";
		cerr << "Options\n";
		cerr << "c - compress\n";
		cerr << "d - decompress\n";

		exit(1);
	}
	char* mode = argv[argc-3];
	const char* InFileName = argv[argc-2];
	const char* OutFileName = argv[argc-1];


	AudioCodec ac = AudioCodec(InFileName, OutFileName, *mode);

	if(*mode == 'c'){
		ac.compress();
	}
	else{
		ac.decompress();
	}
};

AudioCodec::AudioCodec(){}

AudioCodec::AudioCodec(const char* infname, const char* outfname, char mod){
	mode = mod;
	
	if(mode == 'd'){
		predictorN = 2;
		file = BitStream(infname, 'r');
		fileOutName = outfname;
	}
	else if(mode == 'c'){
		predictorN = 2;

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
			xn.push_back(s[0]);
			xn.push_back(s[1]);
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
	
	double avg = 0;
    for(int i = 0; i < (int) rn.size(); i++) {
        if(rn[i] < 0) avg += 2*(-rn[i]) - 1;
		else avg += 2*rn[i];
    }
    avg = avg/rn.size();

    m = (int) ceil(-1/(log2(avg/(avg+1))));
	//cout << m;

	Golomb g = Golomb(m);

	encodeM(m);
	encodeHeader(info.frames, info.samplerate, info.format, info.channels, 0);

	//encodeShamt

	for(int i = 0; i < (int) rn.size(); i++){
		string s = g.encoder(rn[i]);
		char c[s.length()];
		stringtochar(s, c);
		file.writeNBits(c, s.length());
		/*if(i > 300000 & i < 318131){
			cout << "i = " << i << "," << xn[i] << "," << rn[i] << "," << s << "\n";
		}*/
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
	
    for(int i = 0; i < (int) leftxn.size(); i++) {
		if(i < 2){
			rn.push_back(leftxn[i] - 0);
			rn.push_back(rightxn[i] - 0);
			continue;
		}
		rn.push_back(leftxn[i] - 2*leftxn[i-1] + leftxn[i-2]);
		rn.push_back(rightxn[i] - 2*rightxn[i-1] + rightxn[i-2]);
	}
}

void AudioCodec::decompress(){
	if(mode != 'd'){
		cout << "INVALID OPERATION MODE";
		exit(1);
	}

	m = decodeM();
	//cout << m;
	int b = (int) ceil(log2(m));
	int header[5];
	decodeHeader(header);
	int lossy = header[0];
	int frames = header[1];
	int samplerate = header[2];
	int format = header[3];
	int channels = header[4];

	Golomb g = Golomb(m);

	vector<string> sArray;

	for(int i = 0; i < frames*channels; i++){
		string s = getCodeword(m, b);
		rn.push_back(g.decoder(s));
		sArray.push_back(s);
	}
	
	decompPredictorLossless();

	/*for(int i = 0; i < frames*channels; i++){
		if(i > 318110 & i <= 318130){
			cout << "i = " << i << "," << xn[i] << "," << rn[i] << "," << sArray[i] << "\n";
		}
	}*/

	info.frames = frames;
	info.samplerate = samplerate;
	info.format = format;
	info.channels = channels;

	SNDFILE* decompFile = sf_open(fileOutName, SFM_WRITE, &info);
	sf_writef_short(decompFile, &xn[0], xn.size());
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

	for(int i = 0; i < (int) leftrn.size(); i++) {
		if(i < 2){
			leftxn.push_back(leftrn[i]);
			continue;
		}
		leftxn.push_back(leftrn[i] + 2*leftxn[i-1] - leftxn[i-2]);
		/*if(i == 159064)
			cout << "i = " << i << "," << leftrn[i] << "," << leftxn[i-1] << "," << leftxn[i-2] << ":" << leftxn[i] << "\n";
*/	}

	for(int i = 0; i < (int) rightrn.size(); i++) {
		if(i < 2){
			rightxn.push_back(rightrn[i]);
			continue;
		}
		rightxn.push_back(rightrn[i] + 2*rightxn[i-1] - rightxn[i-2]);
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

void AudioCodec::encodeM(int m){
	char binM[32];
	intToBin(binM, m, 32);
	file.writeNBits(binM, 32);
}

int AudioCodec::decodeM(){
	char binM[32];
	file.readNBits(binM, 32);
	int m = binToInt(binM, 32);
	return m;
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
