#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <math.h>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading/writing frames

int main(int argc, char *argv[])
{

	if (argc < 3)
	{
		cerr << "Usage: wav_quant OGwavFile NewwavFile2\n";
		return 1;
	}

	SndfileHandle sfhOG{argv[argc - 2]}; // original file
	if (sfhOG.error())
	{
		cerr << "Error: invalid input original file\n";
		return 1;
	}

	if ((sfhOG.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV)
	{
		cerr << "Error: original file is not in WAV format\n";
		return 1;
	}

	if ((sfhOG.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16)
	{
		cerr << "Error: original file is not in PCM_16 format\n";
		return 1;
	}

	SndfileHandle sfhNew{argv[argc - 1]}; // new file
	if (sfhNew.error())
	{
		cerr << "Error: invalid input new file\n";
		return 1;
	}

	if ((sfhNew.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV)
	{
		cerr << "Error: new file is not in WAV format\n";
		return 1;
	}

	if ((sfhNew.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16)
	{
		cerr << "Error:new file is not in PCM_16 format\n";
		return 1;
	}

	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sfhOG.channels());	// samples of original file
	vector<short> samples2(FRAMES_BUFFER_SIZE * sfhNew.channels()); // samples of new File

	double error = 0;
	double SP=0;
	double maxError=0,tmp;
	double SNR;

	while ((nFrames = sfhOG.readf(samples.data(), 1)))
	{
		sfhNew.readf(samples2.data(),1);
		
		samples2.resize(nFrames*sfhOG.channels());
		samples.resize(nFrames * sfhOG.channels());
		error = error + pow(((samples[0]+samples[1])/2)-((samples2[0]+samples2[1])/2),2);
		SP += pow((samples[0]+samples[1])/2,2);
		
		tmp = abs(((samples[0]+samples[1])/2)-((samples2[0]+samples2[1])/2));
		if (tmp>maxError)
		{
			maxError = tmp;
		}
		
	}
	
	SNR = 10*log10((SP/error));
	printf("SNR %f db\n",SNR);
	printf("Max error %f\n",maxError);
	return 0;
}