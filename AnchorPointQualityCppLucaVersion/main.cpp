#include "tensor.hpp"
#include "voxel_decorator.h"
#include "numpy_array.h"
#include "voxel_grid.h"
#include "example_decorator.h"
#include "voxelization_specs.h"
#include "point_matrix.h"
#include "fingerprint.h"
#include "fingerprint_decorator.h"
#include "example_fingerprint_decorator.h"
#include "data_aggregation.h"
#include "classifier.h"
#include "SimplePlot.h"

#include <iostream>



int main(int argc, char *argv[])
{
	std::string folder_name = "test";
	int object_name = 75;
	if(argc == 3) {
		folder_name 		= argv[1];												// specifico la cartella
		object_name         = std::atoi(argv[2]);									// specifico quale oggetto
	}

	std::cout << "FOLDER : <" << folder_name << ">, OBJECT : <" << object_name << ">" << std::endl;
	SimplePlot sp; 																	// per visualizzare i risultati
	
	VoxelGrid        		    voxelgrid(folder_name +"/voxelizations/voxelization_" + std::to_string(object_name) + ".npy");				// costruisco un oggetto VoxelGrid
	std::cout << ".";
	ExampleDecorator 			voxeldecorator(voxelgrid);							// inizializzo un decorator per incapsulare tale oggetto
	VoxelizationSpecs		    voxelspec(folder_name + "/voxelization_specs/voxelization_spec_" + std::to_string(object_name) + ".npy");			// costruisco un oggetto Voxelization Specifications
	std::cout << ".";
	
	PointMatrix     	 		anchors(folder_name + "/anchors/anchor_" + std::to_string(object_name) + ".npy");						// costruisco un oggetto Anchors, che contiene una lista di anchor points
	std::cout << ".";
	
	FingerPrint 	  			fingerprint(folder_name + "/finger_prints/finger_print_" + std::to_string(object_name) + ".npy");			// costruisco un oggetto FingerPrint
	ExampleFingerPrintDecorator fd(fingerprint);									// infine inizializzo un decorator per incapsulare la fingerprint
	std::cout << ".";
	

	DataAggregation aggregate(voxeldecorator, 										// Costruisco quindi un oggetto che "impacchetti"
							  voxelspec, 											// i decorator e le specifiche in quanto
							  fd);													// saranno informazioni di cui avrá bisogno il classificatore per fare inferenza
							  
	
	Tensor<float,30>   W("test/W_fine_15.npy");										// carico il tensore dei pesi
	NumpyArray<float>  Wf("test/Wf_fine_15.npy");									// carico il tensore dei pesi del finger print [TODO questo formato non va bene]
	Classifier<float,30> classifier(W,Wf);											// inizializzo il classificatore usando il vettore dei pesi appena caricato
 
	
	// il classificatore quindi, dato in ingresso un aggregato e un punto, usa l'aggregato per estrarre
	// una subvoxelgrid attorno al punto richiesto e la usa, dopo la trasformazione
	// nel rappresentante di equivalenza delle rotazioni - descritta dalla funzione phi - , come query per fare inferenza
	// In notazione di Einstein, in particolare
	//
	//  				Y_hat = W_ijk * phi(X)_ijk + f_l * WF_l
	// 
	
	float  i = -3;
	for(auto & p : anchors.getPointSet())			
	{		
			p.print();
			float y_hat = classifier.predict(aggregate,p);
			float y     = voxelspec.center().squareDistance(p);
			std::cout << "\t\t" << y_hat << "\t\t\t" << y << std::endl;
			sp.addPoint(y_hat,y);
	}
	sp.set_ylabel("valori reali");
	sp.set_xlabel("valori predetti");
	sp.show();
	
};