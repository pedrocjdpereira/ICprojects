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
		WAVEffects(const SndfileHandle& sfh) {
			format = sfh.format();
			channels = sfh.channels();
			samplerate = sfh.samplerate();
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
			for(size_t i = 0; i < delay*samplerate*2 && i < samples.size(); i++)
				nsamples.push_back(samples[i]);
			
			for(size_t i = delay*samplerate*2; i < samples.size(); i++)
				nsamples.push_back(samples[i] + atenuation*samples[i-delay*samplerate*2]);

			for(size_t i = samples.size()-delay*samplerate*2; i < samples.size(); i++) {
				nsamples.push_back(atenuation*samples[i]);
			}

			samples = nsamples;
		}

		void multiple_echos(float delay, float atenuation) {
			std::vector<short> nsamples {};
			for(size_t i = 0; i < delay*samplerate*2 && i < samples.size(); i++)
				nsamples.push_back(samples[i]);
			
			for(size_t i = delay*samplerate*2; i < samples.size(); i++)
				nsamples.push_back(samples[i] + atenuation*nsamples[i-delay*samplerate*2]);

			// TODO: delay till hit 0 frequency
			for(size_t i = samples.size()-delay*samplerate*2; i < samples.size(); i++) {
				nsamples.push_back(atenuation*nsamples[i]);
			}

			samples = nsamples;

		}

		void amplitude_modulation() {
			/*
			std::ofstream leftsignal, rightsignal;
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
			*/
		}
};

#endif
