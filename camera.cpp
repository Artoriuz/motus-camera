#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <opencv2/opencv.hpp>
#include <ctime>
#include <iostream>
using namespace cv;
using namespace std;

string get_time() {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H-%M-%S", timeinfo);
	string date_string_output(buffer);

	return date_string_output;
}

void draw_time_on_frame(Mat input_frame) {
	string draw_string = get_time();
	putText(input_frame, draw_string, Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 0));
}

int main(void) {
	bool movement_state, previous_movement_state;
	long frame_counter = 0, difference_total_value = 0, last_movement_frame = 0;
	int recording_count = 0;
	Mat frame, frame_coloured, frame_past, difference;
	VideoCapture cap(0);

	double width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	difference = Mat::zeros(height, width, CV_8UC1);

	namedWindow("Capture", WINDOW_AUTOSIZE);
	namedWindow("Difference", WINDOW_AUTOSIZE);

	videowriter:
	
	string date_str = get_time();
	VideoWriter video_out("recording-" + date_str + ".mp4", CV_FOURCC('a', 'v', 'c', '1'), 24, Size(width, height));

	movement_state = previous_movement_state = false;

	while (true) {
		cap >> frame;
		frame_coloured = frame;
		
		draw_time_on_frame(frame_coloured);

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

		//cout << "frame: " << frame_counter << " difference = " << difference_total_value << endl;

		if (movement_state = true && frame_counter < last_movement_frame + 100) {
			video_out.write(frame_coloured);
			movement_state = true;
		} else {
			if (difference_total_value > frame.rows * 750) {
				//cout << difference_total_value << endl;
				video_out.write(frame_coloured);
				last_movement_frame = frame_counter;
				movement_state = true;
			}
			else {
				movement_state = false;
			}
		}

		if (previous_movement_state == true && movement_state == false) {
			video_out.release();
			recording_count++;
			goto videowriter;
		}
		
		cout << movement_state << " " << previous_movement_state << endl;
		previous_movement_state = movement_state;
		

		imshow("Capture", frame_coloured);
		imshow("Difference", difference);

		if (waitKey(30) >= 0) break;
	}
	cap.release();
	video_out.release();
	destroyAllWindows();
	//system("ffmpeg -i out-test.mp4 -c:v libvpx-vp9 -b:v 0 -crf 28 test-vp9.mkv");
	return 0;
}