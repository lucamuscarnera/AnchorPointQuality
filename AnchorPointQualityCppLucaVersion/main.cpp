#include "tensor.hpp"
#include "voxel_decorator.h"
#include "numpy_array.h"
#include "voxel_grid.h"
#include "example_decorator.h"
#include "voxelization_specs.h"
#include "anchors.h"
#include "fingerprint.h"
#include "fingerprint_decorator.h"
#include "example_fingerprint_decorator.h"
#include "data_aggregation.h"
#include "classifier.h"

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
	
	/*
	ExampleDecorator voxeldecorator(voxelgrid);
	Tensor A = voxeldecorator.subvoxelgrid<5>(10,10,10);
	Tensor B = voxeldecorator.subvoxelgrid<5>(10,10,10);
	std::cout << "prodotto scalare tra le sottogriglie = " <<  A.dot(B) << std::endl;
	
	
	ExampleFingerPrintDecorator fd(fingerprint);
	std::cout << fd.dot(fd) << std::endl;
	*/
	
	
	VoxelGrid        		    voxelgrid("test/voxelization_75.npy");				// costruisco un oggetto VoxelGrid
	ExampleDecorator 			voxeldecorator(voxelgrid);							// inizializzo un decorator per incapsulare tale oggetto
	std::cout << voxeldecorator.shape_X() 
			  << "," 
			  << voxeldecorator.shape_Y() 
			  << "," 
			  << voxeldecorator.shape_Z() 
			  << std::endl;
	VoxelizationSpecs		    voxelspec("test/voxelization_spec_75.npy");			// costruisco un oggetto Voxelization Specifications
	voxelspec.print();
	
	Anchors     	 		    anchors("test/anchor_75.npy");						// costruisco un oggetto Anchors, che contiene una lista di anchor points
	FingerPrint 	  			fingerprint("test/finger_print_75.npy");			// costruisco un oggetto FingerPrint
	ExampleFingerPrintDecorator fd(fingerprint);									// infine inizializzo un decorator per incapsulare la fingerprint

	
	DataAggregation aggregate(voxeldecorator, 										// Costruisco quindi un oggetto che "impacchetti"
							  voxelspec, 											// i decorator e le specifiche in quanto
							  fd);													// saranno informazioni di cui avrá bisogno il classificatore per fare inferenza
							  
	
	Tensor<double,18> W("test/W_9.npy");											// carico il tensore dei pesi
	Classifier<double,18> classifier(W);											// inizializzo il classificatore usando il vettore dei pesi appena caricato
 
	
	// il classificatore quindi, dato in ingresso un aggregato e un punto, usa l'aggregato per estrarre
	// una subvoxelgrid attorno al punto richiesto e la usa, dopo la trasformazione
	// nel rappresentante di equivalenza delle rotazioni - descritta dalla funzione phi - , come query per fare inferenza
	// In notazione di Einstein, in particolare
	//
	//  				Y_hat = W_ijk * phi(X)_ijk + f_l WF_l
	// 
	
	for(auto & p : anchors.getPointSet())			
	{		
			p.print();
			//std::cout<<std::endl;
			//Tensor A = aggregate.template subvoxelgridFromCoordinates<3>(p);
			std::cout << " : " << classifier.predict(aggregate,p) << std::endl;
	}
	
	
	/*
	std::cout << "Inizializzo un classificatore" << std::endl;
	Classifier classifier(aggregate);
	*/
};