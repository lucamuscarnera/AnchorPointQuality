#ifndef EXAMPLE_FINGERPRINT_DECORATOR_H
#define EXAMPLE_FINGERPRINT_DECORATOR_H

#include "voxel_decorator.h"

class ExampleFingerPrintDecorator : public FingerPrintDecorator<FingerPrint,double> {
	using FingerPrintDecorator<FingerPrint,double>::FingerPrintDecorator;
	public:
		const double operator[](int i) 
		{
			return fingerprint.getArray()[i];
		}
		
		const size_t size()
		{
			return fingerprint.getArray().shape[0];
		}
};


#endif