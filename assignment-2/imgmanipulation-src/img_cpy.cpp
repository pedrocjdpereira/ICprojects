#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char *argv[]) {

	if(argc < 3) {
		std::cerr << "Usage: img_cpy <original file> <copy file> \n";
		return 1;
	}

	Mat ogimage = cv::imread(argv[argc - 2]);
	if (ogimage.empty())
	{
		std::cerr << "!!! Failed imread(): image not found";
		return 1;
	}
	Mat imagecopy(ogimage.rows, ogimage.cols, ogimage.type());

	for(int i = 0; i < ogimage.rows; i++){
		for(int j = 0; j < ogimage.cols; j++){
			Vec3b pixel = Vec3b(ogimage.ptr<Vec3b>(i)[j][0], ogimage.ptr<Vec3b>(i)[j][1], ogimage.ptr<Vec3b>(i)[j][2]);
			imagecopy.ptr<Vec3b>(i)[j] = pixel;
		}
	}

	imwrite(argv[argc - 1], imagecopy);
}
