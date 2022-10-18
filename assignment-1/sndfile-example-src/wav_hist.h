#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>
#include <fstream>
#include <filesystem>

class WAVHist {
  private:
	std::vector<std::map<short, size_t>> counts;
	std::map<short, size_t> avgcounts;
	std::map<short, size_t> diffcounts;

  public:
	WAVHist(const SndfileHandle& sfh) {
		counts.resize(sfh.channels() + 2);
	}

	void update(const std::vector<short>& samples) {
		for(size_t i=0; i < samples.size(); i++) {
			counts[i % 2][samples[i]]++;
			/*
				In samples:
					- even indexes means left channel
					- odd indexes means right channel
				When index is odd, calculate the average and the difference
				of samples for the left and right channels. Store the results in
				maps representing the histograms of the calculations.
			*/
			if(i % 2 != 0){
				avgcounts[(samples[i-1] + samples[i])/2]++;
				diffcounts[(samples[i-1] - samples[i])/2]++;
			}
		}
	}

	// NOTE: use the extension ".dat" so the files can be easily ignored by .gitignore
	void dump(const size_t channel, const bool store_histograms = false) const {
		// Output normal usage, when flag to store histograms is not used
		if(!store_histograms) {
			for(auto [value, counter] : counts[channel])
				std::cout << value << '\t' << counter << '\n';
			return;
		}

		std::string hist_path = "../plots/histograms/";
		// Create directory (if exists, does nothing)
		std::filesystem::create_directory(hist_path);

		// Store histogram of the requested channel
		std::ofstream outfile;
		outfile.open(hist_path + "hist_" + std::to_string(channel) + "_channel.dat");		
		for(auto [value, counter] : counts[channel]){
			// Output expected values as if running without flags
			std::cout << value << '\t' << counter << '\n';
			// Store in file
			outfile << value << ',' << counter << '\n';
		}
		outfile.close();
		
		// Store histogram of the other channel
		outfile.open(hist_path + "hist_" + std::to_string(channel==0) + "_channel.dat");
		for(auto [value, counter] : counts[channel==0]){
			outfile << value << ',' << counter << '\n';
		}
		outfile.close();

		// Store histogram of the average of the channels
		outfile.open(hist_path + "hist_avg.dat");
		for(auto [value, counter] : avgcounts) {
			outfile << value << ',' << counter << '\n';
		}
		outfile.close();

		// Store histogram of the difference of the channels
		outfile.open(hist_path + "hist_diff.dat");
		for(auto [value, counter] : diffcounts) {
			outfile << value << ',' << counter << '\n';
		}
		outfile.close();
	}
};

#endif

