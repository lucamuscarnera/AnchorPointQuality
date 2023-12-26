#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "tensor.hpp"
#include "data_aggregation.h"
#include "point3d.h"

template
<
typename T_voxel,int side
>
class Classifier
{
	public:
		Classifier(Tensor<T_voxel,side> & W, NumpyArray<float> &  Wf)
		:	
		W(W),
		Wf(Wf)
		{}
		
		template
		<typename VD,typename FPD>
		auto predict(DataAggregation<VD,FPD> & aggregate, Point3D & p)
		{
			// IN  : un punto
			// OUT : un punteggio
			
			// richiedo il punto ad aggregate
			Tensor A  = aggregate.template subvoxelgridFromCoordinates<side>(p);
			
			// calcolo il prodotto scalare tra la finger print e il regressore
			float fWf = aggregate.getFingerPrintDecorator().dot(Wf);
			
			return A.dot(W);
		}
		
	private:
		Tensor<T_voxel,side> & W;
		NumpyArray<float> & Wf;
};

#endif