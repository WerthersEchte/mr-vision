#include <stdio.h>
#include <stdint.h>
#include <dc1394/dc1394.h>
#include <stdlib.h>
#include <inttypes.h>
#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"
#include <time.h>
#include <math.h>
#include <map>
#include "bot.h"

#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>

#include "markerlist.h"
#include "udp_server.h"

using namespace cv;

#define PI 3.14159265

#ifndef _WIN32
#include <unistd.h>
#endif

#define IMAGE_FILE_NAME "image.ppm"

/*-----------------------------------------------------------------------
 *  Releases the cameras and exits
 *-----------------------------------------------------------------------*/
void cleanup_and_exit(dc1394camera_t *camera)
{
    dc1394_video_set_transmission(camera, DC1394_OFF);
    dc1394_capture_stop(camera);
    dc1394_camera_free(camera);
    exit(1);
}

mrvision::MarkerList vMarkerList;

int derDetector( const cv::Mat &in,int &nRotations ){
    Mat vBWMarker = in > 128;
    Mat vPart;
	nRotations = 0;
    int width=vBWMarker.size().width / 5,
    	height=vBWMarker.size().height / 5;
    std::vector<bool> vMarker;
    for( int i = 1; i < 4; i++){
    	for( int j = 1; j < 4; j++){
    		    		vPart = cv::Mat(vBWMarker, cv::Rect( j * width, i * height, width, height));
    		vMarker.push_back( countNonZero(vPart)>width*height/2 );
    	}
    }



    for( mrvision::Marker vTest : vMarkerList.getMarker() ){
		if( vTest.compareTo( vMarker, nRotations ) ){
			return vTest.getId();
		}
    }

    return -1;
}


int main(int argc, char *argv[])
{
	mrvision::UDPServer vServer( 9050 );
	vServer.start();

    FILE* imagefile;
    dc1394camera_t *camera, *camera1;
    unsigned int width, height;
    dc1394video_frame_t *frame=NULL, *frame22=NULL;
    //dc1394featureset_t features;
    dc1394_t * d;
    dc1394camera_list_t * list;
    dc1394error_t err;

    d = dc1394_new ();
    if (!d)
        return 1;
    err=dc1394_camera_enumerate (d, &list);
    DC1394_ERR_RTN(err,"Failed to enumerate cameras");

    if (list->num == 0) {
        dc1394_log_error("No cameras found");
        return 1;
    }

    camera = dc1394_camera_new (d, list->ids[1].guid);
    if (!camera) {
        dc1394_log_error("Failed to initialize camera with guid %llx", list->ids[0].guid);
        return 1;
    }

    std::cout << "Using camera with GUID " << camera->guid << std::endl ;

    /*-----------------------------------------------------------------------
     *  setup capture
     *-----------------------------------------------------------------------*/
    err=dc1394_video_set_iso_speed(camera, DC1394_ISO_SPEED_400);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not set iso speed");

    err=dc1394_video_set_mode(camera, DC1394_VIDEO_MODE_800x600_MONO8);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not set video mode\n");

    err=dc1394_video_set_framerate(camera, DC1394_FRAMERATE_30);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not set framerate\n");

    err=dc1394_capture_setup(camera,4, DC1394_CAPTURE_FLAGS_DEFAULT);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not setup camera-\nmake sure that the video mode and framerate are\nsupported by your camera\n");

    err=dc1394_feature_set_mode(camera, DC1394_FEATURE_WHITE_BALANCE, DC1394_FEATURE_MODE_AUTO);
    std::cout << "WB: " << err << std::endl;
    err=dc1394_feature_set_mode(camera, DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_AUTO);
    std::cout << "WB: " << err << std::endl;
    err=dc1394_feature_set_mode(camera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_AUTO);
    std::cout << "WB: " << err << std::endl;
    /*-----------------------------------------------------------------------
     *  have the camera start sending us data
     *-----------------------------------------------------------------------*/
    
    err=dc1394_video_set_transmission(camera, DC1394_ON);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not start camera iso transmission\n");

    camera1 = dc1394_camera_new (d, list->ids[0].guid);
    if (!camera1) {
        dc1394_log_error("Failed to initialize camera with guid %llx", list->ids[0].guid);
        return 1;
    }
    dc1394_camera_free_list (list);

    std::cout << "Using camera with GUID " << camera1->guid << std::endl ;

    /*-----------------------------------------------------------------------
     *  setup capture
     *-----------------------------------------------------------------------*/
    err=dc1394_video_set_iso_speed(camera1, DC1394_ISO_SPEED_400);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera1),"Could not set iso speed");

    err=dc1394_video_set_mode(camera1, DC1394_VIDEO_MODE_800x600_MONO8);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera1),"Could not set video mode\n");

    err=dc1394_video_set_framerate(camera1, DC1394_FRAMERATE_30);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera1),"Could not set framerate\n");

    err=dc1394_capture_setup(camera1,4, DC1394_CAPTURE_FLAGS_DEFAULT);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera1),"Could not setup camera-\nmake sure that the video mode and framerate are\nsupported by your camera\n");

    err=dc1394_feature_set_mode(camera1, DC1394_FEATURE_WHITE_BALANCE, DC1394_FEATURE_MODE_AUTO);
    std::cout << "WB: " << err << std::endl;
    err=dc1394_feature_set_mode(camera1, DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_AUTO);
    std::cout << "WB: " << err << std::endl;
    err=dc1394_feature_set_mode(camera1, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_AUTO);
    std::cout << "WB: " << err << std::endl;
    /*-----------------------------------------------------------------------
     *  have the camera start sending us data
     *-----------------------------------------------------------------------*/
    err=dc1394_video_set_transmission(camera1, DC1394_ON);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera1),"Could not start camera iso transmission\n");

    /*-----------------------------------------------------------------------
     *  capture one frame
//     *-----------------------------------------------------------------------*/
//    err=dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
//    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not capture a frame\n");

    Mat edges, edges1, frame1, frame2, frame3;
    namedWindow("CameraRechts",1);

    dc1394_get_image_size_from_video_mode(camera, DC1394_VIDEO_MODE_800x600_MONO8, &width, &height);

    cv::Mat all( height * 2, width * 2, CV_8UC1);
    //cv::Mat half1(all, cv::Rect(0, 0, width, height));
    //cv::Mat half(all, cv::Rect( 0, height, width, height));
    
    int k = 11,it, offset=16, sosize=706;
    std::cout << "farts" << std::endl;
    aruco::CameraParameters CamRechtsParam, CamLinksParam;
    aruco::MarkerDetector MDetector;
    MDetector.setMinMaxSize(0.000000001);
    MDetector.setMakerDetectorFunction(&derDetector);
    vector<aruco::Marker> Markers;
    std::cout << MDetector.getWarpSize() << std::endl;
    CamRechtsParam.readFromXMLFile(argv[1]);
    CamLinksParam.readFromXMLFile(argv[2]);
    float MarkerSize=atof(argv[3]);
    std::cout << MarkerSize << std::endl;

    std::map<int, mrvision::Bot> vBots;

    for(;;) 
    {
    		vBots.clear();
            err=dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
            frame1 = cv::Mat( height, width, CV_8UC1, frame->image);
            err = dc1394_capture_enqueue( camera, frame);
            err=dc1394_capture_dequeue(camera1, DC1394_CAPTURE_POLICY_WAIT, &frame22);
            frame2 = cv::Mat( height, width, CV_8UC1, frame22->image);

            all = frame1 > 85;

            CamRechtsParam.resize( all.size() );
            MDetector.detect(all,Markers,CamRechtsParam,MarkerSize);

            for (unsigned int i=0;i<Markers.size();i++) {
            	Point2f vPoint = Markers[i][1] - Markers[i][2];

                Markers[i].draw(frame1,Scalar(0,0,255),2, true);
                cv::line( frame1,
                		Markers[i].getCenter(),
                		Markers[i].getCenter() + vPoint,
                				Scalar(255,255,255), 2,CV_AA);
            }

            for( aruco::Marker vMarker : Markers )
            {
            	Point2f vPoint = vMarker[1] - vMarker[2];
            	vBots[vMarker.id] = mrvision::Bot(
            			vMarker.id,
            			( vMarker.getCenter().y - 50 ) / 850,
            			( vMarker.getCenter().x - 10 - offset ) / sosize,
            			atan2( vPoint.x, vPoint.y) * 180 / PI);
            }

    	    imshow("CameraRechts", frame1);

    	    all = frame2 > 85;

            CamLinksParam.resize( all.size() );
            MDetector.detect(all,Markers,CamLinksParam,MarkerSize);

            for (unsigned int i=0;i<Markers.size();i++) {
            	Point2f vPoint = Markers[i][1] - Markers[i][2];

                Markers[i].draw(frame2,Scalar(0,0,255),2, true);
                cv::line( frame2,
                		Markers[i].getCenter(),
                		Markers[i].getCenter() + vPoint,
                				Scalar(255,255,255), 2,CV_AA);
            }

            for( aruco::Marker vMarker : Markers )
            {
            	Point2f vPoint = vMarker[1] - vMarker[2];
            	vBots[vMarker.id] = mrvision::Bot(
            			vMarker.id,
            			( vMarker.getCenter().y + 321 ) / 850,
            			( vMarker.getCenter().x - offset ) / sosize,
            			atan2( vPoint.x, vPoint.y) * 180 / PI);
            }

            vServer.send_Data( vBots );
    	    imshow("CameraLinks", frame2);

	    err = dc1394_capture_enqueue( camera, frame);
	    err = dc1394_capture_enqueue( camera1, frame22);

	    it = waitKey(1);
	    if( it == '8' ){

	    	std::cout << ++offset << std::endl;

	    } else if( it == '2' ){

	    	std::cout << --offset << std::endl;

	    } else if( it == '4' ){

	    	std::cout << --sosize << std::endl;

	    } else if( it == '6' ){

	    	std::cout << ++sosize << std::endl;

	    }


    }

    /*-----------------------------------------------------------------------
     *  stop data transmission
     *-----------------------------------------------------------------------*/
    err=dc1394_video_set_transmission(camera,DC1394_OFF);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not stop the camera?\n");

    /*-----------------------------------------------------------------------
     *  save image as 'Image.pgm'
     *-----------------------------------------------------------------------*/


    /*-----------------------------------------------------------------------
     *  close camera
     *-----------------------------------------------------------------------*/
    dc1394_video_set_transmission(camera, DC1394_OFF);
    dc1394_capture_stop(camera);
    dc1394_camera_free(camera);
    dc1394_free (d);
    return 0;
}
//#include <QApplication>
//#include "gui/mainGUI.h"
//
//#include "opencv2/opencv.hpp"
//
//using namespace cv;
//
//int main(int, char**)
//{
////    VideoCapture cap(0); // open the default camera
////    VideoCapture cap2(1); // open the default camera
////    if( !cap.isOpened() && !cap2.isOpened() )  // check if we succeeded
////        return -1;
////
////    Mat edges, edges2;
////    namedWindow("edges",1);
////    namedWindow("edges2",1);
////    for(;;)
////    {
////        Mat frame, frame2;
////        cap >> frame; // get a new frame from camera
////        cap2 >> frame2;
////        imshow("edges", frame);
////        imshow("edges2", frame2);
////        if(waitKey(30) >= 0) break;
////    }
////    // the camera will be deinitialized automatically in VideoCapture destructor
////    return 0;
//
////    cvDestroyWindow("result");
////
////	QApplication app(argc, argv);
////	mainGUI mainWindow;
////	mainWindow.show();
////	return app.exec();
//
//}
