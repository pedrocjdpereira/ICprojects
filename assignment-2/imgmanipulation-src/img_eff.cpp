#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/* Auxiliary functions prototypes */
Mat negative(Mat original);
Mat mirror_horizontally(Mat original);
Mat mirror_vertically(Mat original);
Mat rotate(Mat original, int angle);
Mat intensity(Mat original, int beta);

map<string,int> flagmap = {
	{"-n", 1},		/* Negative flag */
	{"-mh", 2},		/* Mirrored horizontally flag */
	{"-mv", 3},		/* Mirrored vertically flag */
	{"-r", 4},		/* Rotate flag */
	{"-i", 5}		/* Intensity flag */
};

int main(int argc, char *argv[]) {
	/* Verify arguments */
	if(argc < 4) {
		cerr << "Usage: img_cpy <option(s)> <original file> <new file>\n";
		cerr << "Options:\n";
		cerr << "\t-n\t\t\tNegative version\n";
		cerr << "\t-mh\t\t\tMirrored horizontally\n";
		cerr << "\t-mv\t\t\tMirrored vertically\n";
		cerr << "\t-r <angle>\t\tRotate image (angle must be multiple of 90)\n";
		cerr << "\t-i <beta value>\t\tIncrease/decrease intensity values (beta must be between -100 and 100)\n";
		return 1;
	}

	/* Read image passed as argument */
	Mat image = imread(argv[argc - 2]);
	if(image.empty()) {
		cerr << "Error: image not found!\n";
		return 1;
	}

	/* Apply effects to image */
	for(int i = 1; i < argc-2; i++) {
		switch(flagmap[argv[i]]) {
			case 1:
				image = negative(image);
				break;
			case 2:
				image = mirror_horizontally(image);
				break;
			case 3:
				image = mirror_vertically(image);
				break;
			case 4:
				image = rotate(image, stoi(argv[++i]));
				break;
			case 5:
				image = intensity(image, stoi(argv[++i]));
				break;
			default:
				cerr << "Invalid option! No option " << argv[i] << " available!\n";
				return 1;
		}
	}

	/* Write edited image to file */
	if(!imwrite(argv[argc - 1], image)) {
		cerr << "Error: Failed to write image to file!\n";
		return 1;
	}

	return 0;
}

/*** Auxiliar functions ***/

/* Apply negative effect to an image */
Mat negative(Mat original) {
	/* Create copy image object */
	Mat neg(original.rows, original.cols, original.type());

	/* Apply negative effect to each pixel and save it to neg */	
	for(int i = 0; i < original.rows; i++){
		for(int j = 0; j < original.cols; j++){
			neg.ptr<Vec3b>(i)[j] = Vec3b(255 - original.ptr<Vec3b>(i)[j][0],
					  					 255 - original.ptr<Vec3b>(i)[j][1],
					  					 255 - original.ptr<Vec3b>(i)[j][2]);
		}
	}

	return neg;
}

/* Mirror an image horizontally */
Mat mirror_horizontally(Mat original) {
	Mat mir(original.rows, original.cols, original.type());

	/* Store mirrored pixels on mir */
	for(int i = 0; i < original.rows; i++){
		for(int j = 0; j < original.cols; j++){
			mir.ptr<Vec3b>(original.rows-1 - i)[j] = Vec3b(original.ptr<Vec3b>(i)[j][0],
														 original.ptr<Vec3b>(i)[j][1],
														 original.ptr<Vec3b>(i)[j][2]);
		}
	}

	return mir;
}

/* Mirror an image vertically */
Mat mirror_vertically(Mat original) {
	Mat mir(original.rows, original.cols, original.type());

	/* Store mirrored pixels on mir */
	for(int i = 0; i < original.rows; i++){
		for(int j = 0; j < original.cols; j++){
			mir.ptr<Vec3b>(i)[original.cols-1 - j] = Vec3b(original.ptr<Vec3b>(i)[j][0],
														 original.ptr<Vec3b>(i)[j][1],
														 original.ptr<Vec3b>(i)[j][2]);
		}
	}

	return mir;
}

/* Rotate an image a angle mulitple of 90 */
Mat rotate(Mat original, int angle) {
	/* Check if angle is mulitple of 90 */
	if(angle % 90 != 0) {
		throw invalid_argument("Angle is not multiple of 90!");
	}

	/* Enable support for negative angles */
	if(angle < 0) {
		angle = 360 - ( (-angle) % 360 );
	}

	/* Check if image is rotated an angle multiple of 180 degrees */
	/* If not, then the width and the length must be switched */
	Mat rot;
	if(angle % 180 != 0) {
		rot = Mat(original.cols, original.rows, original.type());
	} else {
		rot = Mat(original.rows, original.cols, original.type());
	}

	/* Store rotated pixels on rot */
	for(int i = 0; i < original.rows; i++){
		for(int j = 0; j < original.cols; j++){
			if(angle % 360 == 0) {
				rot.ptr<Vec3b>(i)[j] = Vec3b(original.ptr<Vec3b>(i)[j][0],
											 original.ptr<Vec3b>(i)[j][1],
											 original.ptr<Vec3b>(i)[j][2]);
			}
			else if(angle % 270 == 0) {
				rot.ptr<Vec3b>(j)[original.rows-1 - i] = Vec3b(original.ptr<Vec3b>(i)[j][0],
															   original.ptr<Vec3b>(i)[j][1],
															   original.ptr<Vec3b>(i)[j][2]);
			}
			else if(angle % 180 == 0) {
				rot.ptr<Vec3b>(original.rows-1 - i)[original.cols-1 - j] = Vec3b(original.ptr<Vec3b>(i)[j][0],
																				 original.ptr<Vec3b>(i)[j][1],
																				 original.ptr<Vec3b>(i)[j][2]);
			}
			else if (angle % 90 == 0) {
				rot.ptr<Vec3b>(original.cols-1 - j)[i] = Vec3b(original.ptr<Vec3b>(i)[j][0],
															   original.ptr<Vec3b>(i)[j][1],
															   original.ptr<Vec3b>(i)[j][2]);
			}
		}
	}
	
	return rot;
}

/* Change intensity of an image */
Mat intensity(Mat original, int beta) {
	/* Check if beta is between -100 and 100 */
	if(beta > 100 || beta < -100) {
		throw invalid_argument("Beta is not between -100 and 100!");
	}

	Mat eff(original.rows, original.cols, original.type());

	/* Apply intesity change on each pixel */
	for(int i = 0; i < original.rows; i++){
		for(int j = 0; j < original.cols; j++){
			eff.ptr<Vec3b>(i)[j] = Vec3b(saturate_cast<uchar>(original.ptr<Vec3b>(i)[j][0] + beta),
						  				 saturate_cast<uchar>(original.ptr<Vec3b>(i)[j][1] + beta),
										 saturate_cast<uchar>(original.ptr<Vec3b>(i)[j][2] + beta));
		}
	}

	return eff;
}
