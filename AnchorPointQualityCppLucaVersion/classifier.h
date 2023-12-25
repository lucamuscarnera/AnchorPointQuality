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
		Classifier(Tensor<T_voxel,side> & W)
		:	
		W(W)
		{}
		
		template
		<typename VD,typename FPD>
		auto predict(DataAggregation<VD,FPD> & aggregate, Point3D & p)
		{
			// IN  : un punto
			// OUT : un punteggio
			// richiedo il punto ad aggregate
			Tensor A = aggregate.template subvoxelgridFromCoordinates<side>(p);
			return A.dot(W);
		}
		
	private:
		Tensor<T_voxel,side> & W;
};

#endif