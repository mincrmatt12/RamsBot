///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////




/***************************************************************************************************
 ** This sample demonstrates how to grab images and depth/disparity map with the ZED SDK          **
 ** Both images and depth/disparity map are displayed with OpenCV                                 **
 ** Most of the functions of the ZED SDK are linked with a key press event (using OpenCV)         **
 ***************************************************************************************************/


#include <stdio.h>
#include <string.h>
#include <ctime>
#include <chrono>
#include <iostream>
#include <vector>
#include <sstream>

#include "Object.h"

#include <opencv2/opencv.hpp>

#include <zed/Camera.hpp>
#include <zed/utils/GlobalDefine.hpp>

typedef struct mouseOCVStruct {
    float* data;
    uint32_t step;
    cv::Size _image;
    cv::Size _resize;
    std::string name;
    std::string unit;
} mouseOCV;

mouseOCV mouseStruct;

static void onMouseCallback(int32_t event, int32_t x, int32_t y, int32_t flag, void * param) {
    if (event == CV_EVENT_LBUTTONDOWN) {
        mouseOCVStruct* data = (mouseOCVStruct*) param;

        int y_int = (y * data->_image.height / data->_resize.height);
        int x_int = (x * data->_image.width / data->_resize.width);

        float* ptr_image_num = (float*) ((int8_t*) data->data + y_int * data->step);
        float dist = ptr_image_num[x_int];

        if (isValidMeasure(dist))
            printf("\n%s : %2.2f %s\n", data->name.c_str(), dist, data->unit.c_str());
        else {
            if (dist == TOO_FAR)
                printf("\n%s is too far.\n", data->name.c_str(), dist, data->unit.c_str());
            else if (dist == TOO_CLOSE)
                printf("\n%s is too close.\n", data->name.c_str(), dist, data->unit.c_str());
            else
                printf("\n%s not avaliable\n", data->name.c_str(), dist, data->unit.c_str());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 720;
const int FRAME_HEIGHT = 404;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";

//The following for canny edge detec
Mat dst, detected_edges;
Mat src, src_gray;
int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
const char* window_name = "Edge Map";

void on_trackbar( int, void* )
{//This function gets called whenever a
	// trackbar position is changed

}

string intToString(int number){

	std::stringstream ss;
	ss << number;
	return ss.str();
}

void createTrackbars(){
	//create window for trackbars
	namedWindow(trackbarWindowName,0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH),
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->
	createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
	createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
	createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
	createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
	createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
	createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );
}

void drawObject(vector<Object> theObjects,Mat &frame, Mat &temp, vector< vector<Point> > contours, vector<Vec4i> hierarchy){

	for(int i =0; i<theObjects.size(); i++){
	cv::drawContours(frame,contours,i,theObjects.at(i).getColor(),3,8,hierarchy);
	cv::circle(frame,cv::Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()),5,theObjects.at(i).getColor());
	cv::putText(frame,intToString(theObjects.at(i).getXPos())+ " , " + intToString(theObjects.at(i).getYPos()),cv::Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()+20),1,1,theObjects.at(i).getColor());
	cv::putText(frame,theObjects.at(i).getType(),cv::Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()-20),1,2,theObjects.at(i).getColor());
	}
}

void drawObject(vector<Object> theObjects,Mat &frame){

	for(int i =0; i<theObjects.size(); i++){

	cv::circle(frame,cv::Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()),10,cv::Scalar(0,0,255));
	cv::putText(frame,intToString(theObjects.at(i).getXPos())+ " , " + intToString(theObjects.at(i).getYPos()),cv::Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()+20),1,1,Scalar(0,255,0));
	cv::putText(frame,theObjects.at(i).getType(),cv::Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()-30),1,2,theObjects.at(i).getColor());
	}
}

void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);

	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
}
void trackFilteredObject(Mat threshold,Mat HSV, Mat &cameraFeed)
{
	vector <Object> objects;
	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS)
		{
			for (int index = 0; index >= 0; index = hierarchy[index][0])
			{
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;
				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA)
				{
					Object object;

					object.setXPos(moment.m10/area);
					object.setYPos(moment.m01/area);

					objects.push_back(object);

					objectFound = true;

				}
				else objectFound = false;
			}
			//let user know you found an object
			if(objectFound ==true)
			{
				//draw object location on screen
				drawObject(objects,cameraFeed);
			}
		}
		else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}

void trackFilteredObject(Object theObject,Mat threshold,Mat HSV, Mat &cameraFeed){

	vector <Object> objects;
	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

		//if the area is less than 20 px by 20px then it is probably just noise
		//if the area is the same as the 3/2 of the image size, probably just a bad filter
		//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA){

					Object object;

					object.setXPos(moment.m10/area);
					object.setYPos(moment.m01/area);
					object.setType(theObject.getType());
					object.setColor(theObject.getColor());

					objects.push_back(object);

					objectFound = true;

				}else objectFound = false;
			}
			//let user know you found an object
			if(objectFound ==true){
				//draw object location on screen
				drawObject(objects,cameraFeed,temp,contours,hierarchy);}

		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {

    
if (argc > 3) {
        std::cout << "Only the path of a SVO or a InitParams file can be passed in arg." << std::endl;
        return -1;
    }

    // Quick check input arguments - REMOVED


    sl::zed::Camera* zed;

// Live Mode
        zed = new sl::zed::Camera(sl::zed::HD720);

		
	// SVO playback mode - REMOVED

    // Define a struct of parameters for the initialization
    sl::zed::InitParams params;

 // If A parameters file was given in argument, we load it - REMOVED
  

    // Enables verbosity in the console
    params.verbose = true;


    sl::zed::ERRCODE err = zed->init(params);
    std::cout << "Error code : " << sl::zed::errcode2str(err) << std::endl;
    if (err != sl::zed::SUCCESS) {
        // Exit if an error occurred
        delete zed;
        return 1;
    }

    // Save the initialization parameters
    // The file can be used later in any zed based application - REMOVED

    char key = ' ';
    int viewID = 0;
    int confidenceThres = 100;

    bool displayDisp = true;
    bool displayConfidenceMap = false;

    int width = zed->getImageSize().width; // Gets width and height of the feed
    int height = zed->getImageSize().height;

    cv::Mat disp(height, width, CV_8UC4); // Display can have width,height and a 4 channel matrix matrix of colours ( from 0-255)
    cv::Mat anaglyph(height, width, CV_8UC4);
    cv::Mat confidencemap(height, width, CV_8UC4);

    cv::Size displaySize(720, 404);
    cv::Mat dispDisplay(displaySize, CV_8UC4);
    cv::Mat anaglyphDisplay(displaySize, CV_8UC4);
    cv::Mat confidencemapDisplay(displaySize, CV_8UC4);

    sl::zed::SENSING_MODE dm_type = sl::zed::STANDARD;

    // Mouse callback initialization
    sl::zed::Mat depth;
    zed->grab(dm_type);
    depth = zed->retrieveMeasure(sl::zed::MEASURE::DEPTH); // Get the pointer
    
	// Set the structure
    mouseStruct._image = cv::Size(width, height);
    mouseStruct._resize = displaySize;
    mouseStruct.data = (float*) depth.data;
    mouseStruct.step = depth.step;
    mouseStruct.name = "DEPTH";
    mouseStruct.unit = unit2str(params.unit);

    // The depth is limited to 20 METERS, as defined in zed::init()
    zed->setDepthClampValue(10000);

    // Create OpenCV Windows
    // NOTE: You may encounter an issue with OpenGL support, to solve it either
    // 	use the default rendering by removing ' | cv::WINDOW_OPENGL' from the flags
    //	or recompile OpenCV with OpenGL support (you may also need the gtk OpenGL Extension
    //	on Linux, provided by the packages libgtkglext1 libgtkglext1-dev)
    cv::namedWindow(mouseStruct.name, cv::WINDOW_AUTOSIZE );
    cv::setMouseCallback(mouseStruct.name, onMouseCallback, (void*) &mouseStruct);
    cv::namedWindow("VIEW", cv::WINDOW_AUTOSIZE );

    std::cout << "Press 'q' to exit" << std::endl;

    // Jetson only. Execute the calling thread on core 2
    sl::zed::Camera::sticktoCPUCore(2);

    sl::zed::ZED_SELF_CALIBRATION_STATUS old_self_calibration_status = sl::zed::SELF_CALIBRATION_NOT_CALLED;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//if we would like to calibrate our filter values, set to true.
	bool calibrationMode = true;

	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	Mat threshold;
	Mat HSV;

	if(calibrationMode){
		//create slider bars for HSV filtering
		createTrackbars();
	}
	//video capture object to acquire webcam feed
	//open capture object at location zero (default location for webcam)
	//set height and width of capture frame
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	//----REMOVED CV::CAPTURE AND JUST USED THE ZED GETVIEW FUNCTION USED EARLIER
	waitKey(1000);
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
    // Loop until 'q' is pressed  ----- COMBINED BOTH CV AND ZED LOOPS INTO ONE
    while (key != 'q') {
        // Disparity Map filtering
        zed->setConfidenceThreshold(confidenceThres);

        // Get frames and launch the computation
        bool res = zed->grab(dm_type);

        if (!res) {
            if (old_self_calibration_status != zed->getSelfCalibrationStatus()) {
                std::cout << "Self Calibration Status : " << sl::zed::statuscode2str(zed->getSelfCalibrationStatus()) << std::endl;
                old_self_calibration_status = zed->getSelfCalibrationStatus();
            }

            depth = zed->retrieveMeasure(sl::zed::MEASURE::DEPTH); // Get the pointer

            // The following is the best way to retrieve a disparity map / image / confidence map in OpenCV Mat.
            // If the buffer is not duplicated, it will be replaced by a next retrieve (retrieveImage, normalizeMeasure, getView...)
            // Disparity, depth, confidence are 32F buffer by default and 8UC4 buffer in normalized format (displayable grayscale)


            // -- The next part is about displaying the data --------------------------------------DISPLAY

            // Normalize the disparity / depth map in order to use the full color range of gray level image
			//ADDED _GPU TO ALL RETRIEVES, MAKES IT USE THE GPU BUFFER? IF IT DOES NOT MAKE IT FASTER, 
			//JUST REMOVE ALL INSTANCES OF _GPU
            
			if (displayDisp)
                slMat2cvMat(zed->normalizeMeasure_gpu(sl::zed::MEASURE::DISPARITY)).copyTo(disp);
            else
                slMat2cvMat(zed->normalizeMeasure_gpu(sl::zed::MEASURE::DEPTH)).copyTo(disp);

            // To get the depth at a given position, click on the disparity / depth map image
            cv::resize(disp, dispDisplay, displaySize);
            imshow(mouseStruct.name, dispDisplay);

            if (displayConfidenceMap) {
                slMat2cvMat(zed->normalizeMeasure_gpu(sl::zed::MEASURE::CONFIDENCE)).copyTo(confidencemap);
                cv::resize(confidencemap, confidencemapDisplay, displaySize);
                imshow("confidence", confidencemapDisplay);
            }

			 // 'viewID' can be 'SIDE mode' or 'VIEW mode' - THIS DECIDES WHICH TYPE OF REGULAR VIEW TO USE 
			 //BASED ON CASE STATEMENTS BELOW , CALLS THE WINDOW AS ANAGLYPH BUT MAY NOT BE
-            
			if (viewID >= sl::zed::LEFT && viewID < sl::zed::LAST_SIDE)
-               //Not sure difference between retrieveImage and getView 
				slMat2cvMat(zed->retrieveImage_gpu(static_cast<sl::zed::SIDE> (viewID))).copyTo(anaglyph);
-            else
				//Zed->getView gets the image, copies to anaglyph matrix then converts matrix to cv::Mat to be used in imshow
-                slMat2cvMat(zed->getView_gpu(static_cast<sl::zed::VIEW_MODE> (viewID - (int) sl::zed::LAST_SIDE))).copyTo(anaglyph);
-
-            cv::resize(anaglyph, anaglyphDisplay, displaySize);
-            imshow("VIEW", anaglyphDisplay);

            key = cv::waitKey(5);
//--------------------------------------------------------------------------------------------------DISPLAY
            // Keyboard shortcuts
            switch (key) {
                case 'b':
                    if (confidenceThres >= 10)
                        confidenceThres -= 10;
                    break;
                case 'n':
                    if (confidenceThres <= 90)
                        confidenceThres += 10;
                    break;
                    // From 'SIDE' enum
                case '0': // Left
                    viewID = 0;
                    std::cout << "Current View switched to Left (rectified/aligned)" << std::endl;
                    break;
                case '1': // Right
                    viewID = 1;
                    std::cout << "Current View switched to Right (rectified/aligned)" << std::endl;
                    break;
                    // From 'VIEW' enum
                case '2': // Side by Side
                    viewID = 10;
                    std::cout << "Current View switched to Side by Side mode" << std::endl;
                    break;
					// Overlay -REMOVED
                    // Difference -REMOVED
                    // Anaglyph -REMOVED
            
                case 'c':
                    displayConfidenceMap = !displayConfidenceMap;
                    break;
                case 's':
                    dm_type = (dm_type == sl::zed::SENSING_MODE::STANDARD) ? sl::zed::SENSING_MODE::FILL : sl::zed::SENSING_MODE::STANDARD;
                    std::cout << "SENSING_MODE " << sensing_mode2str(dm_type) << std::endl;
                    break;
                case 'd':
                    displayDisp = !displayDisp;
                    break;
            }
        } else key = cv::waitKey(5);
 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
		//store image to matrix  -- CHANGED TO MAKE THE ZED GETVIEW AND CONVERT TO CV::MAT THEN COPY INTO CAMERAFEED
		slMat2cvMat(zed->getView_gpu(static_cast<sl::zed::VIEW_MODE> (viewID - (int) sl::zed::LAST_SIDE))).copyTo(cameraFeed);

		src = cameraFeed;

  		if( !src.data )
  		{ return -1; }

		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);

		if(calibrationMode==true){

		//need to find the appropriate color range values
		// calibrationMode must be false

		//if in calibration mode, we track objects based on the HSV slider values.
			cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
			inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
			morphOps(threshold);
			imshow(windowName2,threshold);

		//the folowing for canny edge detec
			/// Create a matrix of the same type and size as src (for dst)
	  		dst.create( src.size(), src.type() );
	  		/// Convert the image to grayscale
	  		cvtColor( src, src_gray, CV_BGR2GRAY );
	  		/// Create a window
	  		namedWindow( window_name, CV_WINDOW_AUTOSIZE );
	  		/// Create a Trackbar for user to enter threshold
	  		createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold);
	  		/// Show the image
			trackFilteredObject(threshold,HSV,cameraFeed);
		}
		else{
			//create some temp fruit objects so that
			//we can use their member functions/information
			Object blue("blue"), yellow("yellow"), red("red"), green("green");

			//first find blue objects
			cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
			inRange(HSV,blue.getHSVmin(),blue.getHSVmax(),threshold);
			morphOps(threshold);
			trackFilteredObject(blue,threshold,HSV,cameraFeed);
			//then yellows
			cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
			inRange(HSV,yellow.getHSVmin(),yellow.getHSVmax(),threshold);
			morphOps(threshold);
			trackFilteredObject(yellow,threshold,HSV,cameraFeed);
			//then reds
			cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
			inRange(HSV,red.getHSVmin(),red.getHSVmax(),threshold);
			morphOps(threshold);
			trackFilteredObject(red,threshold,HSV,cameraFeed);
			//then greens
			cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
			inRange(HSV,green.getHSVmin(),green.getHSVmax(),threshold);
			morphOps(threshold);
			trackFilteredObject(green,threshold,HSV,cameraFeed);

		}
		//show frames
		//imshow(windowName2,threshold);

		imshow(windowName,cameraFeed);
		//imshow(windowName1,HSV);

		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}


////////////////////////////////////////////////////////////////////////////////////////////////////


    delete zed;
    return 0;
}
