#include "ROD4BinaryProtocolTypes.h"
//#include <malloc.h>
#include <stdlib.h>

#ifndef NULL
#define NULL 0
#endif


using namespace Leuze::ROD4;


//-------------------------------------------------------------------
Datagram::Datagram(void)
{
	distances = NULL;
	distanceCount = 0;
}
//-------------------------------------------------------------------
Datagram::~Datagram(void)
{
	if(NULL != distances)
	{
		free(distances);
		distances = NULL;
	}
	distanceCount = 0;
}
//-------------------------------------------------------------------
