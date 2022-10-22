#ifndef WAVEFFECTS_H
#define WAVEFFECTS_H

#include <iostream>
#include <vector>
#include <sndfile.hh>

class WAVEffects {
	private:
		std::vector<short> samples {};
		std::string outFilePath;
		int format, channels, samplerate;
	
	public:
		WAVEffects(const SndfileHandle& sfh, std::string ofp) {
			format = sfh.format();
			channels = sfh.channels();
			samplerate = sfh.samplerate();
			outFilePath = ofp;
		}

		void append(const std::vector<short>& newSamples) {
			// append s values to end of samples
			for (auto s: newSamples) samples.push_back(s);
		}

		void dump(SndfileHandle& sfh) const {
			sfh.writef(samples.data(), samples.size()/2);
		}	

		void single_echo(float delay, float atenuation) {
			std::vector<short> nsamples {};
			for(int i = 0; i < delay*samplerate*2 && i < samples.size(); i++)
				nsamples.push_back(samples[i]);
			
			for(int i = delay*samplerate*2; i < samples.size(); i++)
				nsamples.push_back(samples[i] + atenuation*samples[i-delay*samplerate*2]);

			for(int i = samples.size()-delay*samplerate*2; i < samples.size(); i++) {
				nsamples.push_back(atenuation*samples[i]);
			}

			samples = nsamples;
		}

		void multiple_echos(float delay, float atenuation) {
			std::vector<short> nsamples {};
			for(int i = 0; i < delay*samplerate*2 && i < samples.size(); i++)
				nsamples.push_back(samples[i]);
			
			for(int i = delay*samplerate*2; i < samples.size(); i++)
				nsamples.push_back(samples[i] + atenuation*nsamples[i-delay*samplerate*2]);

			// TODO: delay till hit 0 frequency
			for(int i = samples.size()-delay*samplerate*2; i < samples.size(); i++) {
				nsamples.push_back(atenuation*nsamples[i]);
			}

			samples = nsamples;

		}
};

#endif
