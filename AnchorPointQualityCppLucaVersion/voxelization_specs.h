#ifndef VOXELIZATION_SPECS_H
#define VOXELIZATION_SPECS_H

/*
 * Definisco una classe in grado di contenere le informazioni spaziali sulla voxelgrid
 */
#include <string>

class VoxelizationSpecs
{
	public:
	
	VoxelizationSpecs(double x_min,double x_max,
					  double y_min,double y_max,
					  double z_min,double z_max):
					  x_min(x_min),x_max(x_max),y_min(y_min),y_max(y_max),z_min(z_min),z_max(z_max) {};
	
	VoxelizationSpecs(std::string path)
	{
		// inizializzazione da file .npy
		std::cout << path << std::endl;
		NumpyArray<double> array(path);
		std::cout << "ordine = " << array.shape.size() << " shape = " <<  array.shape[0] << std::endl;
		x_min = array.get(0);
		x_max = array.get(1);
		y_min = array.get(2);
		y_max = array.get(3);
		z_min = array.get(4);
		z_max = array.get(5);
		
	}
	
	void print()
	{
		std::cout << "x in (" << x_min << ","<< x_max <<")" << std::endl;
		std::cout << "y in (" << y_min << ","<< y_max <<")" << std::endl;
		std::cout << "z in (" << z_min << ","<< z_max <<")" << std::endl;
	}
	
	double x_min;
	double x_max;
	double y_min;
	double y_max;
	double z_min;
	double z_max;
};

#endif