#ifndef FINGER_PRINT_DECORATOR_H
#define FINGER_PRINT_DECORATOR_H


/**********************************************************************************************************
 *  Classe di porting per le fingerprint.
 *  Immaginiamo che l'utente finale abbia un oggetto F che svolga il ruolo di finger print.
 *  In linea di principio, sarebbe necessario scrivere tutti i metodi che utilizziamo in modo
 *  da renderli compatibili con F. Per evitare di dover incorrere in questa necessitá costruiamo
 *  una classe astratta FingerPrintDecorator che 
 *	Questa classe fornisce lo scheletro per costruire uno scheletro che permetta al
 *  nostro sistema di interagire con tale fingerprint.
 ***********************************************************************************************************/

template<typename T,typename format>
class FingerPrintDecorator
{
	public:
		FingerPrintDecorator( 
		                T & fingerprint
					  ) : 
					    fingerprint(fingerprint)
					   {}											// costruttore
					   
		const virtual format operator[](int i) = 0;		  			// funzione puramente virtuale per ottenere un elemento
		const virtual size_t size() = 0;
		
		template<typename vectorialObject>
		double dot(vectorialObject & other)
		{
			double ret = 0.;
			for(int i = 0; i < size(); i++)
			{
				ret += (operator[](i)) * other[i];
			}
			return ret;
		}
		
		
	protected:	
		T & fingerprint;												// reference alla voxelgrid passata al costruttore
};

#endif