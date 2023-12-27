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
	gradient *= 0.01; // learning rate
	classifier.getW() += gradient;
	return;
	}


int massimo(int a,int b)
{
	return (a > b ? a : b);
}

int main(int argc, char* argv[]) {
	
	constexpr int window = 30;
	// carico i dataset
	
	std::vector<VoxelGrid> 				voxelizations;
	std::vector<VoxelizationSpecs> 		voxelization_specs;
	std::vector<PointMatrix>			anchors;
	std::vector<FingerPrint>			fingerprints;
	
	
	std::string voxelizations_path      = "./processed_data/voxelizations"; 
	std::string voxelization_specs_path = "./processed_data/voxelization_specs"; 
	std::string anchors_path            = "./processed_data/anchors"; 
	std::string fingerprints_path       = "./processed_data/finger_prints"; 

	int N = 100; //number_of_files_in_directory(voxelizations_path);
	
	for(int i = 0; i < N;i++)
	{
		std::cout << "leggo file " << i << std::endl;
		voxelizations.push_back(voxelizations_path                + "/voxelization_"      + std::to_string(i) + ".npy");
		voxelization_specs.push_back(voxelization_specs_path      + "/voxelization_spec_" + std::to_string(i) + ".npy");
		anchors.push_back(anchors_path                            + "/anchor_"            + std::to_string(i) + ".npy");
		fingerprints.push_back(fingerprints_path                  + "/finger_print_"      + std::to_string(i) + ".npy");
	}
	
	Tensor<float,window>   W("test/W_fine_15.npy");										// carico il tensore dei pesi
	W *= 1e-5;
	NumpyArray<float>  Wf("test/Wf_fine_15.npy");									// carico il tensore dei pesi del finger print [TODO questo formato non va bene]
	Classifier<float,window> classifier(W,Wf);											// inizializzo il classificatore usando il vettore dei pesi appena caricato
	
	std::vector<float> history;
	
	for( int i  =0 ; i < 2000 ;i ++)
	{
		float reward =  random_batch_reward<float,window>(
			classifier,
			150,
			voxelizations,
			voxelization_specs,
			anchors,
			fingerprints
		);
		history.push_back(reward);
		float media = 0;
		float count = 0;
		for(int i = massimo(0, history.size() - 100); i < history.size();i++)
		{
			media = (count * media + history[i])/(count + 1.);
			count++;
		}
		
		std::cout << "reward = " << reward << " \t last 100 average" <<  media <<  std::endl;
		random_batch_reward_grad_iteration<float,window>(
			classifier,
			100,
			voxelizations,
			voxelization_specs,
			anchors,
			fingerprints
		);
		
	}
    return 0;
}