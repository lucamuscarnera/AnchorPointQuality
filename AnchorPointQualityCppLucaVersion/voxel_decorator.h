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
					   padValue(padValue),
					   zoom(zoom)
					   {}											// costruttore
					   
		const virtual format get(int i,int j,int k) = 0;		  	// funzione puramente virtuale per ottenere un elemento
		
		const virtual size_t shape_X() = 0;							// metodi per determinare
		const virtual size_t shape_Y() = 0;							// le dimensioni della voxelgrid
		const virtual size_t shape_Z() = 0;							 
		
		const format dummyget() {									// esempio di funzione che usa l'implementazione 
			return get(0,0,0);										// della classe figlia di get per svolgere operazioni
		};
		
		template<int side>
		const Tensor<format, side> 
		subvoxelgrid(int i,											// indice x
					 int j,											// indice y
					 int k, 										// indice z
					 bool onlyball = false)							// se settato true pone a 0 tutti i voxel al di fuori della sfera inscritta nella VG
		{
			Tensor<format, side> ret;
			int halfside = side / 2.;
			for(int ii = ((int) i) - halfside; ii < ((int) i) + halfside; ii++)
			{
				for(int jj = ((int) j) - halfside; jj < ((int) j) + halfside; jj++)
				{
					for(int kk = ((int) k) - halfside; kk < ((int) k) + halfside; kk++)
					{					
						format value = 0;
						format coeff = 0;
						if( ((ii < 0) || (ii >= shape_X())) ||
							((jj < 0) || (jj >= shape_Y())) ||
							((kk < 0) || (kk >= shape_Z()))
						)
						{
							value = 0;
						}
						else
						{
							if(!onlyball)
							{
								coeff = 1;
							}
							else
							{
								// calcolo le coordinate 
								// nel sistema di riferimento cartesiano sul dominio I = (-1,1)^3
								double x = (((double)ii) - (((double)i) - halfside))/side * 2 - 1;  
								double y = (((double)jj) - (((double)j) - halfside))/side * 2 - 1;
								double z = (((double)kk) - (((double)k) - halfside))/side * 2 - 1;
								// considero solo i punti all interno della sfera di raggio 1
								if( (x*x + y*y + z*z) <= 1)
								{
									coeff = 1;
								}									
							}
							value = coeff * get(ii,jj,kk);
						}
						ret(ii - ( ((int) i) - halfside),jj - ( ((int) j) - halfside),kk - ( ((int) k) - halfside)) = value;
					}
				}
			}
			return ret;
		}

	
	protected:	
		T & voxelgrid;												// reference alla voxelgrid passata al costruttore
		format padValue;
		float zoom;
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