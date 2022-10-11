#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <math.h>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading/writing frames

int main(int argc, char *argv[]) {

	if(argc < 3) {
		cerr << "Usage: wav_quant wavFileIn quant_option\nQuantization option:\n0 - Mid-riser\n1 - Mid-tread\n";
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

    int quant_option = stoi(argv[argc-1]);
	if(quant_option < 0 || quant_option > 1) {
		cerr << "Error: invalid quantization option\n";
		return 1;
    }

	SndfileHandle sfhOut { "quant.wav", SFM_WRITE, sfhIn.format(),
	  sfhIn.channels(), sfhIn.samplerate() };
	if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
    }

	int Amax = 1, Amin = -1;
    int numlvls = 8;
    double delta = (Amax-Amin)/pow(numlvls, 2);

	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
	while((nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE))){
		samples.resize(nFrames * sfhIn.channels());
		vector<short> newsamples;
        for(short s : samples){
			printf("\ns = %hu", s);
			if (quant_option == 0){
				s = delta * (floor((s/delta)) + 0.5);
			}
			else{
				s = delta * floor((s/delta) + 0.5);
			}
			printf("\nnew value = %hu", s);
			newsamples.push_back(s);
        }
		sfhOut.writef(newsamples.data(), nFrames);
	}
	return 0;
}

