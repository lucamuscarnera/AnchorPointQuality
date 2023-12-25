#ifndef ANCHORS_H
#define ANCHORS_H

#include <string>

#include <vector>
#include "point3d.h"

class Anchors
{
	public:
		Anchors(std::string path) :
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
		
		std::vector<Point3D> & getPointSet() {
			return pointSet;
		}
	
	private:
		NumpyArray<double> array;
		std::vector<Point3D> pointSet;
};

#endif