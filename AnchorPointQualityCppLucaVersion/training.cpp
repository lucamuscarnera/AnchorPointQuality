// training.cpp

#include <iostream>
#include <filesystem>
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
#include <string>
#include <random>

namespace fs = std::filesystem;

std::size_t number_of_files_in_directory(std::filesystem::path path)
{
    using std::filesystem::directory_iterator;
    return std::distance(directory_iterator(path), directory_iterator{});
}

template
<typename T_voxel, int side, typename FingerPrintDecorator>
float reward(Classifier<T_voxel,side> & C, 
			 std::vector<Tensor<double,side>> & Xs, 
			 std::vector<FingerPrintDecorator> & fpds,
			 std::vector<float> & Y
			 )
{
	float ret = 0.;
	std::vector<float> predictions(Xs.size());
	float average_prediction = 0.;
	float average_correct    = 0.;
	for(int i = 0 ; i < Xs.size() ; i++)
	{
			predictions[i] = C.base_predict(Xs[i], fpds[i]);
			average_prediction = (average_prediction * i + predictions[i] ) / (i + 1.);
			average_correct    = (average_correct    * i + Y[i]           ) / (i + 1.);
	}
	
	for(int i = 0 ; i < Xs.size() ; i++)
	{
		ret += (predictions[i] - average_prediction) * (Y[i] - average_correct); 
	}
	
	float norma_pred = 0.;
	float norma_vera = 0.;
	for(int i = 0 ; i < Xs.size() ; i++)
	{
		norma_pred += (predictions[i] - average_prediction)*(predictions[i] - average_prediction);
		norma_vera += (Y[i] - average_correct)             *(Y[i] - average_correct);
	}
	
	ret = ret/std::sqrt(norma_pred * norma_vera);
	return ret;
}

template
<typename T>
T mean(std::vector<T> v)
{
	T ret = 0;
	for(int i = 0;i < v.size();i++)
	{
		ret = (i * ret + v[i])/(i + 1.);
	}
	return ret;
}

template
<typename T_voxel, int side, typename FingerPrintDecorator>
Tensor<float,side> reward_gradient(Classifier<T_voxel,side> & C, 
			 std::vector<Tensor<double,side>> & Xs, 
			 std::vector<FingerPrintDecorator> & fpds,
			 std::vector<float> & Y
			 )
{
	float ret = 0.;

	std::vector<float> predictions(Xs.size());
	for(int i = 0 ; i < Xs.size() ; i++)
	{
			predictions[i] = C.base_predict(Xs[i], fpds[i]);
	}
	
	float average_prediction = mean(predictions);
	float average_correct    = mean(Y);

	Tensor<float,side> A_1;
	Tensor<float,side> X_bar;
	for(int i = 0; i < Xs.size();i ++)
	{
		for(int j = 0 ; j < side; j++)
			for(int k = 0; k < side; k++)
				for(int l = 0; l < side; l++)
					X_bar(j,k,l) = (i * X_bar(j,k,l) + Xs[i](j,k,l)) / (i + 1.);
	}
	
	for(int i = 0; i < Xs.size();i ++)
	{
		for(int j = 0 ; j < side; j++)
			for(int k = 0; k < side; k++)
				for(int l = 0; l < side; l++)
					A_1(j,k,l) += (Xs[i](j,k,l) -  X_bar(j,k,l))*(Y[i] - average_correct);
	}	
	
	float norma_Y_centrato     = 0;
	float norma_Y_hat_centrato = 0;
	for(int i = 0 ; i < Xs.size();i++)
	{
		norma_Y_centrato += (Y[i] - average_correct)*(Y[i] - average_correct);
		norma_Y_hat_centrato += (predictions[i] - average_prediction)*(predictions[i] - average_prediction);		
	}
	norma_Y_centrato     = sqrt(norma_Y_centrato    );
	norma_Y_hat_centrato = sqrt(norma_Y_hat_centrato);

	A_1 *= (1./(norma_Y_hat_centrato * norma_Y_centrato));
	
	Tensor<float,side> B_1;
	
	for(int i = 0; i < Xs.size();i ++)
	{
		for(int j = 0 ; j < side; j++)
			for(int k = 0; k < side; k++)
				for(int l = 0; l < side; l++)
					B_1(j,k,l) += (Xs[i](j,k,l) -  X_bar(j,k,l))*(predictions[i] - average_prediction);
	}	
	
	float dot_Y_c_Y_hat_c = 0;
	for(int i = 0 ; i < Xs.size();i++)
	{
		dot_Y_c_Y_hat_c += (predictions[i] - average_prediction)*(Y[i] - average_correct);
	}
	B_1 *= dot_Y_c_Y_hat_c;
	B_1 *= 1./( norma_Y_hat_centrato * norma_Y_hat_centrato * norma_Y_hat_centrato * norma_Y_centrato);
	A_1 -= B_1;
	return A_1;
}


template
<typename T_voxel,int window>
void extract_random_batch(
	// INPUT
	int N,  // grandezza del batch
	std::vector<VoxelGrid> 			&	voxelizations,
	std::vector<VoxelizationSpecs> 	&	voxelization_specs,
	std::vector<PointMatrix>		&	anchors,
	std::vector<FingerPrint>		&	fingerprints,
	// OUTPUT
	std::vector<Tensor<double,window>> 				& Xs,
	std::vector<float> 				   				& Ys,
	std::vector<ExampleFingerPrintDecorator> 		& fpds 
)
{
		// estraggo i sample da considerare
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, voxelizations.size() - 1);
		std::vector<int> samples_idx(N);
		for (int i = 0; i < N; i++) {
			int estratto = dis(gen);
			samples_idx[i] = estratto;
		}
		
		// per ogni sample
		
		std::uniform_real_distribution<> dis2(0.0, 1.0);
		for(int i = 0 ; 
			i < N;
			i++)
		{
			int sample_idx = samples_idx[i];
			//std::cout << "[ " << samples_idx.size() << "]aggiungendo : " <<  sample_idx << std::endl;
			// inizializzo l'oggetto aggregato
			
			ExampleDecorator 			voxeldecorator(voxelizations[sample_idx]);			// inizializzo un decorator per incapsulare tale oggetto			
			ExampleFingerPrintDecorator fd(fingerprints[sample_idx]);						// infine inizializzo un decorator per incapsulare la fingerprin

			DataAggregation aggregate(voxeldecorator, 										// Costruisco quindi un oggetto che "impacchetti"
									  voxelization_specs[sample_idx], 						// i decorator e le specifiche in quanto
									  fd);	
			
			// estraggo un punto a caso appartenente al dominio descritto da voxelspecs
			
			double x_unmap = dis2(gen);	//
			double y_unmap = dis2(gen);	//	estraggo un vettore in [0,1]^3
			double z_unmap = dis2(gen);	//
			
			// ora devo mappare l'oggetto in [0,1]^3 nel dominio descritto dalla voxel specification
			
			VoxelizationSpecs & vs = voxelization_specs[sample_idx];
			double x = x_unmap * ( vs.x_max - vs.x_min ) + vs.x_min;
			double y = y_unmap * ( vs.y_max - vs.y_min ) + vs.y_min;
			double z = z_unmap * ( vs.z_max - vs.z_min ) + vs.z_min;
			
			Point3D P(x,y,z); 
			
			Point3D center = vs.center();
			double Y = P.squareDistance(center);
			auto   X = aggregate.template subvoxelgridFromCoordinates<window>(P)		;
			
			Xs.push_back(X);                                  // aggiungo il tensore esttratto
			Ys.push_back(Y);                                  // aggiungo la distanza con il centro
			fpds.push_back(ExampleFingerPrintDecorator(fd));  // definisco il decorator
		}
		
}


template
<typename T_voxel,int window>
float random_batch_reward(
	Classifier<T_voxel,window> classifier,
	int N,  // grandezza del batch
	std::vector<VoxelGrid> 			&	voxelizations,
	std::vector<VoxelizationSpecs> 	&	voxelization_specs,
	std::vector<PointMatrix>		&	anchors,
	std::vector<FingerPrint>		&	fingerprints
	)
	{
		
		std::vector<Tensor<double,window>> Xs;
		std::vector<float> Ys;
		std::vector<ExampleFingerPrintDecorator> fpds;
		
		extract_random_batch<T_voxel,window>( 
			// input
					N,
					voxelizations,
					voxelization_specs,
					anchors,
					fingerprints,
			// output
					Xs,
					Ys,
					fpds
		);

		return reward<float,window, ExampleFingerPrintDecorator>(
					  classifier, 
					  Xs, 
				      fpds,
				      Ys
				 );
	}

template
<typename T_voxel,int window>
void random_batch_reward_grad_iteration(
	Classifier<T_voxel,window> classifier,
	int N,  // grandezza del batch
	std::vector<VoxelGrid> 			&	voxelizations,
	std::vector<VoxelizationSpecs> 	&	voxelization_specs,
	std::vector<PointMatrix>		&	anchors,
	std::vector<FingerPrint>		&	fingerprints
	)
	{
		
		std::vector<Tensor<double,window>> Xs;
		std::vector<float> Ys;
		std::vector<ExampleFingerPrintDecorator> fpds;
		
		extract_random_batch<T_voxel,window>( 
			// input
					N,
					voxelizations,
					voxelization_specs,
					anchors,
					fingerprints,
			// output
					Xs,
					Ys,
					fpds
		);


	Tensor<float, window> gradient = reward_gradient<float,window, ExampleFingerPrintDecorator>(
											  classifier, 
											  Xs, 
											  fpds,
											  Ys
										);
	gradient *= 0.1; // learning rate
	classifier.getW() += gradient;
	return;
	}


int massimo(int a,int b)
{
	return (a > b ? a : b);
}

void plot_history(std::vector<float> history)
{
	SimplePlot sp; 			// per visualizzare i risultati
	if(history.size() < 100) return;
	float chunk = ( history.size() / 10.);
	int count = 0;
	for(float i = 0 ; i < history.size() ; i += chunk)
	{
		if(i > 0)
		{
			float media = 0;
			for(int j = 0 ; j < chunk;j++)
			{
				media = (j * media + history[i - int(j)])/(j + 1);
			}
			sp.addPoint(count, media);
		}
		else
			sp.addPoint(count, history[int(i)]);
		count++;
	}
	sp.set_title("REWARD FUNCTION");
	sp.set_xlabel("sample analizzati");
	//sp.set_ylim(0,1);
	sp.show();
}


template
<typename T_voxel,int side>
void plot_grafico(Classifier<T_voxel,side> & classifier, int idx)
{
	SimplePlot sp; 																	// per visualizzare i risultati
	VoxelGrid        		    voxelgrid("./processed_data/voxelizations/voxelization_" + std::to_string(idx) + ".npy");				// costruisco un oggetto VoxelGrid
	ExampleDecorator 			voxeldecorator(voxelgrid);							// inizializzo un decorator per incapsulare tale oggetto
	VoxelizationSpecs		    voxelspec("./processed_data/voxelization_specs/voxelization_spec_" + std::to_string(idx) + ".npy");			// costruisco un oggetto Voxelization Specifications
	
	PointMatrix     	 		anchors("./processed_data/anchors/anchor_" + std::to_string(idx) + ".npy");						// costruisco un oggetto Anchors, che contiene una lista di anchor points
	
	FingerPrint 	  			fingerprint("./processed_data/finger_prints/finger_print_" + std::to_string(idx) + ".npy");			// costruisco un oggetto FingerPrint
	ExampleFingerPrintDecorator fd(fingerprint);									// infine inizializzo un decorator per incapsulare la fingerprint
	

	DataAggregation aggregate(voxeldecorator, 										// Costruisco quindi un oggetto che "impacchetti"
							  voxelspec, 											// i decorator e le specifiche in quanto
							  fd);													// saranno informazioni di cui avrá bisogno il classificatore per fare inferenza
							  
	 
	
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
			float y_hat = classifier.predict(aggregate,p);
			float y     = voxelspec.center().squareDistance(p);
			sp.addPoint(y_hat,y);
	}
	sp.set_title(" SAMPLE #" + std::to_string(idx));
	sp.set_ylabel("valori reali");
	sp.set_xlabel("valori predetti");
	sp.show();
}

int main(int argc, char* argv[]) {
	
	constexpr int window = 26;
	// carico i dataset
	
	std::vector<VoxelGrid> 				voxelizations;
	std::vector<VoxelizationSpecs> 		voxelization_specs;
	std::vector<PointMatrix>			anchors;
	std::vector<FingerPrint>			fingerprints;

	std::vector<VoxelGrid> 				voxelizations_test;
	std::vector<VoxelizationSpecs> 		voxelization_specs_test;
	std::vector<PointMatrix>			anchors_test;
	std::vector<FingerPrint>			fingerprints_test;
		
	
	std::string voxelizations_path      = "./processed_data/voxelizations"; 
	std::string voxelization_specs_path = "./processed_data/voxelization_specs"; 
	std::string anchors_path            = "./processed_data/anchors"; 
	std::string fingerprints_path       = "./processed_data/finger_prints"; 

	int N = number_of_files_in_directory(voxelizations_path);
	int N_train = N * 0.8;
	
	for(int i = 0; i < N;i++)
	{
		if(i < N_train)
		{
			std::cout << "(training set) leggo file " << i << std::endl;
			voxelizations.push_back(voxelizations_path                + "/voxelization_"      + std::to_string(i) + ".npy");
			voxelization_specs.push_back(voxelization_specs_path      + "/voxelization_spec_" + std::to_string(i) + ".npy");
			anchors.push_back(anchors_path                            + "/anchor_"            + std::to_string(i) + ".npy");
			fingerprints.push_back(fingerprints_path                  + "/finger_print_"      + std::to_string(i) + ".npy");
		}
		else
		{
				std::cout << "(test set) leggo file " << i << std::endl;
				voxelizations_test.push_back(voxelizations_path                + "/voxelization_"      + std::to_string(i) + ".npy");
				voxelization_specs_test.push_back(voxelization_specs_path      + "/voxelization_spec_" + std::to_string(i) + ".npy");
				anchors_test.push_back(anchors_path                            + "/anchor_"            + std::to_string(i) + ".npy");
				fingerprints_test.push_back(fingerprints_path                  + "/finger_print_"      + std::to_string(i) + ".npy");
		}
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<float> dis_continuous(0.0,1.0);
	//Tensor<float,window>   W("test/W_fine_15.npy");										// carico il tensore dei pesi
	Tensor<float,window> W(gen, dis_continuous);										// costruisco il tensore partendo da una distribuzione normale univariata
	W *= 1e-5;		
	NumpyArray<float>  Wf("test/Wf_fine_15.npy");										// carico il tensore dei pesi del finger print [TODO questo formato non va bene]
	Classifier<float,window> classifier(W,Wf);											// inizializzo il classificatore usando il vettore dei pesi appena caricato
	
	std::vector<float> history;
	
	int i = 0;
	

	std::uniform_int_distribution<> dis(N_train, N_train + voxelizations_test.size() - 1);   // un sample a caso nel test set

	for(;;)
	{
		float reward =  random_batch_reward<float,window>(
			classifier,
			150,
			voxelizations_test,
			voxelization_specs_test,
			anchors_test,
			fingerprints_test
		);
		history.push_back(reward);
		float media = 0;
		float varianza = 0;
		float count = 0;
		for(int i = massimo(0, history.size() - 100); i < history.size();i++)
		{
			media = (count * media + history[i])/(count + 1.);
			varianza = (count * varianza + (history[i])*(history[i]) )/(count + 1.);
			count++;
		}
		varianza -= media*media;
		std::cout << i << ") \t " << reward << " \t " <<  media <<"\t " << varianza << std::endl;
		random_batch_reward_grad_iteration<float,window>(
			classifier,
			1000,
			voxelizations,
			voxelization_specs,
			anchors,
			fingerprints
		);
		
		if( i % 100 == 0 )
		{
			int idx = dis(gen);
			std::cout << idx;
			plot_grafico(classifier, idx);
			plot_history(history);
		}
		i++;
	}
    return 0;
}