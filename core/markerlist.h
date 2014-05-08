#ifndef _mrvision_markerlist_h
#define _mrvision_markerlist_h

#include <vector>
#include "marker.h"

namespace mrvision {

class MarkerList {

	std::vector<Marker> mMarker;

public:
	MarkerList();
	virtual ~MarkerList();

	std::vector<Marker> getMarker();
	void print();

};

}

#endif // _mrvision_markerlist_h
