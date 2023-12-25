#ifndef TENSOR_HPP
#define TENSOR_HPP

#include <array>
#include <string>
#include <iostream>
#include "numpy_array.h"

/*  Specializzazione del lavoro di Giuseppe:  
 *	La classe çontiene tutto il necessario ma possiamo sfruttare 
 *  il fatto di conoscere a compile time la dimensione del tensore.
 *  		Luca
 */

template <typename T, size_t side>
class Tensor {
public:
    Tensor() {}
	Tensor(std::string path)  {
		// genero un oggetto numpy array
		// e copio il contenuto. 
		// concettualmente é uno spreco di tempo, ma va fatto una sola volta
		// al caricamento del programma e rende la gestione di tutto un po piú semplice
		NumpyArray<T> array(path);
		std::cout << "Carico da disco un numpy array di shape " << array.shape[0] << " " << array.shape[1] << " " << array.shape[2] << std::endl;
		for(int i = 0; i < side;i++)
			for(int j = 0; j < side; j ++)
				for(int k = 0; k < side; k++)
					data[index(i,j,k)] = array.get(i,j,k);
	}
	
	
	void print() 
	{
		for(int i = 0; i < side;i++)
		{
			for(int j = 0; j < side; j ++)
			{
				std::cout << "[ ";
				for(int k = 0; k < side; k++) 
				{
					std:: cout << data[index(i,j,k)] << " ";
				} 
				std::cout << "]";
			}
			std::cout << "\n";
		}
	}
	
	
    // Get element by index
    T& operator()(int i, int j, int k) {
        return data[index(i, j, k)];
    }
	
    // linear index from the i,j,k
    int index(int i, int j, int k) const {
        return i * (side*side) + j * side + k;
    }
	
	// dot product
	T dot(Tensor<T,side> & other) const
	{
		T ret = T(0.);
		for(int i = 0; i < side;i++)
			for(int j = 0; j < side; j++)
				for(int k = 0; k < side; k++)
					ret += data[index(i,j,k)] * other(i,j,k);
		
		// si osservi come il ciclo piú interno scorre lungo il terzo indice
		// abbiamo un dot product che é quindi allineato con la data locality
		
		return ret;
	}

private:
    std::array<T,side*side*side> data;
    constexpr size_t size() const {				// ottiene la dimensione totale del tensore
        return side*side*side;
    }
};

#endif // TENSOR_HPP
