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
#include "SimplePlot.h"

#include <iostream>



int main()
{
	
	SimplePlot sp; 																	// per visualizzare i risultati
	
	VoxelGrid        		    voxelgrid("test/voxelization_75.npy");				// costruisco un oggetto VoxelGrid
	ExampleDecorator 			voxeldecorator(voxelgrid);							// inizializzo un decorator per incapsulare tale oggetto
	VoxelizationSpecs		    voxelspec("test/voxelization_spec_75.npy");			// costruisco un oggetto Voxelization Specifications
	Anchors     	 		    anchors("test/anchor_75.npy");						// costruisco un oggetto Anchors, che contiene una lista di anchor points
	FingerPrint 	  			fingerprint("test/finger_print_75.npy");			// costruisco un oggetto FingerPrint
	ExampleFingerPrintDecorator fd(fingerprint);									// infine inizializzo un decorator per incapsulare la fingerprint

	
	DataAggregation aggregate(voxeldecorator, 										// Costruisco quindi un oggetto che "impacchetti"
							  voxelspec, 											// i decorator e le specifiche in quanto
							  fd);													// saranno informazioni di cui avrá bisogno il classificatore per fare inferenza
							  
	
	Tensor<float,30> W("test/W_fine_15.npy");										// carico il tensore dei pesi
	NumpyArray<float>  Wf("test/Wf_fine_15.npy");									// carico il tensore dei pesi del finger print [TODO questo formato non va bene]
	Classifier<float,30> classifier(W,Wf);											// inizializzo il classificatore usando il vettore dei pesi appena caricato
 
	
	// il classificatore quindi, dato in ingresso un aggregato e un punto, usa l'aggregato per estrarre
	// una subvoxelgrid attorno al punto richiesto e la usa, dopo la trasformazione
	// nel rappresentante di equivalenza delle rotazioni - descritta dalla funzione phi - , come query per fare inferenza
	// In notazione di Einstein, in particolare
	//
	//  				Y_hat = W_ijk * phi(X)_ijk + f_l * WF_l
	// 
	
	for(auto & p : anchors.getPointSet())			
	{		
			p.print();
			float y_hat = classifier.predict(aggregate,p);
			float y     = voxelspec.center().squareDistance(p);
			std::cout << "\t\t" << y_hat << "\t\t\t" << y << std::endl;
			sp.addPoint(y_hat, y_hat * y_hat);
	}
	sp.set_ylabel("valori reali");
	sp.set_xlabel("valori predetti");
	sp.show();
	
};