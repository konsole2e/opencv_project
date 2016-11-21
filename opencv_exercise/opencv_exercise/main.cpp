#include "settings.h"
#include "use_debug.h"

using namespace cv;
using namespace std;

bool motionDetect(Mat &frame){
	static bool init = true;
	static Mat prevFrame, diff;
	Mat targetRegion = frame(Rect(Point2f(GROUND_POINT_0_X, GROUND_POINT_0_Y), Point2f(GROUND_POINT_2_X, GROUND_POINT_2_Y)));

	cvtColor(targetRegion, targetRegion, CV_RGB2GRAY);
	GaussianBlur(targetRegion, targetRegion, Size(1, 1), 1000);

	if (init){
		prevFrame = targetRegion.clone();
		init = false;

		return false;
	}
	
	absdiff(prevFrame, targetRegion, diff);
	threshold(diff, diff, 50, 255, THRESH_BINARY);

#if DEBUG_IMSHOW
	imshow("targetRegion", targetRegion);
	imshow("diff", diff);
#endif

	prevFrame = targetRegion.clone();
	cout << countNonZero(diff) << endl;
	if (countNonZero(diff) > 0)
		return true;
	else
		return false;
}

int main(){
	VideoCapture capture(0);
	Mat dilate, erode;
	Mat hsv;
	Mat frame;

	cout << "started";
	
	/*
	frame = imread("2.jpg", 0);
	Mat croppedFrame = frame(Rect(GROUND_POINT_0_X, GROUND_POINT_0_Y, abs(GROUND_POINT_0_X - GROUND_POINT_1_X), abs(GROUND_POINT_0_Y - GROUND_POINT_3_Y)));

	Mat gaussianFrame, threshFrame;
	
	GaussianBlur(frame, gaussianFrame, Size(1, 1), 1000);
	threshold(gaussianFrame, threshFrame, 100, 255, THRESH_BINARY);

	//imshow("ThreshFrame", threshFrame);
	imshow("crop", croppedFrame);
	waitKey(0);
	*/

	if (!capture.isOpened()){
		cout << "Could not open camera" << endl;
		return 0;
	}

	double fps = capture.get(CV_CAP_PROP_FPS);
	int delay = 1000 / fps;
	double totalFrame = capture.get(CV_CAP_PROP_FRAME_COUNT);
	char p[20];

	while (1){
		if (!capture.read(frame))
			break;
		
		line(frame, Point2f(GROUND_POINT_0_X, GROUND_POINT_0_Y), Point2f(GROUND_POINT_1_X, GROUND_POINT_1_Y), Scalar(0, 255, 0), 1);
		line(frame, Point2f(GROUND_POINT_1_X, GROUND_POINT_1_Y), Point2f(GROUND_POINT_2_X, GROUND_POINT_2_Y), Scalar(0, 255, 0), 1);
		line(frame, Point2f(GROUND_POINT_2_X, GROUND_POINT_2_Y), Point2f(GROUND_POINT_3_X, GROUND_POINT_3_Y), Scalar(0, 255, 0), 1);
		line(frame, Point2f(GROUND_POINT_3_X, GROUND_POINT_3_Y), Point2f(GROUND_POINT_0_X, GROUND_POINT_0_Y), Scalar(0, 255, 0), 1);

		if (!motionDetect(frame)){
			sprintf(p, "None");
		}
		else{
			sprintf(p, "Motion Detected!!!");
		}

		putText(frame, p, Point(20, 20), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		
		//잘라내기
		Point2f inputQuad[4];
		Point2f outputQuad[4];
		Mat croppedFrame = frame(Rect(GROUND_POINT_0_X, GROUND_POINT_0_Y, abs(GROUND_POINT_0_X - GROUND_POINT_1_X), abs(GROUND_POINT_0_Y - GROUND_POINT_3_Y))).clone();
		Mat warpedFrame;
		Mat lambda(2, 4, CV_32FC1);

		lambda = Mat::zeros(croppedFrame.rows, croppedFrame.cols, croppedFrame.type());

		inputQuad[0] = Point2f(GROUND_POINT_0_X, GROUND_POINT_0_Y);
		inputQuad[1] = Point2f(GROUND_POINT_1_X, GROUND_POINT_1_Y);
		inputQuad[2] = Point2f(GROUND_POINT_2_X, GROUND_POINT_2_Y);
		inputQuad[3] = Point2f(GROUND_POINT_3_X, GROUND_POINT_3_Y);

		double ratio = GROUND_POINT_HEIGHT / (double)GROUND_POINT_WIDTH;
		int height = GROUND_POINT_HEIGHT;
		int width = height * ratio;

		outputQuad[0] = Point2f(0, 0);
		outputQuad[1] = Point2f(width, 0);
		outputQuad[2] = Point2f(width, height);
		outputQuad[3] = Point2f(0, height);

		lambda = getPerspectiveTransform(inputQuad, outputQuad);
		// Apply the Perspective Transform just found to the src image
		warpPerspective(croppedFrame, warpedFrame, lambda, frame.size());

		imshow("warppedFrame", warpedFrame);
		imshow("croppedFrame", croppedFrame);
		imshow("webcam", frame);
		if (waitKey(10) == 27) return 1;
	}

	/*
	VideoCapture vc("test.mp4");
	if (!vc.isOpened()) 
		return 1; // 불러오기 실패

	Mat img;
	Mat frame;

	double fps = vc.get(CV_CAP_PROP_FPS);
	int delay = 1000 / fps;
	double frameNumber;
	double totalFrame = vc.get(CV_CAP_PROP_FRAME_COUNT);
	double percentage = 0;
	char p[20];

	while (1){
		if (!vc.read(frame))
			break;
		frameNumber = vc.get(CV_CAP_PROP_POS_FRAMES);
		percentage = frameNumber / totalFrame * 100;

		cout << fps << " " << percentage << " " << frameNumber << " " << totalFrame << endl;
		sprintf(p, "%.0f %%", percentage);

		putText(frame, p, Point(20, 20), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

		cvtColor(frame, frame, CV_BGR2GRAY);
		imshow("video", frame);
		if (waitKey(delay) == 27)
			break;
	}
	*/
	return 0;
}