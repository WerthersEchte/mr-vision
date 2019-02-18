#pragma once
#ifndef _detectorneural_h
#define _detectorneural_h

#include "src/core/detector.h"
#include "src/core/detectedmarker.h"
#include "src/core/markerlist.h"
#include "src/core/bot.h"

#include <string>
#include <vector>

#include "opencv2/opencv.hpp"

namespace mrvision {

	class DetectorNeural : public Detector {

		Q_OBJECT

		int mIteration;

		int mThreshold = 220;
		int mMinSize = 8, mMaxSize = 15;
		int mDetectionAreaMaxX, mDetectionAreaMaxY, mDetectionAreaMinX, mDetectionAreaMinY;
		bool isRunning, mStatus;

		cv::Mat mBinaryImage;
		std::vector<cv::Point> mPoints, mArea, mRealPoints;
		int mCounterI = 0, mCounterJ = 0, mCounterK = 0, mCounterL = 0;

		int mMaxRangeBetweenPoints = 360, mMinRangeBetweenPoints = 25; //quadratisch
		std::vector<std::vector<int> > mDistanceBetweenPoints;
		std::vector<std::vector<cv::Point> > mFoundCombinations;

		int mPointA = 0, mPointB = 0, mPointC = 0, mPointD = 0;
		int mDistanceAB = 0, mDistanceBC = 0, mDistanceCD = 0, mDistanceAD = 0, mDistanceAC = 0, mDistanceBD = 0;
		bool mDistanceABtest = false, mDistanceBCtest = false, mDistanceCDtest = false, mDistanceADtest = false, mDistanceACtest = false, mDistanceBDtest = false;
	public:
		// Read CameraFile - maybe move to Camera
		cv::Mat mCameraMatrix;
		cv::Mat mDistortionMatrix;
		cv::Size mCameraSize;

	public:
		DetectorNeural(float aSizeOfMarker = 0.025, int aSizeOfImageX = 800, int aSizeOfImageY = 600);
		virtual ~DetectorNeural();
		virtual void setStatus(bool aStatus);

		virtual void setCameraFile(QString aCameraFileName);
		virtual void setThreshold(int aThreshold);
		virtual int getThreshold();
		void setDetectionArea(int aMinX, int aMaxX, int aMinY, int aMaxY);

	public slots:
		virtual void detectMarkers(const cv::Mat& aImage);

	private:

		void detectingMarkerInImage(const cv::Mat& aImage);
		bool identifyMarker(DetectedMarker& aNotYetFoundMarker);

	};

}

#endif // _detectorneural_h
