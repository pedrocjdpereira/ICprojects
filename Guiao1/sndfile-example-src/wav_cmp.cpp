#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <math.h>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading/writing frames

int main(int argc, char *argv[]) {

	if(argc < 3) {
		cerr << "Usage: wav_quant OGwavFile NewwavFile2\n";
		return 1;
	}

	SndfileHandle sfhOG { argv[argc-2] };
	if(sfhOG.error()) {
		cerr << "Error: invalid input original file\n";
		return 1;
    }

	if((sfhOG.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: original file is not in WAV format\n";
		return 1;
	}

	if((sfhOG.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: original file is not in PCM_16 format\n";
		return 1;
	}

    SndfileHandle sfhNew { argv[argc-1] };
	if(sfhNew.error()) {
		cerr << "Error: invalid input new file\n";
		return 1;
    }

	if((sfhNew.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: new file is not in WAV format\n";
		return 1;
	}

	if((sfhNew.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error:new file is not in PCM_16 format\n";
		return 1;
	}

	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sfhOG.channels());
	while((nFrames = sfhOG.readf(samples.data(), FRAMES_BUFFER_SIZE))){
		samples.resize(nFrames * sfhOG.channels());
        for(auto s : samples){
        }
	}

	return 0;
}
