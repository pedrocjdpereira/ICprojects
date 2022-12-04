#include <iostream>
#include "limagecodec.cpp"

using namespace std;

int main(int argc, char *argv[]) {
	if(argc < 2) {
		cerr << "Usage: test_codec <image>\n";
		cerr << "Explanation: This program receives an image as argument,";
		cerr << " encodes it and then decodes it using the codec developed.\n";
		return -1;
	}

	LosslessImageCodec encoder {argv[argc-1], 'e'};
	encoder.encode("encoded_image.lic");
	cout << "Encoded image file: encoded_image.lic\n";
	LosslessImageCodec decoder {"encoded_image.lic", 'd'};
	decoder.decode("decoded_image.ppm");
	cout << "Decoded image file: decoded_image.ppm\n";
	return 0;
}
