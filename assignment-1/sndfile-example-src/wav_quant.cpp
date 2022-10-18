#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <math.h>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading/writing frames

int main(int argc, char *argv[]) {

	if(argc < 3) {
		cerr << "Usage: wav_quant <input file> <output file> <number of bits to reduce>\n";
		return 1;
	}
	SndfileHandle sfhIn { argv[argc-3] };
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

	SndfileHandle sfhOut {argv[argc-2], SFM_WRITE, sfhIn.format(),
	  sfhIn.channels(), sfhIn.samplerate() };
	if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
	}

	if(!isdigit(*argv[argc-1])) {
		cerr << "Error: number of bits must be an integer\n";
		return 1;
	}

    unsigned short reduceBits = stoi(argv[argc-1]);
	if(reduceBits > 16) {
		cerr << "Error: invalid number of bits\n";
		return 1;
    }
	
	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
	while((nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE))){
		samples.resize(nFrames * sfhIn.channels());
		
		vector<short> nsamples;
        for(short s : samples)
			// reduce number of bits used to represent each audio sample
			// (ignore the num(reduceBits) least significant bits)
			nsamples.push_back((s >> reduceBits) << reduceBits);
		
		sfhOut.writef(nsamples.data(), nFrames);
	}

	return 0;
}

