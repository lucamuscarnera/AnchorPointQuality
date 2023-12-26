#ifndef SIMPLE_PLOTLIB_H
#define SIMPLE_PLOTLIB_H

#include <string>
#include <iostream>

class Point2D
{
	public:
		Point2D(float x,float y,std::string symbol) : x(x), y(y), symbol(symbol) {}
		float getX() {return x;}
		float getY() {return y;}
		std::string getSymbol() {return symbol;}
		
	private:
		float x;
		float y;
		std::string symbol;
};

class SimplePlot
{
	public:
		SimplePlot() {};
		void addPoint(float x, float y, std::string symbol = "<>")
		{
			points.push_back( Point2D(x,y,symbol) );
		}
		
		void set_xlabel(std::string label)
		{
			xlabel = label;
		}

		void set_ylabel(std::string label)
		{
			ylabel = label;
		}		
		
		void show(bool showid = false)
		{
			// inizializzo lo schermo
			int H = 20;
			int W = 20;
			char screen[H][W];
			
			for(int i = 0;i < W;i++)
				for(int j=0;j < H;j++)
					screen[i][j] = 0;
			
			// trovo x_min,x_max,y_min,y_max
			float x_min = points[0].getX();
			float y_min = points[0].getY();
			float x_max = points[0].getX();
			float y_max = points[0].getY();
			
			for(auto & p : points)
			{
				if(p.getX() < x_min)
					x_min = p.getX();
				else
					if(p.getX() > x_max)
						x_max = p.getX();
					
				if(p.getY() < y_min)
					y_min = p.getY();
				else
					if(p.getY() > y_max)
						y_max = p.getY();				
			}
			
			std::vector<std::string> symbols;
			
			char count = 0;
			for(auto & p: points)
			{
				float x = p.getX();
				float y = p.getY();
				
				int i = (x - x_min)/(x_max - x_min) * (W - 1);
				int j = (H - 1) - (y - y_min)/(y_max - y_min) * (H - 1);	
				symbols.push_back(p.getSymbol());
				screen[i][j] = count;
				count++;
			}
			
			
			std::cout << std::endl << "\t\t";
			std::cout << "  +";
			for(int i = 0;i < W;i++)
			{
				std::cout << "--";
			}
			std::cout  << "+";
			std::cout << std::endl << "\t\t";
			for(int j=0;j < H;j++)
			{
				
				if(  (j >= (int(H/2) - (ylabel.size()/2)))  && (j < (int(H/2) + (ylabel.size()/2))) )
				{
					std::cout << ylabel[(j - (int(H/2) - (ylabel.size()/2)))] <<" ";
				} 
				else 
				{
					std::cout << "  ";
				}
				
				std::cout << "|";
				for(int i = 0;i < W;i++)
				{
					if( screen[(W-1) - i][j] == 0)
						std::cout << "  ";
					else
					{
						if(!showid)
							std::cout << "<>";
						else
						{
							int count = screen[(W-1) - i][j];
							std::cout << (count < 10? " " : "" ) << count;
						}
					}
				}
				std::cout << "|" << std::endl << "\t\t";
			}
			std::cout  << "  +";
			for(int i = 0;i < W;i++)
			{
				std::cout << "--";
			}
			std::cout  << "+";
			std::cout << std::endl << "\t\t";

			for(int i = 0;i < W;i++)
			{
				if(  (i >= (int(W/2) - (xlabel.size()/2)))  && (i <= (int(W/2) + (xlabel.size()/2))) )
				{
					std::cout << xlabel[(i - (int(W/2) - (xlabel.size()/2)))] <<" ";
				} 
				else 
				{
					std::cout << "  ";
				}
				
			}
			std::cout << std::endl;

			
			
		}
	
	private:
		std::vector<Point2D> points;
		std::string xlabel;
		std::string ylabel;
};

#endif