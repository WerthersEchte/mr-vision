#ifndef _detector_h
#define _detector_h

#include "src/core/detectedmarker.h"
#include "src/core/markerlist.h"
#include "src/core/bot.h"

#include <vector>

#include <QObject>

namespace mrvision {

enum parameter{

    THRESHOLD,
    MARKERSIZE,
    CAMERAFILE

};

class Detector : public QObject{

    Q_OBJECT

public:
    virtual ~Detector(){};

    virtual void setParameter( parameter aParameter, void* aValue ) = 0;
    virtual void getParameter( parameter aParameter, void* aValue ) = 0;
    virtual void setStatus( bool aStatus ) = 0;

public slots:
    virtual void detectMarkers( const cv::Mat& aImage ) = 0;

signals:
    void markersDetected( const std::vector<DetectedMarker>& aListOfMarkers );
    void showMarkerAndImage( const cv::Mat& aImage, const QList<bool>& aMarker );

};

}

#endif // _detector_h
