#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main(void) {
	//bool movement = false;
	long frame_counter = 0, difference_total_value = 0;
	Mat frame, frame_coloured, frame_past, difference;
	VideoCapture cap(0);

	int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	difference = Mat::zeros(height, width, CV_8UC1);

	namedWindow("Capture", WINDOW_AUTOSIZE);
	namedWindow("Difference", WINDOW_AUTOSIZE);

	VideoWriter video_out("out-test.mp4", CV_FOURCC('X', '2', '6', '4'), 24, Size(width, height));

	while (true) {
		cap >> frame;
		frame_coloured = frame;

		cvtColor(frame, frame, CV_BGR2GRAY);
		resize(frame, frame, Size(128,128)); //use this to make processing faster
		//GaussianBlur(frame, frame, Size(7, 7), 0, 0); //use this to smooth the output 


		difference_total_value = 0;
		for (int i = 0; i < frame.rows; i++) {
			for (int j = 0; j < frame.cols; j++) {
				if (frame_counter == 0) {
					difference.at<uchar>(i, j) = 0;
				}
				else {
					difference.at<uchar>(i, j) = abs(frame.at<uchar>(i, j) - frame_past.at<uchar>(i, j));
				}
				difference_total_value += difference.at<uchar>(i, j);
			}
		}

		frame_past = frame;
		frame_counter++;

		cout << "frame: " << frame_counter << " difference = " << difference_total_value << endl;

		if (difference_total_value > 1000000/10) {
			//movement = true;
			cout << "THERE'S MOVEMENT!" << endl;
			video_out.write(frame_coloured);

		}
		/*else {
			movement = false;
		}*/

		imshow("Capture", frame_coloured);
		imshow("Difference", difference);

		if (waitKey(30) >= 0) break;
	}
	cap.release();
	video_out.release();
	destroyAllWindows();
	return 0;
}