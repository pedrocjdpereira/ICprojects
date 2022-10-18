#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <math.h>
#include <fstream>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading/writing frames

int main(int argc, char *argv[]) {

	if(argc < 2) {
		cerr << "Usage: wav_quant wavFileIn\n";
		return 1;
	}
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

	ofstream leftsignal, rightsignal;
  	leftsignal.open ("leftsignal.txt");
	rightsignal.open ("rightsignal.txt");

    int i = 0, lx = 0, rx = 0;
	double ly, ry;
	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
	while((nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE))){
		samples.resize(nFrames * sfhIn.channels());
        double news = 0;
        for(short s : samples){
			news = (((double)s)*8/65536) + 6;
			if(i % 2 == 0){
				ly = news*sin(pow(10,6)*i);
				leftsignal << "x = " << lx << " y = " << ly << "\n";
				lx++;
			}
			else{
				ry = news*sin(pow(10,6)*i);
				rightsignal << "x = " << rx << " y = " << ry << "\n";
				rx++;
			}
            i++;
        }
	}
  	leftsignal.close();
	rightsignal.close();
	return 0;
}

