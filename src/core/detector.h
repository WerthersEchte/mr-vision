#ifndef _detector_h
#define _detector_h

#include "src/core/markerlist.h"
#include "src/core/bot.h"

#include <vector>
#include <aruco/aruco.h>

#include <QObject>

namespace mrvision {

class Detector : public QObject{

    Q_OBJECT

public:
    virtual ~Detector(){};

    virtual void setMarkerSize( float aMarkerSize ) = 0;
    virtual float getMarkerSize() = 0;

    virtual void setCameraFile( QString aCameraFile ) = 0;
    virtual aruco::CameraParameters* getCameraFile() = 0;

    virtual void setThreshold( int aThreshold = 128 ) = 0;
    virtual int getThreshold() = 0;
    virtual void setStatus( bool aStatus ) = 0;

public slots:
    virtual void detectMarkers( const cv::Mat& aImage ) = 0;

signals:
    void markersDetected( const std::vector<aruco::Marker>& aListOfMarkers );
    void showMarkerAndImage( const cv::Mat& aImage, const QList<bool>& aMarker );

};

}

#endif // _detector_h
