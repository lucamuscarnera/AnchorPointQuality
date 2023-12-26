#ifndef POINT3D_H
#define POINT3D_H

class Point3D
{
	public:
		Point3D(double x,double y,double z)
		: x(x), y(y), z(z)
		{	
		}
	
	void print() {
		std::cout << "(" << x << "," << y << "," << z << ")";
	}
	
	
	double squareDistance(Point3D & other)
	{
		return (x - other.x)*(x - other.x) +  (y - other.y)*(y - other.y) +  (z - other.z)*(z - other.z);
	}
	
	double & getX() {return x;}
	double & getY() {return y;}
	double & getZ() {return z;}
	
	double & operator[](int i)
	{
		switch(i)
		{
			case 0 : {
					return getX();
				break;
			}
			
			case 1: {
					return getY();
				break;
			}
			
			case 2: {
					return getZ();
				break;
			}
		}
	}
	
	private:
		double x;
		double y;
		double z;
};

#endif