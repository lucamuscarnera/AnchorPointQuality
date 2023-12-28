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
#include "point3d.h"
#include "point_matrix.h"

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
					 bool onlyball = false,							// se settato true pone a 0 tutti i voxel al di fuori della sfera inscritta nella VG
					 bool rotational_invariance = false 			// se settato a true mappa la subvoxel grid estratta nel suo rappresentate di equivalenza per la relazione x ~ y dover
																	// x~y <--> esiste una rotazione che porta da x a y 
		)
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
							value = (rand() % 2); // TODO : occhio che qua era  0 , é per fare una prova
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
								
										 //				indice interno (da 0 a side - 1)
										 //   
										 // -------------------'------------------- //
										 // |                                      |
								double x = (((double)ii) - (((double)i) - halfside))/(side - 1) * 2 - 1;  
								double y = (((double)jj) - (((double)j) - halfside))/(side - 1) * 2 - 1;
								double z = (((double)kk) - (((double)k) - halfside))/(side - 1) * 2 - 1;
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
			
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(rotational_invariance)
			{
				// 1. mappa la subvoxel grid in una nuvola di punti
				// 			>>> pointSet <= ...(subvoxelGrid);
				PointMatrix P;
				for(int i = 0 ; i < side ; i++)
					for(int j = 0 ; j < side;j++)
						for(int k = 0; k < side;k++)
						{
							Point3D p(
										 (1. * i)/ (side - 1),
										 (1. * j)/ (side - 1),
										 (1. * k)/ (side - 1)
									  );
							if( ret(i,j,k) > 0.5 )
							{
								P.addPoint(p);
							}
						}

				// 2.a se il numero di punti estratto é minore di 2 ritorrna la griglia ottenuta negli step precedenti
				//			>>> return ret;
				
				if(P.size() < 2)
					return ret;
				
				// 2.b altrimenti mappa la nuvola di punti nel suo rappresentante di equivalenza
				//			>>> pointSet.mapIntoRepresentant()
				P.mapIntoRepresentant();
				// 10. Standardizzo i punti tra 0 e 1
				//			>>> pointSet.standardize()
				P.standardize();
				
				// 11. Sovrascrivo ret
				//			>>>	ret = ret * 0
				ret *= 0;
				//			>>> for p in pointSet'
				for(auto & p : P.getPointSet())
				{
				//			>>>		i = p.coord[0] * ( shapeX - 1 )
				//			>>>		j = p.coord[1] * ( shapeY - 1 )
				//			>>>		k = p.coord[2] * ( shapeZ - 1 )
						int i = p[0] * ( side - 1  );
						int j = p[1] * ( side - 1  );
						int k = p[2] * ( side - 1  );
				//			>>>		ret[i,j,k] = 1
				//		std::cout << i << " " << j << " " << k<< std::endl;
						ret(i,j,k) = 1.;
				}
				// 12. Ritorno  ret
				//			>>> return ret
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