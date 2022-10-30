#include <vector>
#include <sndfile.hh>
#include <math.h>
#include "bitstream.h"

using namespace std;

constexpr int ARRAY_SIZE = 8;

int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		cerr << "Usage: encod_decod <original file> <new file> <option>\n";
		cerr << "\nOptions:\n";
		cerr << "\t0 - Text to Binary\n";
		cerr << "\t1 - Binary to Text\n";
		return 1;
	}

	char *ofname = argv[argc-3];
	char *nfname = argv[argc-2];
	int option = atoi(argv[argc-1]);
	if(option != 0 && option != 1) {
		cerr << "Invalid option!\n";
		return 1;
	}

	if(option) {
		BitStream obs {ofname, 'r'};
		fstream nfs {nfname, ios::out};
		long long size = obs.byteSize() * 8;
		while(size-- > 0) {
			char bit = obs.readBit();
			nfs << ((bit == 1) ? '1' : '0') << endl;
		}

		obs.close();
		nfs.close();
	} else {
		fstream ofs {ofname, ios::in};
		BitStream nbs {nfname, 'w'};

		string ln;
		while(getline(ofs, ln)) {
			if(ln == "1")
				nbs.writeBit(1);
			if(ln == "0")
				nbs.writeBit(0);
		}

		ofs.close();
		nbs.close();
	}

	return 0;
}
