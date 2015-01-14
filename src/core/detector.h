#ifndef _detector_h
#define _detector_h

#include "src/core/markerlist.h"
#include "src/core/bot.h"

#include <vector>
#include <aruco/aruco.h>

namespace mrvision {

class Detector{

public:
    virtual ~Detector(){};

    virtual void setMarkerList( MarkerList* aMarkerList ) = 0;
    virtual std::vector<aruco::Marker>* detectMarkers( const cv::Mat& aImage ) = 0;

};

}

#endif // _detector_h
