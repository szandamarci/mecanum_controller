#ifndef LEUZE_ASCIIREMOTEPROTOCOLTYPES_H
#define LEUZE_ASCIIREMOTEPROTOCOLTYPES_H


namespace Leuze
{
	namespace ROD4
	{
		/** \enum CoordinateSystem
		\brief Possible ROD4 ascii remote protocol coordinate systems
		*/
		enum CoordinateSystem
		{
			COORD_POLAR,    /**< */
			COORD_CARTESIAN /**< */
		};
		/**
		\brief Represents a distance measure in a polar coordinate system.
		*/
		struct PolarCoordinate
		{
			unsigned short distance;
		};
		/**
		\brief Represents a distance measure in a cartesian coordinate system.
		*/
		struct CartesianCoordinate
		{
			signed short   xValue;
			signed short   yValue;
		};
		/**
		\brief Union storing the coordinate type of measure data
		*/
		union Coordinate
		{
			PolarCoordinate     polar;
			CartesianCoordinate cartesian;
		};
		/**
		\brief ROD4 ascii remote measurement segment 
		*/
		struct Segment
		{
			/**
			Index of segment with segments list
			*/
			unsigned short  index;
			/**
			Number of measure points
			*/
			unsigned short  coordinateCount;
			/**
			Measure points
			*/
			Coordinate     *coordinates;
		};
		/**
		\brief ROD4 ascii remote measure datagram
		*/
		struct AsciiMeasureDatagram 
		{
			/**
			Scan number of measurement
			*/
			unsigned int      scanNumber;
			/**
			Applied coordinate system
			*/
			CoordinateSystem  coordinateSystem;
			/**
			Number of segments
			*/
			unsigned short    segmentCount;
			/**
			Measure segments
			*/
			Segment          *segments;
			AsciiMeasureDatagram(void);
			~AsciiMeasureDatagram(void);
		};
	}
}


#endif //LEUZE_ASCIIREMOTEPROTOCOLTYPES_H