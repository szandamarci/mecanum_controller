#include "AsciiRemoteProtocolTypes.h"

//#include <malloc.h>
#include <stdlib.h>

#ifndef NULL
#define NULL 0
#endif


using namespace Leuze::ROD4;


//-------------------------------------------------------------------
AsciiMeasureDatagram::AsciiMeasureDatagram(void)
{
	scanNumber = 0;
	coordinateSystem = COORD_POLAR;
	segments = NULL;
	segmentCount = 0;
}
//-------------------------------------------------------------------
AsciiMeasureDatagram::~AsciiMeasureDatagram(void)
{
	if(NULL != segments)
	{
		for(int i = 0; i < segmentCount; ++i)
		{
			if(NULL != segments[i].coordinates)
			{
				free(segments[i].coordinates);
				segments[i].coordinates = NULL;
			}
			segments[i].coordinateCount = 0;
		}
		free(segments);
		segments = NULL;
	}
	segmentCount = 0;
}
