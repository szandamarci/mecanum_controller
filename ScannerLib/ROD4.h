#ifndef LEUZE_ROD4_H
#define LEUZE_ROD4_H


namespace Leuze
{
	namespace ROD4
	{
		/**
		The minimum segment index allowed (1).
		*/
		extern const int MIN_SEGMENT;
		/**
		The maximum segment index allowed (529) - and
		the maximum of segments at the same time).
		*/
		extern const int MAX_SEGMENT;
		/**
		The minimum angel possible (-5.04 grad).
		*/
		extern const float MIN_ANGLE;
		/**
		The maximum angel possible (+185.04 grad).
		*/
		extern const float MAX_ANGLE;
		/**
		The angle gap between two segments (0.36 grad).
		*/
		extern const float DELTA_ANGLE;
	}
}


#endif //LEUZE_ROD4_H