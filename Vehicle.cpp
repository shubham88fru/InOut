#include "Vehicle.h"

const int FRAME_HEIGHT = 360;
const int FRAME_WIDTH = 640;

Vehicle::Vehicle()
{
}


void Vehicle::trackVehicles() 
{
	//Mat object to store the diffrence of the frame
	Mat diffFrame;
	
	Mat threshedImg;

	//Mat object to grab the frames from the video
	Mat  videoFrame2;
	
	//to store the copies of the frame
	Mat videoFrame2Copy, videoFrame1Copy,threshedImgCopy;

	_video >> videoFrame2;

	//resize the video to 640x360;
	resize(videoFrame2, videoFrame2, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_CUBIC);

	//copy the frame for other use
	videoFrame2Copy = videoFrame2.clone();
	videoFrame1Copy = _firstFrame.clone();

	cvtColor(videoFrame1Copy, videoFrame1Copy, CV_BGR2GRAY);
	cvtColor(videoFrame2Copy, videoFrame2Copy, CV_BGR2GRAY);

	GaussianBlur(videoFrame1Copy, videoFrame1Copy, Size(5, 5), 0);
	GaussianBlur(videoFrame2Copy, videoFrame2Copy, Size(5, 5), 0);

	absdiff(videoFrame2Copy, videoFrame1Copy, diffFrame);

	//calcute the FPS
	//putText(_firstFrame, _FPS, Point(600, 100), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 8);
	
	//display the frames
	imshow("Video", _firstFrame);

	threshold(diffFrame, threshedImg, 30, 255.0, CV_THRESH_BINARY);

	//dilate and erode for better results
	dilate(threshedImg, threshedImg, getStructuringElement(CV_SHAPE_RECT, Size(5, 5)));
	dilate(threshedImg, threshedImg, getStructuringElement(CV_SHAPE_RECT, Size(5, 5)));
	erode(threshedImg, threshedImg, getStructuringElement(CV_SHAPE_RECT, Size(5, 5)));

	//clone the threshed image because finding contors will change the image
	threshedImgCopy = threshedImg.clone();

	//find the contours

	findContours(threshedImgCopy, _contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	
	Mat contoursImg(threshedImgCopy.size(), CV_8UC3, Scalar(0,0,0));

	drawContours(contoursImg, _contours, -1, Scalar(255, 255, 255), -1);

	vector < vector<Point> > _convexHulls(_contours.size());

	////extract the convex hulls out of the contours
	for (int j=0; j < _contours.size(); j++) {

		convexHull(_contours[j], _convexHulls[j]);
	}

	convexImg=Mat(_firstFrame.size(), CV_8UC3, Scalar(0, 0, 0));

	//extract the cars out of the video
	_extractCars(_convexHulls);

	imshow("Contours", contoursImg);

	imshow("thresh", threshedImg);

	_firstFrame = videoFrame2.clone();

}

void Vehicle::getFirstframe(VideoCapture &video)
{
	_video = video;

	
	//sprintf(_FPS, "%d", _video.get(CV_CAP_PROP_FPS));

	Mat firstFrame;

	_video.read(firstFrame);

	//resize the video to 640x360;
	resize(firstFrame, firstFrame, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_CUBIC);

	_firstFrame = firstFrame;
}

void Vehicle::_extractCars(vector < vector<Point> > &convexHulls)
{

	

	for (int i = 0; i < _contours.size(); i++) {
	
		_blob.getBlobSpecs(convexHulls[i]);
     
		if (_blob.bound.area() >= 1000 && _blob.boundWidth >= 150 &&
			_blob.AspectRatio >= 0.2&&_blob.AspectRatio <= 1.2&&
			_blob.boundHeight > 100&&_blob.boundHeight<=300 && _blob.DiagonalSize > 150.0) {
		
					_Blob.push_back(_blob);

				
		}
		//_carBlobs.push_back(convexHulls[i]);
	}

	//Mat convexImg(_firstFrame.size(), CV_8UC3, Scalar(0, 0, 0));

	convexHulls.clear();

	for (int i = 0; i < _Blob.size(); i++) {
	
		convexHulls.push_back(_Blob[i].contour);

	}

	

	drawContours(convexImg, convexHulls, -1, Scalar(255, 255, 255), -1);
	imshow("Blobs", convexImg);

	for (int i = 0; i < _Blob.size(); i++) {

		rectangle(_firstFrame, _Blob[i].bound, Scalar(0, 255, 0), 1);

	}
	imshow("Video", _firstFrame);
	_Blob.clear();
}