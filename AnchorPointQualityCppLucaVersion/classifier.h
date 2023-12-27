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
		
		// predizione direttamente su un tensore e su una finger print
		template
		<typename T,typename FPD>
		auto base_predict(Tensor<T,side> & X, FPD & f, bool proto = true) // if proto é vera allora il ligando non viene considerato.
		{
			if(proto)
			{
				return X.dot(W);
			}
			else
			{
				float fWf = f.dot(Wf);
				return X.specialDot(W,2,fWf);
			}
		}
		
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
			
			return A.specialDot(W,2,fWf);
		}
		
		Tensor<T_voxel,side> & getW() {return W;}
		
		
		
	private:
		Tensor<T_voxel,side> & W;
		NumpyArray<float> & Wf;
};

#endif