#ifndef TENSOR_HPP
#define TENSOR_HPP

#include <array>
#include <iostream>


/*  Specializzazione del lavoro di Giuseppe:  
 *	La classe çontiene tutto il necessario ma possiamo sfruttare 
 *  il fatto di conoscere a compile time la dimensione del tensore.
 *  		Luca
 */

template <typename T, size_t side>
class Tensor {
public:
    Tensor() {}
	
    // Get element by index
    T& operator()(int i, int j, int k) {
        return data[index(i, j, k)];
    }
	
    // linear index from the i,j,k
    int index(int i, int j, int k) const {
        return i * (side*side) + j * side + k;
    }
	
	// dot product
	T dot(Tensor<T,side> & other)
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
