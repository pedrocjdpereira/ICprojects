#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {
	/* Verify arguments */
	if(argc < 3) {		cerr << "Usage: img_cpy <original file> <copy file>\n";
		return 1;
	}

	/* Read image passed as argument */
	Mat original = imread(argv[argc - 2]);
	if(original.empty()) {
		cerr << "Error: Image not found\n";
		return 1;
	}

	/* Create copy image object */
	Mat copy(original.rows, original.cols, original.type());
	
	/* Copy original image pixel by pixel */
	for(int i = 0; i < original.rows; i++){
		for(int j = 0; j < original.cols; j++){
			Vec3b pixel = Vec3b(original.ptr<Vec3b>(i)[j][0], original.ptr<Vec3b>(i)[j][1], original.ptr<Vec3b>(i)[j][2]);
			copy.ptr<Vec3b>(i)[j] = pixel;
		}
	}

	/* Write copied image to file */
	if(!imwrite(argv[argc - 1], copy)) {
		cerr << "Error: Failed to write image to file!\n";
		return 1;
	}

	return 0;
}
