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
			int i = 0;
			for(auto & p: pointSet)
			{
				ret.getX() = (ret.getX() * i + p.getX())/(i + 1);
				ret.getY() = (ret.getY() * i + p.getY())/(i + 1);
				ret.getZ() = (ret.getZ() * i + p.getZ())/(i + 1);
				i+=1;
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
			}
		}
		
		void representant() {
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
				
				Eigen::EigenSolver<Eigen::MatrixXd> eigensolver;
				eigensolver.compute(C);
				Eigen::MatrixXd V = eigensolver.eigenvectors().real();
				
				// 6. Proietta il PointSet contro V
				//			>>> pointSet' <= pointSet.project(V)
				{
					SimplePlot sp;
					for(auto & p: pointSet)
						sp.addPoint(p[0],p[1]);
					sp.show(true);
				}
				project(V);
				{
					SimplePlot sp;
					for(auto & p: pointSet)
						sp.addPoint(p[0],p[1]);
					sp.show(true);
				}				
				// 7. Ottieni il segno delle coordinate massime in modulo
				//			>>> massimali[3] = -1,-1,-1
				//			>>> for p in pointSet' 
				//			>>>		for i,coord in p.coordinate
				//			>>>			if |coord| > massimali[i]
				//			>>>				massimali[i] = coord
				//			>>> segni = sign(massimali)
				// 8. Moltiplica l'autovettore i-esimo per il segno i-esimo
				//			>>> V' <= ( segno_1*v_1 | segno_2*v_2 | segno_3*v_3 )
				// 9. Proietta il PointSet contro V'
				//			>>> pointSet <= pointSet.project(V')
			
		}
		
	private:
		NumpyArray<double> array;
		std::vector<Point3D> pointSet;
};

#endif