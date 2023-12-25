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
	
	double getX() {return x;}
	double getY() {return y;}
	double getZ() {return z;}
	
	private:
		double x;
		double y;
		double z;
};

#endif