#ifndef AUDIOCODEC_H
#define AUDIOCODEC_H

#include <stdio.h>
#include <sndfile.h>
#include <vector>
#include "bitstream.h"

using namespace std;

class AudioCodec{
	private:
		BitStream file;
		const char* fileOutName;
		SF_INFO info;
		char mode;
		int m;
        int predictorN;
		int lossy;
        vector<short> xn;
		vector<short> rn;

	public:
		AudioCodec();

		AudioCodec(const char* infname, const char* outfname, char mode);

		void compress();

		void predictorLossless();

		void decompress();
	
		void decompPredictorLossless();

		string getCodeword(int m, int b);
		
		void encodeMandPred(int m, int Pred);

		void decodeMandPred();

		void encodeHeader(int frames, int samplerate, int format, int channels, int lossy);

		void decodeHeader(int header[]);
};


#endif
