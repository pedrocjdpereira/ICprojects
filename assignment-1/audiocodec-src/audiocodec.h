#ifndef AUDIOCODEC_H
#define AUDIOCODEC_H

#include "../bitstream-src/bitstream.h"
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cmath>
#include <bitset>
#include <fftw3.h>
#include <numeric>
#include <sndfile.hh>

using namespace std;

class AudioCodec {
	private:
		size_t frames;
		size_t samplerate;
		size_t format;
		size_t channels;
		size_t blocksize;
		double dctFrac;

	public:
		AudioCodec() { }
		
		void encode(SndfileHandle sfh, const char *ofname, size_t bs, double dctf) {
			channels = static_cast<size_t>(sfh.channels());
			frames = static_cast<size_t>(sfh.frames());
			samplerate = static_cast<size_t>(sfh.samplerate());
			format = static_cast<size_t>(sfh.format());
			
			vector<short> samples(channels * frames);
			sfh.readf(samples.data(), frames);
			blocksize = bs;
			dctFrac = dctf;
			
			BitStream obs {ofname, 'w'};
			
			// Write header to file
			string aux = encodeHeader();
			char header[164];
			strcpy(header, aux.c_str());
			obs.writeNBits(header, 164);

			size_t blocks { static_cast<size_t>(ceil(static_cast<double>(frames) / blocksize)) };

			// Do zero padding, if necessary
			samples.resize(blocks * blocksize * channels);

			// Vector for holding all DCT coefficients, channel by channel
			vector<vector<double>> x_dct(channels, vector<double>(blocks * blocksize));

			// Vector for holding DCT computations
			vector<double> x(blocksize);

			// Direct DCTn
			fftw_plan plan_d = fftw_plan_r2r_1d(blocksize, x.data(), x.data(), FFTW_REDFT10, FFTW_ESTIMATE);
			for(size_t n = 0 ; n < blocks ; n++)
				for(size_t c = 0 ; c < channels ; c++) {
					for(size_t k = 0 ; k < blocksize ; k++)
						x[k] = samples[(n * blocksize + k) * channels + c];

					fftw_execute(plan_d);
					// Keep only "dctFrac" of the "low frequency" coefficients
					for(size_t k = 0 ; k < blocksize * dctFrac ; k++) {
						short value = static_cast<short>(round(x[k] / (blocksize << 1)));
						string s = bitset<16>(value).to_string();
						for(int i = 0; i < 16; i++) obs.writeBit(s[i]);
						x_dct[c][n * blocksize + k] = x[k] / (blocksize << 1);
					}
				}
	
			obs.close();
		}

		void decode(const char *ifname, const char *ofname) {
			BitStream obs {ifname, 'r'};
			char header[164];
			obs.readNBits(header, 164);
			decodeHeader(header);

			SndfileHandle sfhOut { ofname, SFM_WRITE, static_cast<int>(format),\
			  static_cast<int>(channels), static_cast<int>(samplerate) };
			if(sfhOut.error())
				throw runtime_error("Invalid input file");

			vector<short> samples(channels * frames);

			size_t blocks { static_cast<size_t>(ceil(static_cast<double>(frames) / blocksize)) };

			// Do zero padding, if necessary
			samples.resize(blocks * blocksize * channels);

			// Vector for holding all DCT coefficients, channel by channel
			vector<vector<double>> x_dct(channels, vector<double>(blocks * blocksize));

			// Vector for holding DCT computations
			vector<double> x(blocksize);
			
			char w[16];
			// Get coefficients from file
			for(size_t n = 0 ; n < blocks ; n++)
				for(size_t c = 0 ; c < channels ; c++)
					for(size_t k = 0 ; k < blocksize * dctFrac ; k++) {
						obs.readNBits(w, 16);
						unsigned short num = 0;
						for(int i = 0; i < 16; i++) {
							if(w[i] == 0x01)
								num += pow(2,15-i);
						}

						cout << (short)num << '\n';
						x_dct[c][n * blocksize + k] = (short)num;
					}

			fftw_plan plan_i = fftw_plan_r2r_1d(blocksize, x.data(), x.data(), FFTW_REDFT01, FFTW_ESTIMATE);
			for(size_t n = 0 ; n < blocks ; n++)
				for(size_t c = 0 ; c < channels ; c++) {
					for(size_t k = 0 ; k < blocksize ; k++)
						x[k] = x_dct[c][n * blocksize + k];

					fftw_execute(plan_i);
					for(size_t k = 0 ; k < blocksize ; k++)
						samples[(n * blocksize + k) * channels + c] = static_cast<short>(round(x[k]));

				}

			sfhOut.writef(samples.data(), frames);
		}

		string encodeHeader() {
			string res = convertToBin(frames, 32);
			res += convertToBin(samplerate, 32);
			res += convertToBin(format, 32);
			res += convertToBin(channels, 4);
			res += convertToBin(blocksize, 32);
			res += convertToBin((int)dctFrac*100, 32);
			return res;
		}

		void decodeHeader(char *header) {
			frames = 0;
			for(int i = 0; i < 32; i++)
				frames += static_cast<int>(header[i]) * pow(2, i);
			
			samplerate = 0;
			for(int i = 32; i < 64; i++)
				samplerate += static_cast<int>(header[i]) * pow(2, i-32);
			
			format = 0;
			for(int i = 64; i < 96; i++)
				format += static_cast<int>(header[i]) * pow(2, i-64);
			
			channels = 0;
			for(int i = 96; i < 100; i++)
				channels += static_cast<int>(header[i]) * pow(2, i-96);
			
			blocksize = 0;
			for(int i = 100; i < 132; i++)
				blocksize += static_cast<int>(header[i]);

			dctFrac = 0;
			for(int i = 100; i < 164; i++)
				dctFrac += static_cast<int>(header[i]);
			dctFrac /= 100;
		}

		string convertToBin(int value, int numBits) {
			string res = "";

			while(value) {
				res += (value % 2 == 0) ? '0' : '1';
				value /= 2;
				numBits--;
			}

			while(numBits--)
				res+='0';

			return res;
		}
};

#endif
