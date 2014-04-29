#include <stdio.h>
#include <stdint.h>
#include <dc1394/dc1394.h>
#include <stdlib.h>
#include <inttypes.h>
#include <iostream>
#include "opencv2/opencv.hpp"
#include <time.h>

using namespace cv;

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

int main(int argc, char *argv[])
{
    FILE* imagefile;
    dc1394camera_t *camera, *camera1;
    unsigned int width, height;
    dc1394video_frame_t *frame=NULL;
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
    namedWindow("edges",1);
    namedWindow("edges1",1);
    namedWindow("edges2",1);

    dc1394_get_image_size_from_video_mode(camera, DC1394_VIDEO_MODE_800x600_MONO8, &width, &height);

    cv::Mat all( height * 2, width, CV_8UC1);
    cv::Mat half1(all, cv::Rect(0, 0, width, height));
    cv::Mat half(all, cv::Rect( 0, height, width, height));

    for(;;) 
    {

	timespec tb, ta;
	clock_gettime(CLOCK_REALTIME, &tb);

        err=dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
        frame1 = cv::Mat( height, width, CV_8UC1, frame->image);
	err = dc1394_capture_enqueue( camera, frame);
	err=dc1394_capture_dequeue(camera1, DC1394_CAPTURE_POLICY_WAIT, &frame);
        frame2 = cv::Mat( height, width, CV_8UC1, frame->image);
	err = dc1394_capture_enqueue( camera1, frame);

    	frame1.copyTo(half1);
	frame2.copyTo(half);
	clock_gettime(CLOCK_REALTIME, &ta);
	unsigned long elapsed_time = ta.tv_nsec - tb.tv_nsec ;
	std::cout << (double) elapsed_time/1000000 << std::endl;

	imshow("edges2", all);

        if(waitKey(30) >= 0) break;

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
