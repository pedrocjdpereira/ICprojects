#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <math.h>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65535; // Buffer for reading/writing frames

int main(int argc, char *argv[]) {

	if(argc < 3) {
		cerr << "Usage: wav_quant wavFileIn noOfLostBits\n";
		return 1;
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

    int lost_bits = stoi(argv[argc-1]);
	if(lost_bits < 0 || lost_bits > 16) {
		cerr << "Error: invalid number of bits lost\n";
		return 1;
    }

	SndfileHandle sfhOut { "quant.wav", SFM_WRITE, sfhIn.format(),
	  sfhIn.channels(), sfhIn.samplerate() };
	
	if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
    }

	int NoBitsIn = 16;
	int NoBitsOut = NoBitsIn-lost_bits;
	int y = 0;
	for(int i = NoBitsIn-1; i >= NoBitsOut; i--){
		y = y + pow(2, i);
	}
	y = pow(2, NoBitsIn) - y - 1;
	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
	while((nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE))){
		samples.resize(nFrames * sfhIn.channels());
		vector<short> newsamples;
        for(short s : samples){
			s = y & (s >> lost_bits);
			newsamples.push_back(s);
        }
		sfhOut.writef(newsamples.data(), nFrames);
	}
	return 0;
}

