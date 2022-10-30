#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <fftw3.h>
#include "audiocodec.h"

using namespace std;

int main(int argc, char *argv[]) {

	if(argc < 4) {
		cerr << "Usage: audiocodec <option> <input file> <output file> \n";
		cerr << "\nOptions:\n";
		cerr << "\t-e [blocksize] [dctFrac]\t- encode file\n";
		cerr << "\t-d                      \t- decode file\n";
		return 1;
	}
	
	AudioCodec ac {};
	
	if(string(argv[1]) == "-e") {
		size_t bs;
		double dctFrac;
		if(argc >= 6) {
			bs = stoi(argv[2]);
			dctFrac = stod(argv[3]);
		} else {
			bs = 1024;
			dctFrac = 0.2;
		}

		SndfileHandle sfhIn { argv[argc-2] };
		if(sfhIn.error()) {
			cerr << "Error: invalid input file\n";
			return 1;
		}

		if((sfhIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
			cerr << "Error: file is not in WAV format\n";
			return 1;
		}

		if((sfhIn.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
			cerr << "Error: file is not in PCM_16 format\n";
			return 1;
		}

		ac.encode(sfhIn, argv[argc-1], bs, dctFrac);

		return 0;

	} else if (string(argv[1]) == "-d") {
		ac.decode(argv[argc-2], argv[argc-1]);
		return 0;
	} else {
		cerr << "Invalid Option!";
	}

	return 1;
}
