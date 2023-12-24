#ifndef VOXEL_DECORATOR_H
#define VOXEL_DECORATOR_H

/**********************************************************************************************************
 *  Classe di porting per la voxelizzazione.
 *  Immaginiamo che l'utente finale abbia un oggetto V che svolga il ruolo di voxelgrid.
 *  In linea di principio, sarebbe necessario scrivere tutti i metodi che utilizziamo in modo
 *  da renderli compatibili con V. Per evitare di dover incorrere in questa necessitá costruiamo
 *  una classe astratta VoxelDecorator che 
 *	Questa classe fornisce lo scheletro per costruire uno scheletro che permetta al
 *  nostro sistema di interagire con tale voxelgrid.
 ***********************************************************************************************************/

template<typename T,typename format>
class VoxelDecorator
{
	public:
		VoxelDecorator(T & voxelgrid, format padValue = format(0)) : 
					   voxelgrid(voxelgrid),
					   padValue(padValue)
					   {}											// costruttore
					   
		const virtual format get(int i,int j,int k) = 0;		  	// funzione puramente virtuale per ottenere un elemento
		
		const virtual size_t shape_X() = 0;							// metodi per determinare
		const virtual size_t shape_Y() = 0;							// le dimensioni della voxelgrid
		const virtual size_t shape_Z() = 0;							 
		
		const format dummyget() {									// esempio di funzione che usa l'implementazione 
			return get(0,0,0);										// della classe figlia di get per svolgere operazioni
		};
		
		template<int side>
		const Tensor<format, 2*side> subvoxelgrid(int i,int j,int k)
		{
			Tensor<format, 2*side> ret;
			for(size_t ii = i - side; ii < i + side; ii++)
			{
				for(size_t jj = j - side; jj < j + side; jj++)
				{
					for(size_t kk = k - side; kk < k + side; kk++)
					{
						format value;
						if( ((ii < 0) || (ii >= shape_X())) ||
							((jj < 0) || (jj >= shape_Y())) ||
							((kk < 0) || (kk >= shape_Z()))
						)
						{
							value = padValue;
						}
						else
						{
							value = get(ii,jj,kk);
						}
						ret(ii - (i - side),jj - (j - side),kk - (k -side)) = value;
					}
				}
			}
			return ret;
		}
		
	protected:	
		T & voxelgrid;												// reference alla voxelgrid passata al costruttore
		format padValue;
};


/***********************************************************************************************************
 * In questo esempio vediamo come utilizzare la classe astratta definita
 * in precedenza per produrre un Decorator adatto alle nostre esigenze
 * Si va dunque a costruire una classe figlia della classe template astratta
 * specificando come argomenti del template 
 *		1.	il tipo della voxel grid
 *		2.  il tipo dei valori nelle celle della voxelgrid
 *
 * a quel punto é sufficiente overridere il metodo get e il decorator é pronto!
 * L'algoritmo interagirá con la voxel grid usando il decorator passato in ingresso
 ***********************************************************************************************************/

class MyVoxelDecorator : public VoxelDecorator<int,int> {
	using VoxelDecorator<int,int>::VoxelDecorator;
	public:
		const int get(int i,int j,int k) override {					// overriding del metodo get
			return 1;
		};
		const size_t shape_X() override {
			return 1;
		}
		const size_t shape_Y() override {
			return 1;
		}
		const size_t shape_Z() override {
			return 1;
		}
};


#endif