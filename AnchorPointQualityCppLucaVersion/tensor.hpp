#ifndef TENSOR_HPP
#define TENSOR_HPP

#include <array>
#include <string>
#include <iostream>
#include "numpy_array.h"
#include <concepts>
#include <cmath>

/*  Specializzazione del lavoro di Giuseppe:  
 *	La classe çontiene tutto il necessario ma possiamo sfruttare 
 *  il fatto di conoscere a compile time la dimensione del tensore.
 *  		Luca
 */
 
 
template<typename P,typename T>
concept GroupInjectable = requires (T a, P b)
{
	// richiediamo che il tipo dell'altro tensore
	// possa essere mappato in un oggetto nel gruppo
	// definito dal tipo del tensore corrente.
	// in altri termini richiediamo che sia ben definita 
	// l'operazione di prodotto e somma.
    a + b; 
	a * b; 
};

template <typename T, size_t side>
class Tensor {
public:
    Tensor() {
		for(int i = 0; i < side;i++)
			for(int j = 0; j < side; j ++)
				for(int k = 0; k < side; k++)
					data[index(i,j,k)] = 0;
	}
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
	template<typename P>
	T dot(Tensor<P,side> & other) const
	{
		static_assert(GroupInjectable<P,T>, "!!!");
		{
			T ret = T(0.);
			for(int i = 0; i < side;i++)
				for(int j = 0; j < side; j++)
					for(int k = 0; k < side; k++)
						ret += data[index(i,j,k)] * other(i,j,k);
			return ret;
		}
		// si osservi come il ciclo piú interno scorre lungo il terzo indice
		// abbiamo un dot product che é quindi allineato con la data locality
	}

	// prodotto scalare con "sbilanciamento" nelle sfera di raggio radix
	template <typename P, typename Q>
	T specialDot(Tensor<P,side> & W, int radius, Q & fWf)
	{
		static_assert(GroupInjectable<P,T>, "!!!");
		static_assert(GroupInjectable<Q,T>, "!!!");
		T ret = T(0.);
			for(int i = 0; i < side;i++)
				for(int j = 0; j < side; j++)
					for(int k = 0; k < side; k++)
					{
						
						float x = i - (side / 2);
						float y = j - (side / 2);
						float z = k - (side / 2);
						
						float O_ijk = (x*x + y*y + z*z) < radius*radius;
						ret += data[index(i,j,k)] * ( W(i,j,k) + O_ijk * fWf );
					}
		return ret;
	}

	void operator*= (T scalar)
	{
		for(int i = 0 ; i < side;i++)
			for(int j = 0; j < side;j++)
				for(int k = 0; k < side;k++)
					data[index(i,j,k)] *= scalar;
	}	
	
	void operator-= (Tensor<T,side> & other)
	{
		for(int i = 0 ; i < side;i++)
			for(int j = 0; j < side;j++)
				for(int k = 0; k < side;k++)
					data[index(i,j,k)] -= other(i,j,k);
	}		
	
	void operator+= (Tensor<T,side> & other)
	{
		for(int i = 0 ; i < side;i++)
			for(int j = 0; j < side;j++)
				for(int k = 0; k < side;k++)
					data[index(i,j,k)] += other(i,j,k);
	}	
	
	T norm()
	{
		T ret = 0;
		for(int i = 0; i < side;i++)
			for(int j = 0; j < side; j ++)
				for(int k = 0; k < side; k++)
					ret += data[index(i,j,k)]*data[index(i,j,k)];
		return std::sqrt(ret);
	}
private:
    std::array<T,side*side*side> data;
    constexpr size_t size() const {				// ottiene la dimensione totale del tensore
        return side*side*side;
    }
};

#endif // TENSOR_HPP
