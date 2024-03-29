#ifndef NUMPY_ARRAY_H
#define NUMPY_ARRAY_H

/* Struttura dati elementare per leggere un numpy array da file npy
 * e interagirci usando un getter. Usato dalla classe voxelgrid
 * per leggere la voxelgrid da file
 * 
 */

#include <vector>
#include <string>
#include <fstream>

/*
N U M P Y { t i p o = f 8 , . . . , s h a p e = ( s h a p e X, s h a p e Y , s h a p e Z ) } \x10
byte1 byte2 ... byte_sizeof(tipo)*N
*/

template
<typename DataType>
class NumpyArray
{
	public:
		NumpyArray() {};
		NumpyArray(std::string path)
		{
			// leggo il file 
			char ch;
			char curr_datum[sizeof(DataType)];
			int  datum_counter = 0;
			
			bool leggi_shape = false;
			bool leggi_dati  = false;
			bool aspetta_newline = false;
			std::string curr_shape_dim("");
			
			
			std::fstream fin(path, std::fstream::in);
			while (!fin.eof()) {
				if(!leggi_dati) {
					fin >> ch;
					if(ch == '(') {
						//std::cout << "INIZIO SHAPE" << std::endl;
						leggi_shape = true;
					} 
					else
					{
						if(ch == '}')
						{
							//std::cout << "INIZIO DATI" << std::endl;
							leggi_dati = true;
							aspetta_newline = true;
						}
						else 
						{
							if(leggi_shape == true)
							{
								if(ch == ',' || ch == ')')
								{
									if(curr_shape_dim.size() > 0)
										shape.push_back(std::stoi(curr_shape_dim));
									curr_shape_dim = "";
									if(ch == ')')
									{
										leggi_shape = false;
										//std::cout << "shape = (";
										int shapeprod = 1;
										for(auto & s : shape)
										{
											//std::cout <<  s << " ";
											shapeprod *= s;
										}
										//std::cout << ")" << std::endl;
										rawdata.clear();
										rawdata.reserve(shapeprod);
									}
								}
								else 
								{
									curr_shape_dim = curr_shape_dim + ch;
								}
							}
						}
					}
				}
				else
				{ 	
					if(aspetta_newline)
						while(ch != 10)
							fin >> std::noskipws >> ch;
						aspetta_newline = false;
					
					fin >> std::noskipws >> ch;
					//std::cout << "["<<(unsigned int)ch<<"]\t";

					curr_datum[datum_counter] = (char) ch;
					datum_counter++;
					if(datum_counter == sizeof(DataType))
					{
						DataType val = *((DataType*) curr_datum);
						rawdata.push_back(val);
						//std::cout << val << std::endl;
						datum_counter = 0;
					}
				}
				////std::cout << ch <<","<< (int)ch  << "\n"; // Or whatever
			};
		}
		
		template <class ... Ts>
		DataType get(Ts && ... inputs)
		{
			int i = 0;
			int indice = 0;
			
			
			([&]
			{
				size_t idx = inputs;
				size_t offset = 1;
				for(int j = i + 1; j < shape.size() + 1;j++)
				{
					int curr = 0;
					if(j == shape.size())
					{
						curr = 1;
					}
					else
					{
						curr = shape[j];
					}
					offset *= curr;
				}
				indice += idx * offset;				
				////std::cout << inputs << " su " << shape[i] << std::endl;
				i++;
			} (), ...);
			//std::cout << "indice richiesto = " << indice << " lunghezza = " << rawdata.size() << std::endl;
			return rawdata[indice];
		}
		
		DataType operator[](int i)
		{
			// solo per dati unidimensionale
			return rawdata[i];
		}
		
	std::vector<int  > shape;
	std::vector<DataType> rawdata;
};

#endif