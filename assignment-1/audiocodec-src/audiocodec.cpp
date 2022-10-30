#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <fftw3.h>
#include "audiocodec.h"

using namespace std;

int main(int argc, char *argv[]) {

	SndfileHandle sfhIn { argv[argc-1] };
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

	size_t channels { static_cast<size_t>(sfhIn.channels()) };
	size_t frames { static_cast<size_t>(sfhIn.frames()) };
	size_t samplerate {static_cast<size_t>(sfhIn.samplerate()) };
	size_t format {static_cast<size_t>(sfhIn.format()) };

	// Read all samples: c1 c2 ... cn c1 c2 ... cn ...
	// Note: A frame is a group c1 c2 ... cn
	vector<short> samples(channels * frames);
	sfhIn.readf(samples.data(), frames);

	size_t bs {1024};
	double dctFrac {0.2};
	
	AudioCodec ac {samples, frames, samplerate, format, channels};
	
	ac.encode("out.ac", bs, dctFrac);
	ac.decode("out.ac");
}
