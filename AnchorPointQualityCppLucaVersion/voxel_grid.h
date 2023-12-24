#ifndef VOXEL_GRID_H
#define VOXEL_GRID_H

/*
 * In questa classe definiamo una possibile implementazione della voxel grid.
 * Si noti come non é assolutamente obbligatorio che il sistema su cui gira il metodo
 * utilizzi questa classe per rappresentare le tasche.
 * Infatti la classe Classifier prende in ingresso un VoxelGridDecorator, non un VoxelGrid.
 * In un file separato verrá prodotto anche il VoxelGridDecorator specializzato per la classe
 * definita in questo file
 */

class VoxelGrid
{
	public:
	VoxelGrid(std::string path) : array(path) {};
	double operator()(int i,int j,int k) {
		return array.get(i,j,k);
	}
	
	NumpyArray & getArray() {
		return array;
	}
	
	private:
	NumpyArray array;
};

#endif 