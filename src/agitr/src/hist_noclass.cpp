#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <cfloat>

using namespace std;
using namespace cv;

// declaring a lot of variables :P
Mat src, hsv, bgrThresh, hsvThresh, ROI, dst, hsvResult, bgrResult;
// Mat b_hist, g_hist, r_hist, h_hist, s_hist, v_hist;

// Multidimensional Mat
MatND bgrHist, hsvHist;

// int count=0;
char ch;
int flag=0;

vector<Mat> bgr_planes;

Point pt1, pt2;

// The Offset of the rectangle in src image
int OFFSET_x = 25;
int OFFSET_y = 25;

// Opening camera
// VideoCapture cam(1);

// we compute the BGR-histogram from the 0-th, 1-st and 2-nd channels
int bgrChannels[] = {0, 1, 2};

// Establish the number of bins
int bgrHistSize[] = {256, 256, 256};

// Set the ranges ( for B,G,R )
float bgrRange[] = { 0, 256 } ;
const float* bgrHistRange[] = { bgrRange, bgrRange, bgrRange };

// Quantize the hue to 30 levels
// and the saturation to 32 levels
int hbins = 90, sbins = 128;
int histSize[] = {hbins, sbins};

// we compute the HSV-histogram from the 0-th and 1-st channels
int hsvChannels[] = {0, 1};

// hue varies from 0 to 179
float hranges[] = { 0, 180 };

// saturation varies from 0 (black-gray-white) to
// 255 (pure spectrum color)
float sranges[] = { 0, 256 };
const float* ranges[] = { hranges, sranges };

int dims = 2;

// image_pub_ = it_.advertise("image_converter/output_video", 1);

void imageCb(const sensor_msgs::ImageConstPtr& msg);

int main(int argc, char** argv)
{
	ros::init(argc, argv, "image_converter");
	// ImageConverter ic;
	// ros::spin();

	ros::NodeHandle nh_;
	image_transport::ImageTransport it_(nh_);
	image_transport::Subscriber image_sub_;
	image_transport::Publisher image_pub_;

	const std::string SUBS_NAME = "cv_camera/image_raw";
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe(SUBS_NAME, 1, 
      &imageCb);
    // image_pub_ = it_.advertise("image_converter/output_video", 1);
	
    namedWindow("Trackbars");

    ros::spin();

	return 0;
}

void imageCb(const sensor_msgs::ImageConstPtr& msg)
{
	cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

	// while(1)
	// {
		// cam.read(src);
    	src=cv_ptr->image;
		// flip(src, src, 1);

		// GaussianBlur(src, src, Size(5,5), 0);
		// adaptiveThreshold(src, src, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5, 2);

		cvtColor(src, hsv, CV_BGR2HSV);
		// Mat grey;
		// cvtColor(src, grey, CV_BGR2GRAY);
		// split(src, bgr_planes);
		// for(int i=0; i<3; ++i)
		// {
		// // 	GaussianBlur(bgr_planes[i], bgr_planes[i], Size(5,5), 0);
		// 	subtract(bgr_planes[i], grey, bgr_planes[i]);
		// 	adaptiveThreshold(bgr_planes[i], bgr_planes[i], 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 5, 2);
		// 	// threshold(bgr_planes[i], bgr_planes[i], 0, 255, THRESH_BINARY + THRESH_OTSU);
		// }
		// merge(bgr_planes, src);
		// vector<Mat> faltu;
		// for(int i=0; i<3; ++i)
		// 	faltu.push_back(bgr_planes[1]);
		// Mat OUT;
		// merge(faltu, OUT);
		// dilate(OUT, OUT, Mat());
		// erode(OUT, OUT, Mat());
		// threshold(OUT, OUT, 0, 255, THRESH_BINARY + THRESH_OTSU);
		// medianBlur(OUT,OUT,3);
		// imshow("OUT", OUT);

		if(!flag)
		{
			createTrackbar("Width", "Trackbars", &OFFSET_x, ((src.cols)/2)-15);
			createTrackbar("Height", "Trackbars", &OFFSET_y, ((src.rows)/2)-15);

			pt1.x = (src.cols/2) - (OFFSET_x+5);
			pt1.y = (src.rows/2) - (OFFSET_y+5);
			pt2.x = (src.cols/2) + (OFFSET_x+5);
			pt2.y = (src.rows/2) + (OFFSET_y+5);

			rectangle(src, pt1, pt2, Scalar(0,255,0));

			Mat temp = src( Rect(pt1, pt2) );
			ROI = temp.clone();
			
			// Mat image;
			// image = imread("white.jpg", CV_LOAD_IMAGE_COLOR);
			// // imshow("img", image);
			// ROI = image.clone();

			// Mat image;
			// image = imread("black.jpg", CV_LOAD_IMAGE_COLOR);
			// // imshow("img", image);
			// ROI = image.clone();

			// calcHist( &ROI, 1, bgrChannels, Mat(), bgrHist, 2, bgrHistSize, bgrHistRange);

			cvtColor(ROI, ROI, CV_BGR2HSV);

			calcHist( &ROI, 1, hsvChannels, Mat(), hsvHist, dims, histSize, ranges);

			// normalize(bgrHist, bgrHist, 0, 255, NORM_MINMAX);
			normalize(hsvHist, hsvHist, 0, 255, NORM_MINMAX);
		}
		else
		{
			calcBackProject( &hsv, 1, hsvChannels, hsvHist, hsvResult, ranges);
			// calcBackProject( &src, 1, bgrChannels, bgrHist, bgrResult, bgrHistRange);
			
			// imshow("beforethresh_hsv", hsvResult);
			// imshow("beforethresh_bgr", bgrResult);

			// Mat disc = getStructuringElement(MORPH_ELLIPSE, Size(5,5));

			// filter2D(hsvResult, hsvResult, -1, disc);
			// GaussianBlur(hsvResult, hsvResult, Size(5,5), 0);
			// GaussianBlur(bgrResult, bgrResult, Size(5,5), 0);
			// Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3,3));
			// erode(hsvResult, hsvResult, element);
			// element = getStructuringElement(MORPH_ELLIPSE, Size(3,3));
			// dilate(hsvResult, hsvResult, element);
			// medianBlur(hsvResult, hsvResult, 3);

			Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3,3));
			erode(hsvResult, hsvResult, element);
			dilate(hsvResult, hsvResult, element);
			dilate(hsvResult, hsvResult, element);
			

			// adaptiveThreshold(hsvResult, hsvThresh, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5, 2);
			threshold(hsvResult, hsvThresh, 0, 255, THRESH_BINARY);
			// threshold(bgrResult, bgrThresh, 50, 255, THRESH_BINARY + THRESH_OTSU);

			// Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3,3));
			element = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
			erode(hsvThresh, hsvThresh, element);
			element = getStructuringElement(MORPH_ELLIPSE, Size(13,13));
			dilate(hsvThresh, hsvThresh, element);
			medianBlur(hsvThresh, hsvThresh, 5);

			// vector<Mat> th;
			// for(int i=0; i<3; ++i)
			// 	th.push_back(thresh);
			// merge(th, thresh);

			// bitwise_and(src, thresh, dst);

			string str;
			string str1;
			
			vector<vector<Point> > contour;
			vector<Vec4i> hierarchy;
			vector<Point> approx;
			Mat abc = hsvThresh.clone();
			findContours(abc, contour, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, Point(0,0));
			if(contour.size())                        
			{                                                   //if contours are made
				double maxarea = contourArea(contour[0]);
			    double area;
			    int maxindex=0;
			    for(int i=1; i<contour.size(); i++)
			    {
					area = contourArea(contour[i]);
			      	if(area>maxarea)
			      	{
			       		maxarea = area;
			       	 	maxindex = i;
			      	}
			    }
				approxPolyDP( Mat(contour[maxindex]), approx, arcLength(Mat(contour[maxindex]), true)*0.035, true );
				for(int i=0; i<approx.size(); ++i)
				{
					line(src, approx[i], approx[(i+1)%approx.size()], Scalar(0,255,0), 2);
				}
			}

			imshow("hsvResult", hsvResult);
			imshow("hsvThresh", hsvThresh);

			// if(ch == 'o')
			// {
			// 	for(; count < 9; ++count)
			// 	{
			// 		// str1[0] = (char)count;
			// 		// str1[1] = '\0';
			// 		// str = "out" + str1 + ".jpg";
			// 		stringstream ss;
			// 		ss << "out" << count << ".jpg";
			// 		ss >> str;

			// 		cerr << str << endl;
			// 		imwrite(str, hsvThresh);	
			// 	}
			// 	count = 0;
			// }
			
			// imshow("bgrThresh", bgrThresh);
			
			// bitwise_and(hsvThresh, bgrThresh, dst);
			// imshow("dst", dst);

		}

		imshow("src", src);
		// imshow("hsv", hsv);
		
		ch = waitKey(50);
		if(ch == 'q' || ch == 27)
			exit(0);
		else if(ch == 'g')
			{
				flag=!flag;
				Mat out;
				cvtColor(ROI, out, CV_HSV2BGR);
				imwrite("ROI.jpg", out);
			}
	// }
	// image_pub_.publish(cv_ptr->toImageMsg());
}