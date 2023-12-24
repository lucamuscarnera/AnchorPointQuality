#include "tensor.hpp"
#include "voxel_decorator.h"
#include "classifier.h"
#include "numpy_array.h"
#include "voxel_grid.h"
#include "example_decorator.h"

#include <iostream>



int main()
{
	/*
	int voxelgrid = 3;
	MyVoxelDecorator V(voxelgrid);
	Classifier classifier(V);
	std::cout << "Hello World!" << std::endl;
	std::cout << V.dummyget() << std::endl;
	
	Tensor<int,3> A;
	Tensor<int,3> B;
	
	for(int i = 0 ; i < 3;i++)
		for(int j = 0 ; j < 3;j++)
			for(int k = 0; k < 3;k++)
			{
				A(i,j,k) = 1;
				B(i,j,k) = 1;
			}
	std::cout << "L'output dovrebbe essere 3^3 ovvero 27" << std::endl;
	std::cout << "A @ B = " << A.dot(B) << std::endl;
	*/
	VoxelGrid voxelgrid("voxelization_75.npy");
	ExampleDecorator voxeldecorator(voxelgrid);
	Tensor A = voxeldecorator.subvoxelgrid<5>(10,10,10);
	Tensor B = voxeldecorator.subvoxelgrid<5>(10,10,10);
	std::cout << "prodotto scalare tra le sottogriglie = " <<  A.dot(B) << std::endl;
};