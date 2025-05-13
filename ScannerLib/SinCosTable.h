#ifndef LEUZE_SINCOSTABLE_H
#define LEUZE_SINCOSTABLE_H


namespace Leuze
{
	namespace ROD4
	{
		/**
		A lookup table for sinus values
		(array index 0 corresponds to segment index 1).
		*/
		extern const double dSin_table[];
		/**
		A lookup table for cosinus values
		(array index 0 corresponds to segment index 1).
		*/
		extern const double dCos_table[];
	}
}


#endif //LEUZE_SINCOSTABLE_H