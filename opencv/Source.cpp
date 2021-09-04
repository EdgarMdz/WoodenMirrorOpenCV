#include <iostream>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define WIDTH 20
#define HEIGHT 20

using namespace std;
using namespace cv;



Mat** createSections(Mat image);
Mat makeImage(Mat** array);
Mat setAvarageColor(Mat image);

int main()
{
	VideoCapture cap(0);
	//setting resolution
	cap.set(3, 1080);
	cap.set(4, 720);
	
	if (!cap.isOpened()) {
		cout << "Change camera port\n\n";
		return -1;
	}


	CascadeClassifier faceCascade;
	String faceCascadename = "haarcascade_frontalface_default.xml";

	//loading cascade
	if (!faceCascade.load(faceCascadename)) { return -1; }
	

	while (true)
	{
		//frames to store camera input and convert it to gray scale
		Mat frame,grayFrame,faceImage;
		
		//getting frame from camera
		cap.read(frame);
		

		//converting frame to grayscale
		cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);
		

	
		//using cascade
		vector<Rect> faces;
		faceCascade.detectMultiScale(grayFrame, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
		
		//reconstructing image
		Mat** sections;

		//getting first face detected in order to get a less detailed image to reconstruct with a lower definition
		if (faces.size() > 0) //if faces were detected
		{
			sections = createSections(grayFrame(faces[0]));
		}
		else//if not faces were detected 
		{
			 sections = createSections(grayFrame);
		}

		faceImage = makeImage(sections);

		resize(faceImage, faceImage, Size(500, 500));
			
		//showing frame
		try {
			imshow("Webcam Frame", faceImage);
		}
		catch (cv::Exception & e) {
			cerr << e.msg << endl; // output exception message
			break;
		}

		delete[] sections;

		//wait key to exit
		if (waitKey(30) >= 0) {
			break;
		}
	}

	destroyAllWindows();
	return 0;
}

Mat** createSections(Mat image) {
	//setting first dimension
	Mat** arr = new Mat * [HEIGHT];
	
	//setting second dimension
	for (int i = 0; i < HEIGHT; i++)
	{
		arr[i] = new	Mat[WIDTH];
	}

	//step per each axis
	int  wStep = image.cols / WIDTH;
	int hStep = image.rows / HEIGHT;

	//trimming image
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			int x, y, deltaX, deltaY;
			//starting coordinates
			x = j * wStep;
			y = i * hStep;

			//cropped image width and height
			deltaX = wStep;
			deltaY = hStep;

			//subtracting 1 pixel from the image when whichever of the loops are in their last iteration
			if (j == WIDTH - 1)
				deltaX -= 1;

			if (i == HEIGHT - 1)
				deltaY -= 1;

			//getting cropped immage
			arr[i][j] = setAvarageColor(image(Rect(x, y, deltaX, deltaY)));
		}
	}
	return arr;
}

Mat makeImage(Mat** inputArray) {

	Mat result;
	vector<Mat> images;
	
	//concatentating horizontally
	for (int i = 0; i < HEIGHT; i++)
	{
		Mat outputArray;
		hconcat(inputArray[i], WIDTH, outputArray);
		images.push_back(outputArray);//saving image strip to concatenating later
		outputArray.release();
	}

	//concatenating vertically
	vconcat(images, result);

	return result;
}

Mat setAvarageColor(Mat image)
{
	int color = 0;
	
	for (int i = 0; i < image.cols; i++)
	{
		for (int j = 0; j < image.rows; j++)
		{
			color = image.at<uchar>(j, i) / 2;
		}
	}

	image.setTo(Scalar(color, color, color));
	return image;
}