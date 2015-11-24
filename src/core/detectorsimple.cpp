#include "src/core/detectorsimple.h"

#include "src/core/detectedmarker.h"

#include <iostream>
#include <fstream>

#include <QtConcurrent/QtConcurrent>
#include "opencv2/opencv.hpp"

namespace mrvision {

DetectorSimple::DetectorSimple( float aSizeOfMarker, int aSizeOfImageX, int aSizeOfImageY ) :
    mThreshold(150),
    mMinSize(8),
    mMaxSize(15),
    mDetectionAreaMaxX(aSizeOfImageX), mDetectionAreaMaxY(aSizeOfImageY), mDetectionAreaMinX(0), mDetectionAreaMinY(0),
    isRunning(false),
    mStatus(false),
    mBinaryImage(),
    mPoints(), mArea(), mRealPoints(),
    mCounterI(0), mCounterJ(0), mCounterK(0), mCounterL(0),
    mMaxRangeBetweenPoints(400), mMinRangeBetweenPoints(64),
    mDistanceBetweenPoints(),
    mFoundCombinations(),
    mPointA(0), mPointB(0), mPointC(0), mPointD(0),
    mDistanceAB(0), mDistanceBC(0), mDistanceCD(0), mDistanceAD(0), mDistanceAC(0), mDistanceBD(0),
    mDistanceABtest(false), mDistanceBCtest(false), mDistanceCDtest(false), mDistanceADtest(false), mDistanceACtest(false), mDistanceBDtest(false),
    mCameraMatrix(), mDistortionMatrix(), mCameraSize()
{

    qRegisterMetaType< cv::Mat >("cv::Mat");

    //connect( &mDetector, SIGNAL( showMarkerAndImage( cv::Mat, QList<bool> ) ), this, SLOT( blahMarkerAndImage( cv::Mat, QList<bool> ) ) );

}

DetectorSimple::~DetectorSimple(){}

void DetectorSimple::detectingMarkerInImage( const cv::Mat& aImage ){

    std::vector<DetectedMarker> vFoundMarkers;

    mBinaryImage = aImage > mThreshold;
    mRealPoints.clear();
    mPoints.clear();

    try{
        cv::findNonZero(
                        mBinaryImage(
                                     cv::Rect(
                                              cv::Point(std::max(0, mDetectionAreaMinX),std::max(0, mDetectionAreaMinY)),
                                              cv::Point(std::min(mBinaryImage.size().width, mDetectionAreaMaxX),std::min(mBinaryImage.size().height, mDetectionAreaMaxY))
                                              )
                                     ),
                                     mPoints );
    } catch( cv::Exception vNoPointsDetected ){
        std::cout << "No Points detected!\n" << std::endl;
    }

    while( !mPoints.empty() ){
        mCounterI = 0;
        mArea.clear();

        mArea.push_back (mPoints.back());
        mPoints.pop_back();

        while( mCounterI < mArea.size() ){

            for( mCounterJ = mPoints.size() - 1; mCounterJ >= 0; --mCounterJ ){

                if( mArea[mCounterI].y == mPoints[mCounterJ].y && mArea[mCounterI].x + 1 == mPoints[mCounterJ].x ||
                    mArea[mCounterI].y == mPoints[mCounterJ].y && mArea[mCounterI].x - 1 == mPoints[mCounterJ].x ||
                    mArea[mCounterI].y + 1 == mPoints[mCounterJ].y && mArea[mCounterI].x == mPoints[mCounterJ].x ||
                    mArea[mCounterI].y - 1 == mPoints[mCounterJ].y && mArea[mCounterI].x == mPoints[mCounterJ].x ||
                    mArea[mCounterI].y + 1 == mPoints[mCounterJ].y && mArea[mCounterI].x + 1 == mPoints[mCounterJ].x ||
                    mArea[mCounterI].y - 1 == mPoints[mCounterJ].y && mArea[mCounterI].x - 1 == mPoints[mCounterJ].x ||
                    mArea[mCounterI].y - 1 == mPoints[mCounterJ].y && mArea[mCounterI].x + 1 == mPoints[mCounterJ].x ||
                    mArea[mCounterI].y + 1 == mPoints[mCounterJ].y && mArea[mCounterI].x - 1 == mPoints[mCounterJ].x ){

                    mArea.push_back( mPoints[mCounterJ] );
                    mPoints.erase( mPoints.begin() + mCounterJ );

                }

            }
            ++mCounterI;
        }
        mRealPoints.push_back(cv::Point());
        for( cv::Point vPoint : mArea ){

            mRealPoints.back() += vPoint;

        }

        mRealPoints.back().x /= mArea.size();
        mRealPoints.back().y /= mArea.size();

    }

    for( int vI = 0; vI < mRealPoints.size(); ++vI ){
        mRealPoints[vI].x += mDetectionAreaMinX;
        mRealPoints[vI].y += mDetectionAreaMinY;
    }

    mFoundCombinations.clear();
    if( mDistanceBetweenPoints.size() < mRealPoints.size() ){
        mDistanceBetweenPoints.resize( mRealPoints.size() );
        for( mCounterI = 0; mCounterI < mRealPoints.size(); ++mCounterI ){
            mDistanceBetweenPoints[mCounterI].resize( mRealPoints.size() );
        }
    }

    for( mCounterI = 0; mCounterI < mRealPoints.size(); ++mCounterI ){

        for( mCounterJ = 0; mCounterJ < mRealPoints.size(); ++mCounterJ ){
            mDistanceBetweenPoints[mCounterI][mCounterJ] = std::pow( mRealPoints[mCounterI].x - mRealPoints[mCounterJ].x, 2 ) + std::pow( mRealPoints[mCounterI].y - mRealPoints[mCounterJ].y, 2 );
            mDistanceBetweenPoints[mCounterJ][mCounterI] = mDistanceBetweenPoints[mCounterI][mCounterJ];
        }

    }

    for( int mCounterI = 0; mCounterI < mRealPoints.size(); ++mCounterI ){

        mPointA = mCounterI;

        for( int mCounterJ = 0; mCounterJ < (mRealPoints.size() - mCounterI) ; ++mCounterJ ){

            mPointB = (mCounterI + mCounterJ) % mRealPoints.size();
            mDistanceAB = mDistanceBetweenPoints[mPointA][mPointB];

            if( mPointA != mPointB &&
                mDistanceAB <= mMaxRangeBetweenPoints * 1.5 &&
                mDistanceAB >= mMinRangeBetweenPoints ){

                for( int mCounterK = 0; mCounterK < (mRealPoints.size() - mCounterJ) ; ++mCounterK ){

                    mPointC = (mCounterI + mCounterJ + mCounterK) % mRealPoints.size();
                    mDistanceBC = mDistanceBetweenPoints[mPointB][mPointC];

                    if( mPointA != mPointC &&
                        mPointB != mPointC &&
                        mDistanceBC <= mMaxRangeBetweenPoints * 1.5 &&
                        mDistanceBC >= mMinRangeBetweenPoints ){

                        for( int mCounterL = 0; mCounterL < (mRealPoints.size() - mCounterK) ; ++mCounterL ){

                            mPointD = (mCounterI + mCounterJ + mCounterK + mCounterL) % mRealPoints.size();
                            mDistanceCD = mDistanceBetweenPoints[mPointC][mPointD];
                            mDistanceAD = mDistanceBetweenPoints[mPointA][mPointD];

                            if( mPointA != mPointD &&
                                mPointB != mPointD &&
                                mPointC != mPointD &&
                                mDistanceCD <= mMaxRangeBetweenPoints * 1.5 &&
                                mDistanceCD >= mMinRangeBetweenPoints &&
                                mDistanceAD <= mMaxRangeBetweenPoints * 1.5 &&
                                mDistanceAD >= mMinRangeBetweenPoints ){

                                if( !checkForDuplicates(mPointA, mPointB, mPointC, mPointD) ){

                                    mDistanceABtest = mDistanceAB >= mMinRangeBetweenPoints && mDistanceAB <= mMaxRangeBetweenPoints;
                                    mDistanceBCtest = mDistanceBC >= mMinRangeBetweenPoints && mDistanceBC <= mMaxRangeBetweenPoints;
                                    mDistanceCDtest = mDistanceCD >= mMinRangeBetweenPoints && mDistanceCD <= mMaxRangeBetweenPoints;
                                    mDistanceADtest = mDistanceAD >= mMinRangeBetweenPoints && mDistanceAD <= mMaxRangeBetweenPoints;
                                    mDistanceAC = mDistanceBetweenPoints[mPointA][mPointC];
                                    mDistanceACtest = mDistanceAC >= mMinRangeBetweenPoints && mDistanceAC <= mMaxRangeBetweenPoints;
                                    mDistanceBD = mDistanceBetweenPoints[mPointB][mPointD];
                                    mDistanceBDtest = mDistanceBD >= mMinRangeBetweenPoints && mDistanceBD <= mMaxRangeBetweenPoints;

                                    std::vector<int> vCombination;

                                    if( mDistanceABtest && mDistanceBCtest && mDistanceCDtest && mDistanceADtest &&
                                        checkAngles( mRealPoints[mPointA], mRealPoints[mPointB], mRealPoints[mPointC], mRealPoints[mPointD] ) ){

                                        vCombination.push_back( mPointA );
                                        vCombination.push_back( mPointB );
                                        vCombination.push_back( mPointC );
                                        vCombination.push_back( mPointD );

                                    } else if( mDistanceABtest && mDistanceBDtest && mDistanceCDtest && mDistanceACtest &&
                                               checkAngles( mRealPoints[mPointA], mRealPoints[mPointB], mRealPoints[mPointD], mRealPoints[mPointC] ) ){

                                        vCombination.push_back( mPointA );
                                        vCombination.push_back( mPointB );
                                        vCombination.push_back( mPointD );
                                        vCombination.push_back( mPointC );

                                    } else if( mDistanceACtest && mDistanceBCtest && mDistanceBDtest && mDistanceADtest &&
                                               checkAngles( mRealPoints[mPointA], mRealPoints[mPointC], mRealPoints[mPointB], mRealPoints[mPointD] ) ){

                                        vCombination.push_back( mPointA );
                                        vCombination.push_back( mPointC );
                                        vCombination.push_back( mPointB );
                                        vCombination.push_back( mPointD );

                                    }

                                    if( !vCombination.empty() ){

                                        mFoundCombinations.push_back(vCombination);

                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    cv::Mat vRegionOfInterest, vRegionOfInterestImage, vMarkerImage, vRotationMatrix;

    std::vector<cv::Point> vPotentialMarker;
    cv::RotatedRect vRotationBox;

    for( std::vector<int> vMarker : mFoundCombinations){

        try{

        vPotentialMarker.clear();
        vPotentialMarker.push_back(mRealPoints[vMarker[0]]);
        vPotentialMarker.push_back(mRealPoints[vMarker[1]]);
        vPotentialMarker.push_back(mRealPoints[vMarker[2]]);
        vPotentialMarker.push_back(mRealPoints[vMarker[3]]);

        vRotationBox = cv::minAreaRect(cv::Mat(vPotentialMarker));
        vRegionOfInterest = mBinaryImage(vRotationBox.boundingRect());
        vRotationMatrix = cv::getRotationMatrix2D(cv::Point(vRegionOfInterest.size())*0.5, vRotationBox.angle, 1);

        cv::warpAffine(vRegionOfInterest, vRegionOfInterestImage, vRotationMatrix, vRegionOfInterest.size(), cv::INTER_CUBIC);
        cv::resize(vRegionOfInterestImage, vMarkerImage, cv::Size(), 2, 2);

        vMarkerImage = vMarkerImage > 50;
        DetectedMarker vUnDetectedMarker( vMarkerImage, vRotationBox.angle, vRotationBox.center);

        if( identifyMarker(vUnDetectedMarker) ){
            vFoundMarkers.push_back(vUnDetectedMarker);
        }

        } catch( cv::Exception dinge ){
            std::cout << dinge.what() << std::endl;
        }
    }



    if( vFoundMarkers.size() ){
        emit markersDetected( vFoundMarkers );
    }
    isRunning = false;

}



bool DetectorSimple::checkForDuplicates( int vA, int vB, int vC, int vD ){

    for( std::vector<int> vFoundCombination : mFoundCombinations ){

        if( std::find(vFoundCombination.begin(), vFoundCombination.end(), vA) != vFoundCombination.end() &&
            std::find(vFoundCombination.begin(), vFoundCombination.end(), vB) != vFoundCombination.end() &&
            std::find(vFoundCombination.begin(), vFoundCombination.end(), vC) != vFoundCombination.end() &&
            std::find(vFoundCombination.begin(), vFoundCombination.end(), vD) != vFoundCombination.end() ){

                return true;

        }

    }


    return false;
}

bool DetectorSimple::checkAngles( const cv::Point& aA, const cv::Point& aB, const cv::Point& aC, const cv::Point& aD ){

    cv::Point vP1 = aC - aB, vP2 = aA - aB; //ABC
    double vAngle = std::atan2( std::abs( vP1.x*vP2.y - vP1.y*vP2.x  ), vP1.x*vP2.x + vP1.y*vP2.y ) * 180/M_PI;
    if( vAngle < 70 || (vAngle > 110 && vAngle < 250 ) || vAngle > 290 ){
        return false;
    }

    vP1 = aD - aC; vP2 = aB - aC; //BCD
    vAngle = std::atan2( std::abs( vP1.x*vP2.y - vP1.y*vP2.x  ), vP1.x*vP2.x + vP1.y*vP2.y ) * 180/M_PI;
    if( vAngle < 70 || (vAngle > 110 && vAngle < 250 ) || vAngle > 290 ){
        return false;
    }

    vP1 = aA - aD; vP2 = aC - aD; //CDA
    vAngle = std::atan2( std::abs( vP1.x*vP2.y - vP1.y*vP2.x  ), vP1.x*vP2.x + vP1.y*vP2.y ) * 180/M_PI;
    if( vAngle < 70 || (vAngle > 110 && vAngle < 250 ) || vAngle > 290 ){
        return false;
    }

    vP1 = aB - aA; vP2 = aD - aA; //DAB
    vAngle = std::atan2( std::abs( vP1.x*vP2.y - vP1.y*vP2.x  ), vP1.x*vP2.x + vP1.y*vP2.y ) * 180/M_PI;
    if( vAngle < 70 || (vAngle > 110 && vAngle < 250 ) || vAngle > 290 ){
        return false;
    }

    return true;

}

void DetectorSimple::detectMarkers( const cv::Mat& aImage ){

    if( mStatus && !isRunning ){
        isRunning = true;
        QtConcurrent::run(this, &DetectorSimple::detectingMarkerInImage, aImage);
    }

}

bool DetectorSimple::identifyMarker( DetectedMarker& aNotYetFoundMarker ){

    cv::Mat vPart, vColorPart;
    int width=aNotYetFoundMarker.mMarkerImage.size().width / 5,
    	height=aNotYetFoundMarker.mMarkerImage.size().height / 5;

    int vInnerWidth = aNotYetFoundMarker.mMarkerImage.size().width - 2*width, vInnerHeight = aNotYetFoundMarker.mMarkerImage.size().height - 2*height;
    int vBonusWidth = 0, vBonusHeight = 0;
    if( vInnerWidth % 3 == 2 ){
        vBonusWidth = 1;
    }
    if( vInnerHeight % 3 == 2 ){
        vBonusHeight = 1;
    }

    int vPositionWidth[4] = {width, width+vInnerWidth/3+vBonusWidth, width+vInnerWidth-vInnerWidth/3-vBonusWidth, width+vInnerWidth},
        vPositionHeight[4] = {height, height+vInnerHeight/3+vBonusHeight, height+vInnerHeight-vInnerHeight/3-vBonusHeight, height+vInnerHeight};

    for( int i = 0; i < 3; i++){
    	for( int j = 0; j < 3; j++){

            vPart = aNotYetFoundMarker.mMarkerImage( cv::Rect( vPositionWidth[j], vPositionHeight[i], vPositionWidth[j+1]-vPositionWidth[j], vPositionHeight[i+1]-vPositionHeight[i]) );
    		aNotYetFoundMarker.mMarker.push_back( countNonZero(vPart)>vPart.size().width*vPart.size().height/4 );

    	}
    }

    int vRotations = 0;
    for( mrvision::Marker vTest : MARKERLIST.getMarker() ){
		if( vTest.compareTo( aNotYetFoundMarker.mMarker, vRotations ) ){
            aNotYetFoundMarker.mMarkerDirection += 270 - vRotations*90;
            aNotYetFoundMarker.mMarkerId = vTest.getId();
			return true;
		}
    }

    return false;
}

void DetectorSimple::blahMarkerAndImage( const cv::Mat& aImage, const QList<bool>& aMarker ){
    emit showMarkerAndImage( aImage, aMarker );
}

void DetectorSimple::setStatus( bool aStatus ){

    mStatus = aStatus;

}
void DetectorSimple::setThreshold( int aThreshold ){
    mThreshold = aThreshold;
}
int DetectorSimple::getThreshold(){
    return mThreshold;
}


void DetectorSimple::setCameraFile( QString aCameraFileName ){

    cv::FileStorage fs(aCameraFileName.toStdString(), cv::FileStorage::READ);
    int w=-1,h=-1;
    cv::Mat MCamera,MDist;

    fs["image_width"] >> w;
    fs["image_height"] >> h;
    fs["distortion_coefficients"] >> MDist;
    fs["camera_matrix"] >> MCamera;

    if (MCamera.cols==0 || MCamera.rows==0)throw cv::Exception(9007,"File :"+aCameraFileName.toStdString()+" does not contains valid camera matrix","CameraParameters::readFromXML",__FILE__,__LINE__);
    if (w==-1 || h==0)throw cv::Exception(9007,"File :"+aCameraFileName.toStdString()+" does not contains valid camera dimensions","CameraParameters::readFromXML",__FILE__,__LINE__);

    if (MCamera.type()!=CV_32FC1) MCamera.convertTo(mCameraMatrix,CV_32FC1);
    else mCameraMatrix=MCamera;

    if (MDist.total()<4) throw cv::Exception(9007,"File :"+aCameraFileName.toStdString()+" does not contains valid distortion_coefficients","CameraParameters::readFromXML",__FILE__,__LINE__);
    //convert to 32 and get the 4 first elements only
    cv::Mat mdist32;
    MDist.convertTo(mdist32,CV_32FC1);
//     Distorsion.create(1,4,CV_32FC1);
//     for (int i=0;i<4;i++)
//         Distorsion.ptr<float>(0)[i]=mdist32.ptr<float>(0)[i];

    mDistortionMatrix.create(1,5,CV_32FC1);
    for (int i=0;i<5;i++)
        mDistortionMatrix.ptr<float>(0)[i]=mdist32.ptr<float>(0)[i];
    mCameraSize.width=w;
    mCameraSize.height=h;

}

    void DetectorSimple::setDetectionArea( int aMinX, int aMaxX, int aMinY, int aMaxY ){

        mDetectionAreaMaxX = aMaxX;
        mDetectionAreaMaxY = aMaxY;
        mDetectionAreaMinX = aMinX;
        mDetectionAreaMinY = aMinY;

    }
}
