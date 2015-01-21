
#include "markerlist.h"
#include "marker.h"

namespace mrvision {

MarkerList::MarkerList() :
		mMarker()
{

	mMarker.push_back( Marker( std::vector<bool>{true,false,true,false,false,false,false,false,true}, 1 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,false,false,false,false,false,true}, 2 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,false,false,true,false,false,true}, 3 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,false,false,true,false,false,true}, 4 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,true,false,true,false,false,true}, 5 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,true,false,true,false,false,true}, 6 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,true,false,false,false,false,true}, 7 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,true,false,false,false,false,true}, 8 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,false,true,false,false,false,true}, 9 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,false,true,false,false,false,true}, 10 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,true,true,false,false,false,true}, 11 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,true,true,false,false,false,true}, 12 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,false,true,true,false,false,true}, 13 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,false,true,true,false,false,true}, 14 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,true,true,true,false,false,true}, 15 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,true,true,true,false,false,true}, 16 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,false,false,false,false,true,true}, 17 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,false,false,false,false,true,true}, 18 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,true,false,false,false,true,true}, 19 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,true,false,false,false,true,true}, 20 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,false,true,false,false,true,true}, 21 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,false,true,false,false,true,true}, 22 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,true,true,false,false,true,true}, 23 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,true,true,false,false,true,true}, 24 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,false,false,true,false,true,true}, 25 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,false,false,true,false,true,true}, 26 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,true,false,true,false,true,true}, 27 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,true,false,true,false,true,true}, 28 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,false,true,true,false,true,true}, 29 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,false,true,true,false,true,true}, 30 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,false,true,true,true,true,false,true,true}, 31 ) );
	mMarker.push_back( Marker( std::vector<bool>{true,true,true,true,true,true,false,true,true}, 32 ) );
};

MarkerList::~MarkerList(){

};

void MarkerList::print(){

	for( Marker vMarker : mMarker ){

		vMarker.print();

	}

};


std::vector<Marker> MarkerList::getMarker(){

	return mMarker;

};

}

