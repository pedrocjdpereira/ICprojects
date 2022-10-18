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
		cerr << "Usage: wav_cmp <original file> <derived file>\n";
		return 1;
	}

	SndfileHandle sfhOriginal{argv[argc - 2]};
	if (sfhOriginal.error())
	{
		cerr << "Error: invalid original file\n";
		return 1;
	}

	if ((sfhOriginal.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV)
	{
		cerr << "Error: original file is not in WAV format\n";
		return 1;
	}

	if ((sfhOriginal.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16)
	{
		cerr << "Error: original file is not in PCM_16 format\n";
		return 1;
	}

	SndfileHandle sfhDerived{argv[argc - 1]};
	if (sfhDerived.error())
	{
		cerr << "Error: invalid derived file\n";
		return 1;
	}

	if ((sfhDerived.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV)
	{
		cerr << "Error: derived file is not in WAV format\n";
		return 1;
	}

	if ((sfhDerived.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16)
	{
		cerr << "Error: derived file is not in PCM_16 format\n";
		return 1;
	}

	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sfhOriginal.channels());	// samples of original file
	vector<short> samples2(FRAMES_BUFFER_SIZE * sfhDerived.channels()); // samples of new File

	double error = 0;
	double maxError=0,tmp;
	double SNR;
	// double D=0,S=0;
	// double N = 0;

	while ((nFrames = sfhOriginal.readf(samples.data(), FRAMES_BUFFER_SIZE)))
	{
		sfhDerived.readf(samples2.data(),FRAMES_BUFFER_SIZE);
		// N = (double)sfhOG.channels()*nFrames;

		samples2.resize(nFrames*sfhOriginal.channels());
		samples.resize(nFrames * sfhOriginal.channels());
		error = error + pow(((samples[0]+samples[1])/2)-((samples2[0]+samples2[1])/2),2);
		//D += pow(((samples[0]+samples[1])/2)-((samples2[0]+samples2[1])/2),2);
		//S += pow((samples[0]+samples[1]/2), 2);
		tmp = abs(((samples[0]+samples[1])/2)-((samples2[0]+samples2[1])/2));
		if (tmp>maxError)
		{
			maxError = tmp;
		}
		
		// for (auto s : samples)
		// {
		// }
		//samples.size();
		


	}
	// D = (1/N) * D;
    // S = (1/N) * S;

	SNR = 10*log((pow(FRAMES_BUFFER_SIZE,2)/error))/log(10);
	// double SNR1 = 10*log10(S/D);
	// printf("N %f\n",N);
	cout << "SNR " << SNR << " db\n";
	// printf("SNR1111 %f db\n",SNR1);
	cout << "Max error " << maxError << "\n";
	return 0;
}
