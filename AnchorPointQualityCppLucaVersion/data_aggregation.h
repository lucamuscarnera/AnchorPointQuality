#ifndef DATA_AGGREGATOR_H
#define DATA_AGGREGATOR_H

/*
 * Classe atta a contenere informazioni riguardo 
 * voxelizzazione,specifiche,anchor points, fingerprint
 */
 
#include "tensor.hpp"
#include <type_traits>
#include <functional>
#include <utility>
#include <typeinfo>


template
<typename VD,typename FPD>
class DataAggregation
{
	public:
		DataAggregation(
			VD  & voxelDecorator,
			VoxelizationSpecs & voxelizationSpecs,
			FPD & fingerPrintDecorator
		)
		:
		voxelDecorator(voxelDecorator),
		voxelizationSpecs(voxelizationSpecs),
		fingerPrintDecorator(fingerPrintDecorator)
		{}
		
		
		// il tipo del dato nel tensore viene dedotto dal decorator della voxel grid	
		// la dimensione della sub voxel grid da estrarre invece, é un parametro a compile time 
		template<int side>
		auto subvoxelgridFromCoordinates(Point3D & p)	
		{			
			
			int i,j,k;
			i = (p.getX() - voxelizationSpecs.x_min) / (voxelizationSpecs.x_max - voxelizationSpecs.x_min) * voxelDecorator.shape_X();
			j = (p.getY() - voxelizationSpecs.y_min) / (voxelizationSpecs.y_max - voxelizationSpecs.y_min) * voxelDecorator.shape_Y();
			k = (p.getZ() - voxelizationSpecs.z_min) / (voxelizationSpecs.z_max - voxelizationSpecs.z_min) * voxelDecorator.shape_Z();
						
			return voxelDecorator.template subvoxelgrid<side>(i,j,k,true);
		}
		
		
		
		VD & getVoxelDecorator() {return voxelDecorator;}
		VoxelizationSpecs & getVoxelizationSpecs() {return voxelizationSpecs;}
		FPD & getFingerPrintDecorator() {return fingerPrintDecorator;}
	
	private:
		VD  & voxelDecorator;
		VoxelizationSpecs & voxelizationSpecs;
		FPD & fingerPrintDecorator;
};

#endif
