#ifndef EXAMPLE_DECORATOR_H
#define EXAMPLE_DECORATOR_H

#include "voxel_decorator.h"

class ExampleDecorator : public VoxelDecorator<VoxelGrid,double> {
	using VoxelDecorator<VoxelGrid,double>::VoxelDecorator;
	public:
		const double get(int i,int j,int k) override {					// overriding del metodo get
			double ret = voxelgrid(i,j,k);
																		// @TODO : la gestione dello zoom NON dovrebbe essere affidata al decorator
			
			return ret;													// nella classe voxelgrid che abbiamo costruito noi
		};																// l'accesso avviene tramite operatore
		
		const size_t shape_X() override {								// per ottenere le dimensioni della voxel gr	id
			return voxelgrid.getArray().shape[0];					// era necessario accedere alla struttura dati
		}																// su cui poggia la voxelgrid 
		const size_t shape_Y() override {
			return voxelgrid.getArray().shape[1];
		}
		const size_t shape_Z() override {
			return voxelgrid.getArray().shape[2];
		}
};


#endif