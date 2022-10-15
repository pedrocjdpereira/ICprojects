#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>
#include <iostream>
#include <fstream>

class WAVHist {
  private:
	std::vector<std::map<short, size_t>> counts;

  public:
	WAVHist(const SndfileHandle& sfh) {
		counts.resize(sfh.channels() + 2);
	}

	void update(const std::vector<short>& samples) {
		
		size_t n { };
		short val = 0;
		for(auto s : samples){
			counts[n++ % 2][s]++;
			if(n % 2 != 0){
				val = s;
			}
			if(n % 2 == 0){
				counts[2][(val+s)/2]++;
				counts[3][(val-s)/2]++;
			}
		}
	}

	void dump(const size_t channel) const {
		std::ofstream myfile;
		myfile.open ("hist.txt");
		for(auto [value, counter] : counts[channel]){
			myfile << value << ',' << counter << '\n';
			std::cout << value << '\t' << counter << '\n';
		}
		myfile.close();
	}
};

#endif

