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
		vector<short> samples;
		size_t frames;
		size_t samplerate;
		size_t format;
		size_t channels;
		size_t blocksize;
		double dctFrac;

	public:
		AudioCodec(vector<short> samples, size_t frames, size_t samplerate, \
			size_t format, size_t channels) {
			AudioCodec::samples = samples;
			AudioCodec::frames = frames;
			AudioCodec::samplerate = samplerate;
			AudioCodec::format = format;
			AudioCodec::channels = channels;
		}
		
		void encode(const char *ofname, size_t blocksize, double dctFrac) {
			AudioCodec::blocksize = blocksize;
			AudioCodec::dctFrac = dctFrac;

			BitStream obs {ofname, 'w'};
			
			string aux = encodeHeader();
			char header[100];
			strcpy(header, aux.c_str());
			obs.writeNBits(header, 100);

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

		void decode(const char *ofname) {
			BitStream obs {ofname, 'r'};
			char header[100];
			obs.readNBits(header, 100);
			decodeHeader(header);

			SndfileHandle sfhOut {"out.wav", SFM_WRITE, format, channels, samplerate};
			if(sfhOut.error()) {
				cerr << "Error: invalid output file\n";
				return;
			}

			size_t blocks { static_cast<size_t>(ceil(static_cast<double>(frames) / blocksize)) };

			// Vector for holding DCT computations
			vector<double> x(blocksize);

			// Inverse DCT
			char w[16];
			
			// Vector for holding all DCT coefficients, channel by channel
			vector<vector<double>> x_dct(channels, vector<double>(blocks * blocksize));
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
