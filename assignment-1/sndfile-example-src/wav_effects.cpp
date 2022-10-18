#include <iostream>
#include <vector>
#include <sndfile.hh>
#include "wav_effects.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading/writing frames

int main(int argc, char *argv[]) {
	if(argc < 5) {
		cerr << "Usage: wav_effects <options> <input file> <output file>\n";
		cerr << "\nOptions:\n";
		cerr << "\t-se <delay> <atenuation>\t\t= Single echo.\n";
		cerr << "\t-me <some value>\t\t= Multiple echos.\n";
		cerr << "\t-am <some value>\t\t= Amplitude modulation\n";
		return 1;
	}

	// declare variables representing options
	unsigned short se=0, me=0, am=0;
	for(short n=1; n < argc-2; n++) {
		if(string(argv[n]) == "-se") se=n;
		else if(string(argv[n]) == "-me") me=n;
		else if(string(argv[n]) == "-am") am=n;
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

	// Read samples from file and store them in WAVEffects object
	size_t nFrames;
	int n {};
	vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
	WAVEffects eff { sfhIn, argv[argc-1] };
	while((nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sfhIn.channels());
		eff.append(samples);
		n += nFrames;
	}

	//if(se) eff.single_echo(stof(argv[se+1]), stof(argv[se+2]));

	if(me) eff.multiple_echos(stof(argv[me+1]), stof(argv[me+2]));

	eff.dump();

	return 0;
}
