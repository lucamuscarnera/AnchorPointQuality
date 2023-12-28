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
		SimplePlot() : y_lim_set(false), x_lim_set(false) {};
		
		void addPoint(float x, float y, std::string symbol = "<>")
		{
			points.push_back( Point2D(x,y,symbol) );
		}
		
		void set_xlabel(std::string label)
		{
			xlabel = label;
		}
		
		void set_title(std::string label)
		{
			title = label;
		}
		

		void set_ylabel(std::string label)
		{
			ylabel = label;
		}		
		
		void set_xlim(float from,float to) {
			x_lim_set = true;
			x_lim_from = from;
			x_lim_to   = to;
		}

		void set_ylim(float from,float to) {
			y_lim_set = true;
			y_lim_from = from;
			y_lim_to   = to;
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
			
			
			if( x_lim_set )
			{
				x_min = x_lim_from;
				x_max = x_lim_to;
			}
			if( y_lim_set )
			{
				y_min = y_lim_from;
				y_max = y_lim_to;
			}
			
			std::vector<std::string> symbols;
			
			char count = 0;
			for(auto & p: points)
			{
				float x = p.getX();
				float y = p.getY();
				
				if( (x_min <= x) && (x <= x_max)  )
				{		
					if( (y_min <= y) && (y <= y_max) )
					{
						int i = (x - x_min)/(x_max - x_min) * (W - 1);
						int j = (H - 1) - (y - y_min)/(y_max - y_min) * (H - 1);	
						symbols.push_back(p.getSymbol());
						screen[i][j] = count;
					}
				}
				count++;
			}
			
			std::cout << std::endl << "\t\t";
			for(int i = 0;i < W;i++)
			{
				if(  (i >= (int(W/2) - (title.size()/2)))  && (i <= (int(W/2) + (title.size()/2))) )
				{
					std::cout << title[(i - (int(W/2) - (title.size()/2)))] <<" ";
				} 
				else 
				{
					std::cout << "  ";
				}
				
			}
			std::cout << std::endl;
			
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
					if( screen[i][j] == 0)
						std::cout << "  ";
					else
					{
						if(!showid)
							std::cout << "<>";
						else
						{
							int count = screen[i][j];
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
		std::string title;
		
		float x_lim_from;
		float y_lim_from;
		float x_lim_to;
		float y_lim_to;
		
		bool x_lim_set;
		bool y_lim_set;
};

#endif