#ifndef POINT_MATRIX_H
#define POINT_MATRIX_H

#include <string>

#include <vector>
#include "point3d.h"
#include "SimplePlot.h"
#include <Eigen/Dense>


class PointMatrix
{
	public:
	
		PointMatrix()  {}  
		PointMatrix(std::string path) :
		array(path)
		{
			for(int i = 0; i < array.shape[0];i++)
			{	
				pointSet.push_back(
					Point3D(array.get(i,0),
							array.get(i,1),
							array.get(i,2)
					)
				);
			}
		}
		
		
		void addPoint(Point3D & p)
		{
			pointSet.push_back(p);
		}
		
		std::vector<Point3D> & getPointSet() {
			return pointSet;
		}
	
		int size() {
			return pointSet.size();
		}
		
		Point3D sampleMean()
		{
			Point3D ret(0,0,0);
			double i = 0;
			for(auto & p: pointSet)
			{
				ret.getX() = (ret.getX() * i + p.getX())/(i + 1.);
				ret.getY() = (ret.getY() * i + p.getY())/(i + 1.);
				ret.getZ() = (ret.getZ() * i + p.getZ())/(i + 1.);
				i++;
			}
			return ret;
		}
		
		void shift(Point3D & q)
		{
			
			for(auto & p: pointSet)
			{
				p.getX() = (p.getX() - q.getX());
				p.getY() = (p.getY() - q.getY());
				p.getZ() = (p.getZ() - q.getZ());
			}	
		}
		
		
		Eigen::MatrixXd XTX()
		{
			Eigen::MatrixXd ret(3,3);
			for(int i = 0 ; i < 3;i++)
			{
				for(int j = i ; j < 3;j++)
				{
					
					float valore = 0;
					for(auto & p : pointSet)
					{
						valore += p[i] * p[j];
					}
					
					ret(i,j) = valore;
					ret(j,i) = valore;
				}	
			}
			//std::cout << ret << std::endl;
			return ret;
		}		
		
		void project(Eigen::MatrixXd & V)
		{
			for(auto & p: pointSet)
			{
				Point3D newp(0,0,0);
				for (int i = 0; i < 3;i++)
					newp[i] =  p[0] * V(0,i) + p[1] * V(1,i) + p[2] * V(2,i);
				p[0] = newp[0];
				p[1] = newp[1];
				p[2] = newp[2];
				//p.print();
				//std::cout << std::endl;
			}
		}
		
		void standardize() 
		{
				// scritta sporca, giusto per provare. TODO: refactor
				Point3D minimi(pointSet[0].getX(),pointSet[0].getY(),pointSet[0].getZ());
				Point3D massimi(pointSet[0].getX(),pointSet[0].getY(),pointSet[0].getZ());
				for( auto & p : pointSet)
				{
					for(int i = 0 ; i < 3 ; i ++)
					{
						if( p[i] > massimi[i] )
						{
							massimi[i] = p[i];
						}
						else
						{
							if(p[i] < minimi[i] )
							{
								minimi[i] = p[i];
							}
						}
					}
				}
				
				// adesso massimi = (x_max, y_max, z_max) e minimi = (x_min, y_min, z_min)
				
				// posso applicare la standardizzazione
				for( auto & p : pointSet)
				{
					for(int i = 0 ; i < 3;i++)
					{
						p[i] = (p[i] - minimi[i])/(massimi[i] - minimi[i]);
					}
				}
				

		}
		
		void mapIntoRepresentant() {
				/*********************************************************************************************	
				 *	mappa la nuvola di punti nel suo rappresentante di equivalenza della relazione ~ dove
				 *  x ~ y <--> esiste una rotazione che mappa x in y.
				 *
				 *********************************************************************************************/
				
			
				// 3. Sottrai la media alla nuvola di punti
				//			>>> pointSet <= pointSet - media(pointSet)
				Point3D mean = sampleMean();
				shift(mean);
				
				
				
				// 4. Calcola la matrice di covarianza della nuvola
				//			>>> C <= pointSet.covariance()
				
				Eigen::MatrixXd C = XTX();
				
				// 5. Ottieni la matrice degli autovettori di Calcola
				//			>>> V <= eigenvectors(C)
				
				Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver;
				eigensolver.compute(C);
				Eigen::MatrixXd V = eigensolver.eigenvectors();
				// ordino in maniera decrescente
				V.rowwise().reverseInPlace();
				//std::cout << "Matrice autovettori" << std::endl;
				//std::cout << V << std::endl;
				
				// 6. Proietta il PointSet contro V
				//			>>> pointSet' <= pointSet.project(V)
				/*{
					//SimplePlot sp;
					for(auto & p: pointSet){ 
						p.print();
						std::cout << std::endl;
					}
					//sp.show(true);
				}*/
				// std::cout << "PROIETTO" << std::endl;
				
				project(V);

				
				// std::cout << "PROIEZIONE ESEGUITA" << std::endl;
				/*
				{
					//SimplePlot sp;
					for(auto & p: pointSet){ 
						p.print();
						std::cout << std::endl;
					}
					//sp.show(true);
				}*/		
				// 7. Ottieni il segno delle coordinate massime in modulo
				//			>>> massimali[3] = -1,-1,-1
				Point3D massimali(0,0,0);
				//			>>> for p in pointSet' 
				for(auto & p : pointSet) 
				{
				//			>>>		for i,coord in p.coordinate
					for(int i = 0 ; i < 3; i++)
					{
				//			>>>			if |coord| > massimali[i]
						if( std::abs(p[i]) > std::abs(massimali[i]) )
						{							
				//			>>>				massimali[i] = coord
							massimali[i]  = p[i];
						}
					}
				//			>>> segni = sign(massimali)
				}
				
				/*
				std::cout << "TROVATI I MASSIMALI " << std::endl;
				massimali.print();
				std::cout << std::endl;
				*/
				
				// cambia i segni
				for(auto & p: pointSet){ 
						for(int i = 0 ; i < 3;i++)
						{
								if( massimali[i] < 0 )
								{
									p[i] *= -1;				// in questo modo forzo l'elemento massimale ad avere segno positivo
								}
						}
					}
			
				
				
				
				//std::cout << "INVARIANZA PER SEGNO OTTENUTA" << std::endl;
				/*{
					//SimplePlot sp;
					for(auto & p: pointSet){ 
						//p.print();
						std::cout << std::endl;
					}
					//sp.show(true);
				}*/	
		}
		
	private:
		NumpyArray<double> array;
		std::vector<Point3D> pointSet;
};

#endif