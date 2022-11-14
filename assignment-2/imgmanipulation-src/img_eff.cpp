#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char *argv[]) {

	if(argc < 4) {
		std::cerr << "Usage: img_cpy <original file> <copy file> <option>\n";
		std::cerr << "Options:\n";
		std::cerr << "0-Negative version\n1-Mirrored horizontally\n2-Mirrored vertically\n";
		std::cerr << "3-Rotate image\n";
		std::cerr << "4-Increase/decrease intensity values\n";
		return 1;
	}

	Mat ogimage = cv::imread(argv[argc - 3]);
	if (ogimage.empty())
	{
		std::cerr << "!!! Failed imread(): image not found";
		return 1;
	}
	Mat imagecopy(ogimage.rows, ogimage.cols, ogimage.type());

	int option = atoi(argv[argc - 1]);
	int angle;
	int beta;
	if(option == 3){
		std::cout << "Insert the angle of rotation:";
		std::cin >> angle;
		while(!((angle == 90) | (angle == 180) | (angle == 270))){
			std::cout << "Angle must be a multiple of 90º!\n";
			std::cout << "Insert the angle of rotation:";
			std::cin >> angle;
		}
	}
	else if(option == 4){
		std::cout << "Insert the beta value:";
		std::cin >> beta;
		while(!((beta >= -100) & (beta <= 100))){
			std::cout << "Beta must be more than -100 and less than 100!\n";
			std::cout << "Insert the beta value:";
			std::cin >> beta;
		}
	}
	for(int i = 0; i < ogimage.rows; i++){
		for(int j = 0; j < ogimage.cols; j++){
			Vec3b pixel;
			if(option == 0){
				pixel = Vec3b(255 - ogimage.ptr<Vec3b>(i)[j][0], 255 - ogimage.ptr<Vec3b>(i)[j][1], 255 - ogimage.ptr<Vec3b>(i)[j][2]);
			}
			else if(option == 4){
				pixel = Vec3b(saturate_cast<uchar>(ogimage.ptr<Vec3b>(i)[j][0] + beta ), saturate_cast<uchar>(ogimage.ptr<Vec3b>(i)[j][1] + beta ), saturate_cast<uchar>(ogimage.ptr<Vec3b>(i)[j][2] + beta ));
			}
			else{
				pixel = Vec3b(ogimage.ptr<Vec3b>(i)[j][0], ogimage.ptr<Vec3b>(i)[j][1], ogimage.ptr<Vec3b>(i)[j][2]);
			}

			if(option == 0){
				imagecopy.ptr<Vec3b>(i)[j] = pixel;
			}
			else if(option == 1){
				imagecopy.ptr<Vec3b>(ogimage.rows - i)[j] = pixel;
			}
			else if(option == 2){
				imagecopy.ptr<Vec3b>(i)[ogimage.cols - j] = pixel;
			}
			else if(option == 3){
				if (angle == 90){
					imagecopy.ptr<Vec3b>(ogimage.rows - j)[i] = pixel;
				}
				else if(angle == 180){
					imagecopy.ptr<Vec3b>(ogimage.rows - i)[ogimage.cols - j] = pixel;
				}
				else if(angle == 270){
					imagecopy.ptr<Vec3b>(j)[ogimage.rows - i] = pixel;
				}
			}
			else if(option == 4){
				imagecopy.ptr<Vec3b>(i)[j] = pixel;
			}
			else{
					std::cerr << "Invalid option";
					return 1;
			}
		}
	}
	imwrite(argv[argc - 2], imagecopy);
}
