#ifndef LEUZE_ROD4BINARYPROTOCOLTYPES_H
#define LEUZE_ROD4BINARYPROTOCOLTYPES_H


namespace Leuze
{
	namespace ROD4
	{
		/**
		\brief Option 1 bit field
		*/
		struct Option1Bitfield 
		{
			/**
			The total number of option bytes
			*/
			unsigned char numberOfOptions : 2;
			/**
			Current operation mode of sensor
			*/
			unsigned char status : 3;
			unsigned char reserved : 3;
		};
		/**
		\brief Option 1
		*/
		union Option1 
		{
			Option1Bitfield bit;
			unsigned char   byte;
		};
		/**
		\brief Option 2 bit field
		*/
		struct Option2Bitfield 
		{
			/**
			Signals an occupation of detection field near 1
			*/
			unsigned char detectionField_1_near : 1;
			/**
			Signals an occupation of detection field far 1
			*/
			unsigned char detectionField_1_far : 1;
			/**
			Signals an active warning
			*/
			unsigned char warning : 1;
			/**
			Signals occurence of an error and thus sensor operation mode error/configuration
			*/
			unsigned char fault : 1;
			/**
			Signals whether rerun lock is engaged
			*/
			unsigned char restart_disable : 1;
			/**
			Signals an occupation of detection field near 2
			*/
			unsigned char detectionField_2_near : 1;
			/**
			Signals an occupation of detection field far 2
			*/
			unsigned char detectionField_2_far : 1;
			/**
			Signals whether option byte 3 follows
			*/
			unsigned char optionByte_3_follows : 1;
		};
		/**
		\brief Option 2
		*/
		union Option2 
		{
			Option2Bitfield bit;
			unsigned char   byte;
		};
		/**
		\brief Option 3 bit field
		*/
		struct Option3Bitfield 
		{
			/**
			Selected first detection field pair number (if not in eigth field mode)
			*/
			unsigned char detectionField_1_fieldPair : 3;
			/**
			Selected second detection field pair number (if not in eigth field mode)
			*/
			unsigned char detectionField_2_fieldPair : 3;
			/**
			State of OSSD ouputs.
			*/
			unsigned char ossd_status : 1;
			unsigned char reserved : 1;
		};
		/**
		\brief Option 3
		*/
		union Option3 
		{
			Option3Bitfield bit;
			unsigned char   byte;
		};
		/**
		\brief Datagramm frame (header & footer)
		*/
		struct DatagramFrame 
		{
			/**
			The type of message received
			*/
			unsigned char msg_identifier;
			/**
			Status byte 1
			*/
			Option1 option1;
			/**
			Status byte 2
			*/
			Option2 option2;
			/**
			Status byte 3
			*/
			Option3 option3;
			/**
			Result of a XOR operation on message content, initialized with the message identifier
			and ending directly before this XOR byte, BUT omitting the 0xFE values separating the 
			scan number octets.
			*/
			unsigned char checksum;
		};
		/**
		\brief Header of measure value datagram.
		(see status byte in DatagramFrame
		*/
		struct MeasuringHeader 
		{
			/**
			Scan number of measurement
			*/
			unsigned long  scanNumber;
			/**
			Output resolution
			*/
			unsigned char  resolution;
			/**
			Number of first measurement angle (1..529)
			*/
			unsigned short start;
			/**
			Index of last measurement angle (1..529)
			*/
			unsigned short stop;
		};
		/**
		\brief ROD4 binary datagram
		*/
		struct Datagram 
		{
			/**
			Data belonging to protocol frame
			*/
			DatagramFrame    frame;
			/**
			Data belonging to measure data message
			*/
			MeasuringHeader  header;
			/**
			Number of distances received
			*/
			unsigned short   distanceCount;
			/**
			Pointer to distances
			*/
			unsigned short  *distances;
			Datagram(void);
			~Datagram(void);
		};
	}
}


#endif //LEUZE_ROD4BINARYPROTOCOLTYPES_H