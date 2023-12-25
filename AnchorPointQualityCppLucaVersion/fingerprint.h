#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include "numpy_array.h"

class FingerPrint
{
	public:
		FingerPrint(std::string path) :
		array(path)
		{
		}
		
		NumpyArray<int> & getArray() {return array;}
		
	private:
		NumpyArray<int> array;
};

#endif