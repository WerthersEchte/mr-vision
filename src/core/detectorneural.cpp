#include "src/core/detectorneural.h"

#include "src/core/detectedmarker.h"

#include "src/core/neural.h"
#include <c_api.h>

#include <iostream>
#include <fstream>

#include <QtConcurrent/QtConcurrent>
#include "opencv2/opencv.hpp"

namespace mrvision {

	model_t modelvar;
	bool model_loaded = false;
	model_t getModel() {
		if (!model_loaded) {
			//https://gist.github.com/asimshankar/7c9f8a9b04323e93bb217109da8c7ad2
			if (!ModelCreate(&modelvar, "./frozen_model.pb")) {
				std::cout << "Model loading failed" << std::endl;
			}
			else {
				model_loaded = true;
			}
		}
		return modelvar;
	}

	int handleError(int status, const char* func_name,
		const char* err_msg, const char* file_name,
		int line, void* userdata)
	{
		//Do nothing -- will suppress console output
		return 0;   //Return value is not used
	}

	DetectorNeural::DetectorNeural(float aSizeOfMarker, int aSizeOfImageX, int aSizeOfImageY) :
		mIteration(0),
		mThreshold(150),
		mMinSize(8),
		mMaxSize(15),
		mDetectionAreaMaxX(aSizeOfImageX), mDetectionAreaMaxY(aSizeOfImageY), mDetectionAreaMinX(0), mDetectionAreaMinY(0),
		isRunning(false),
		mStatus(false),
		mBinaryImage(),
		mPoints(), mArea(), mRealPoints(),
		mCounterI(0), mCounterJ(0), mCounterK(0), mCounterL(0),
		mMaxRangeBetweenPoints(360), mMinRangeBetweenPoints(25),
		mDistanceBetweenPoints(),
		mFoundCombinations(),
		mPointA(0), mPointB(0), mPointC(0), mPointD(0),
		mDistanceAB(0), mDistanceBC(0), mDistanceCD(0), mDistanceAD(0), mDistanceAC(0), mDistanceBD(0),
		mDistanceABtest(false), mDistanceBCtest(false), mDistanceCDtest(false), mDistanceADtest(false), mDistanceACtest(false), mDistanceBDtest(false),
		mCameraMatrix(), mDistortionMatrix(), mCameraSize()
	{

		qRegisterMetaType< cv::Mat >("cv::Mat");
		cv::redirectError(handleError);

		//connect( &mDetector, SIGNAL( showMarkerAndImage( cv::Mat, QList<bool> ) ), this, SLOT( blahMarkerAndImage( cv::Mat, QList<bool> ) ) );

	}

	DetectorNeural::~DetectorNeural() {}

	static double gImageCounter = 0, gFoundRectCounter = 0, gFoundMarkerCounter = 0;

	void DetectorNeural::detectingMarkerInImage(const cv::Mat& aImage) {

		std::vector<DetectedMarker> vFoundMarkers;

		mBinaryImage = aImage > mThreshold;

		mRealPoints.clear();
		mPoints.clear();

		try {
			cv::findNonZero(
				mBinaryImage(
					cv::Rect(
						cv::Point(std::max(0, mDetectionAreaMinX), std::max(0, mDetectionAreaMinY)),
						cv::Point(std::min(mBinaryImage.size().width, mDetectionAreaMaxX), std::min(mBinaryImage.size().height, mDetectionAreaMaxY))
					)
				),
				mPoints);
		}
		catch (cv::Exception vNoPointsDetected) {
			std::cout << "No Points detected!\n" << std::endl;
			isRunning = false;
			return;
		}



		while (!mPoints.empty()) {
			mCounterI = 0;
			mArea.clear();

			mArea.push_back(mPoints.back());
			mPoints.pop_back();

			while (mCounterI < mArea.size()) {

				for (mCounterJ = mPoints.size() - 1; mCounterJ >= 0; --mCounterJ) {

					if (mArea[mCounterI].y == mPoints[mCounterJ].y && mArea[mCounterI].x + 1 == mPoints[mCounterJ].x ||
						mArea[mCounterI].y == mPoints[mCounterJ].y && mArea[mCounterI].x - 1 == mPoints[mCounterJ].x ||
						mArea[mCounterI].y + 1 == mPoints[mCounterJ].y && mArea[mCounterI].x == mPoints[mCounterJ].x ||
						mArea[mCounterI].y - 1 == mPoints[mCounterJ].y && mArea[mCounterI].x == mPoints[mCounterJ].x /*||
						mArea[mCounterI].y + 1 == mPoints[mCounterJ].y && mArea[mCounterI].x + 1 == mPoints[mCounterJ].x ||
						mArea[mCounterI].y - 1 == mPoints[mCounterJ].y && mArea[mCounterI].x - 1 == mPoints[mCounterJ].x ||
						mArea[mCounterI].y - 1 == mPoints[mCounterJ].y && mArea[mCounterI].x + 1 == mPoints[mCounterJ].x ||
						mArea[mCounterI].y + 1 == mPoints[mCounterJ].y && mArea[mCounterI].x - 1 == mPoints[mCounterJ].x */) {

						mArea.push_back(mPoints[mCounterJ]);
						mPoints.erase(mPoints.begin() + mCounterJ);

					}

				}
				++mCounterI;
			}
			//Calculate middle point of blob
			mRealPoints.push_back(cv::Point());
			for (cv::Point vPoint : mArea) {

				mRealPoints.back() += vPoint;

			}
			mRealPoints.back().x /= mArea.size();
			mRealPoints.back().y /= mArea.size();
		}

		//Bei zu vielen Points gib auf, Kamera falsch eingestellt oder zu viel Müll auf dem Tisch
		if (mRealPoints.size() > 400) {
			isRunning = false;
			return;

		}

		cv::Mat out3;
		cv::cvtColor(aImage, out3, cv::COLOR_GRAY2BGR);

		for (auto p : mRealPoints) {
			cv::circle(out3, p, 1, cv::Scalar(0, 0, 255), 2);
		}


		for (int vI = 0; vI < mRealPoints.size(); ++vI) {
			mRealPoints[vI].x += mDetectionAreaMinX;
			mRealPoints[vI].y += mDetectionAreaMinY;
		}

		mFoundCombinations.clear();
		if (mDistanceBetweenPoints.size() < mRealPoints.size()) {
			mDistanceBetweenPoints.resize(mRealPoints.size());
			for (mCounterI = 0; mCounterI < mRealPoints.size(); ++mCounterI) {
				mDistanceBetweenPoints[mCounterI].resize(mRealPoints.size());
			}
		}

		for (mCounterI = 0; mCounterI < mRealPoints.size(); ++mCounterI) {
			for (mCounterJ = 0; mCounterJ < mRealPoints.size(); ++mCounterJ) {
				mDistanceBetweenPoints[mCounterI][mCounterJ] = std::pow(mRealPoints[mCounterI].x - mRealPoints[mCounterJ].x, 2) + std::pow(mRealPoints[mCounterI].y - mRealPoints[mCounterJ].y, 2);
				mDistanceBetweenPoints[mCounterJ][mCounterI] = mDistanceBetweenPoints[mCounterI][mCounterJ];
			}

		}


		std::stringstream strstrm;

		auto almostEquals = [delta = 0.25](double a, double b) {return std::abs(a - b) < (std::min(a, b)); };
		auto inRange = [min = 11.0, max = 17.0](double val){return (val >= min * min && val <= max * max); };
		int ctr = 0;

		for (int a = 0; a < mRealPoints.size(); a++) {
			for (int b = 0; b < mRealPoints.size(); b++) {
				if (a == b) continue;
				double ab = mDistanceBetweenPoints[a][b];
				if (!inRange(ab)) continue;
				for (int c = 0; c < mRealPoints.size(); c++) {
					if (a == c || b == c) continue;
					double bc = mDistanceBetweenPoints[b][c];
					if (!inRange(bc)) continue;
					double ac = mDistanceBetweenPoints[a][c];
					ctr++;
					double abbc = ab + bc;

					strstrm << ctr << "|   " << "ab: " << ab << "  bc: " << bc << "  ac: " << ac << "  abbc: " << abbc << std::endl;

					if (almostEquals(ab, bc) && almostEquals(abbc, ac)) {

						//std::cout << ctr << "|   " << "ab: " << ab << "  bc: " << bc << "  ac: " << ac << "  abbc: " << abbc << " FOUND" << std::endl;
						cv::Point d;
						if (mRealPoints[c].x == mRealPoints[a].x) {
							d = cv::Point(mRealPoints[a].x + (mRealPoints[a].x - mRealPoints[b].x), mRealPoints[a].y);
						}
						else {
							double m = ((double)(mRealPoints[c].y - mRealPoints[a].y)) / (mRealPoints[c].x - mRealPoints[a].x);
							double m_p = -1 / m;
							double b_ac = (double)mRealPoints[a].y - (m * mRealPoints[a].x);
							double b_p = (double)mRealPoints[b].y - (m_p * mRealPoints[b].x);

							double x = (b_p - b_ac) / (m - m_p);

							double d_x = x + (x - mRealPoints[b].x);
							double d_y = d_x * m_p + b_p;
							d = cv::Point(d_x, d_y);
						}
						std::vector<cv::Point> vCombination;
						vCombination.push_back(mRealPoints[a]);
						vCombination.push_back(mRealPoints[b]);
						vCombination.push_back(mRealPoints[c]);
						vCombination.push_back(d);
						//std::cout << "Points: " << mRealPoints[a] << " - " << mRealPoints[b] << " - " << mRealPoints[c] << " - " << d << std::endl;
						mFoundCombinations.push_back(vCombination);
					}
				}
			}
		}

		if (mFoundCombinations.size() == 0) {
			//std::cout << strstrm.str();
		}

		cv::Mat vRegionOfInterest, vRegionOfInterestImage, vMarkerImage, vRotationMatrix;

		cv::Mat outputImage;
		cv::cvtColor(aImage, outputImage, cv::COLOR_GRAY2BGR);


		std::vector<cv::Point> vPotentialMarker;
		cv::RotatedRect vRotationBox;
		bool markerFound = false;
		for (std::vector<cv::Point> vMarker : mFoundCombinations) {

			try {

				vPotentialMarker.clear();
				vPotentialMarker.push_back(vMarker[0]);
				vPotentialMarker.push_back(vMarker[1]);
				vPotentialMarker.push_back(vMarker[2]);
				vPotentialMarker.push_back(vMarker[3]);

				vRotationBox = cv::minAreaRect(cv::Mat(vPotentialMarker));
				vRegionOfInterest = mBinaryImage(vRotationBox.boundingRect());
				vRotationMatrix = cv::getRotationMatrix2D(cv::Point(vRegionOfInterest.size())*0.5, vRotationBox.angle, 1);

				cv::warpAffine(vRegionOfInterest, vRegionOfInterestImage, vRotationMatrix, vRegionOfInterest.size());
				cv::resize(vRegionOfInterestImage, vMarkerImage, cv::Size(), 1, 1);

				if (vMarkerImage.data != NULL && !vMarkerImage.empty()) {
					//emit showPotentialMarker(vMarkerImage, mIteration);
				}

				DetectedMarker vUnDetectedMarker(vMarkerImage, vRotationBox.angle, vRotationBox.center);

				cv::Point2f vertices[4];
				vRotationBox.points(vertices);
				for (int i = 0; i < 4; i++)
					cv::line(outputImage, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 1);

				cv::imshow("Marker", vMarkerImage);

				if (identifyMarker(vUnDetectedMarker)) {
					std::cout << "Marker identified: " << vUnDetectedMarker.mMarkerId << std::endl;//vFoundMarkers.push_back(vUnDetectedMarker);
					markerFound = true;
					vFoundMarkers.push_back(vUnDetectedMarker);
				}

			}
			catch (cv::Exception dinge) {
				//std::cout << dinge.what() << std::endl;
			}
		}

		auto isNear = [max_distance = 10 * 10](DetectedMarker m1, DetectedMarker m2) {float distance_sq = std::pow(m1.mMarkerPosition.x - m2.mMarkerPosition.x, 2) + std::pow(m1.mMarkerPosition.y - m2.mMarkerPosition.y, 2); return distance_sq <= max_distance; };

		std::vector<DetectedMarker> ActuallyRealFoundMarkers;

		if (vFoundMarkers.size()) {
			while (vFoundMarkers.size()) {
				std::vector<DetectedMarker> cluster;
				cluster.push_back(vFoundMarkers.back());
				vFoundMarkers.pop_back();

				for (int i = 0; i < vFoundMarkers.size(); i++) {
					if (isNear(vFoundMarkers[i], cluster.front())) {
						cluster.push_back(vFoundMarkers[i]);
						vFoundMarkers.erase(vFoundMarkers.begin() + i, vFoundMarkers.begin() + i + 1);
						i--;
					}
				}

				DetectedMarker best_marker = cluster.front();
				for (auto m : cluster) {
					if (m.mMarkerConfidence > best_marker.mMarkerConfidence) {
						best_marker = m;
					}
				}
				ActuallyRealFoundMarkers.push_back(best_marker);
			}

			emit markersDetected(ActuallyRealFoundMarkers);
		}

		mIteration++;

		isRunning = false;
	}

	void DetectorNeural::detectMarkers(const cv::Mat& aImage) {

		if (mStatus && !isRunning) {
			isRunning = true;
			QtConcurrent::run(this, &DetectorNeural::detectingMarkerInImage, aImage);
		}

	}

	static int gCounter = 0;

	bool DetectorNeural::identifyMarker(DetectedMarker& aNotYetFoundMarker) {
		try {
			const auto imsize = 100.0;
			const auto marker_threshold = 0.25;

			cv::Mat vResizedImage(imsize, imsize, aNotYetFoundMarker.mMarkerImage.type());
			cv::resize(aNotYetFoundMarker.mMarkerImage, vResizedImage, vResizedImage.size(), 0, 0, cv::INTER_LINEAR);

			cv::Mat smallImage(32, 32, aNotYetFoundMarker.mMarkerImage.type());
			cv::resize(aNotYetFoundMarker.mMarkerImage, smallImage, smallImage.size(), 0, 0, cv::INTER_LINEAR);

			cv::Mat img2;
			smallImage.convertTo(img2, CV_32FC1);

			float matData[32 * 32];

			for (int y = 0; y < 32; y++) {
				for (int x = 0; x < 32; x++) {
					matData[y * 32 + x] = img2.at<float>(y, x);
				}
			}

			const int64_t dims[4] = { 1, 32, 32 , 1 };
			const size_t nbytes = 32 * 32 * sizeof(float);
			TF_Tensor* t = TF_AllocateTensor(TF_FLOAT, dims, 4, nbytes);
			memcpy(TF_TensorData(t), matData, nbytes);
			model_t model = getModel();
			TF_Output inputs[1] = { model.input };
			TF_Tensor* input_values[1] = { t };
			TF_Output outputs[1] = { model.output };
			TF_Tensor* output_values[1] = { NULL };

			TF_SessionRun(model.session, //Session
				NULL, //Run options
				inputs /*input tensor*/, input_values /*input values*/, 1 /*ninputs*/,
				outputs /*output tensor*/, output_values /*output values*/, 1 /*noutputs*/,
				/* No target operations to run */
				NULL /*target operations*/, 0 /*number of targets*/, NULL /*run metadata*/, model.status /*output status*/);
			TF_DeleteTensor(t);
			if (!Okay(model.status)) return false;
			float* predictions = (float*)malloc(sizeof(float) * 16);
			memcpy(predictions, TF_TensorData(output_values[0]), sizeof(float) * 16);
			TF_DeleteTensor(output_values[0]);

			char* ids[] = { "7", "8", "9", "10" };
			char* dirs[] = { "up  ", "down", "left", "right" };
			float rots[] = { 270.0, 90.0, 180.0, 0.0 };
			bool found = false;
			float rot = aNotYetFoundMarker.mMarkerDirection;
			for (int i = 0; i < 16; ++i) {
				if (predictions[i] > 0.95 && aNotYetFoundMarker.mMarkerConfidence < predictions[i]) {
					found = true;
					aNotYetFoundMarker.mMarkerDirection = (int)((rot + rots[i % 4]) + 360) % 360;
					aNotYetFoundMarker.mMarkerId = (i / 4) + 7;
					aNotYetFoundMarker.mMarkerConfidence = predictions[i];
				}
			}
			free(predictions);
			std::cout << std::endl;
			return found;
		}
		catch (const std::exception& e) {
			std::cout << "ERRÖHR" << e.what() << std::endl;
		}


		return false;
	}

	void DetectorNeural::setStatus(bool aStatus) {

		mStatus = aStatus;

	}
	void DetectorNeural::setThreshold(int aThreshold) {
		mThreshold = aThreshold;
	}
	int DetectorNeural::getThreshold() {
		return mThreshold;
	}


	void DetectorNeural::setCameraFile(QString aCameraFileName) {

		cv::FileStorage fs(aCameraFileName.toStdString(), cv::FileStorage::READ);
		int w = -1, h = -1;
		cv::Mat MCamera, MDist;

		fs["image_width"] >> w;
		fs["image_height"] >> h;
		fs["distortion_coefficients"] >> MDist;
		fs["camera_matrix"] >> MCamera;

		if (MCamera.cols == 0 || MCamera.rows == 0)throw cv::Exception(9007, "File :" + aCameraFileName.toStdString() + " does not contains valid camera matrix", "CameraParameters::readFromXML", __FILE__, __LINE__);
		if (w == -1 || h == 0)throw cv::Exception(9007, "File :" + aCameraFileName.toStdString() + " does not contains valid camera dimensions", "CameraParameters::readFromXML", __FILE__, __LINE__);

		if (MCamera.type() != CV_32FC1) MCamera.convertTo(mCameraMatrix, CV_32FC1);
		else mCameraMatrix = MCamera;

		if (MDist.total() < 4) throw cv::Exception(9007, "File :" + aCameraFileName.toStdString() + " does not contains valid distortion_coefficients", "CameraParameters::readFromXML", __FILE__, __LINE__);
		//convert to 32 and get the 4 first elements only
		cv::Mat mdist32;
		MDist.convertTo(mdist32, CV_32FC1);
		//     Distorsion.create(1,4,CV_32FC1);
		//     for (int i=0;i<4;i++)
		//         Distorsion.ptr<float>(0)[i]=mdist32.ptr<float>(0)[i];

		mDistortionMatrix.create(1, 5, CV_32FC1);
		for (int i = 0; i < 5; i++)
			mDistortionMatrix.ptr<float>(0)[i] = mdist32.ptr<float>(0)[i];
		mCameraSize.width = w;
		mCameraSize.height = h;

	}

	void DetectorNeural::setDetectionArea(int aMinX, int aMaxX, int aMinY, int aMaxY) {

		mDetectionAreaMaxX = aMaxX;
		mDetectionAreaMaxY = aMaxY;
		mDetectionAreaMinX = aMinX;
		mDetectionAreaMinY = aMinY;

	}
}
