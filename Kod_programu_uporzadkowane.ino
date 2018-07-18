/*
 * Przemysław Cichocki 154812
 * Projekt inżynierski - Bezprzewodowy pomiar energii elektrycznej
 */

//--------------
// Biblioteki: |
//--------------------------------------------------------------------------------------------------------
#include <VirtualWire.h>               // Biblioteka umożliwiająca wysyłanie inf z nad do odb            |
#include "EmonLib.h"                   // Bibliotega umożliwiająca pomiar energii                        |
EnergyMonitor emon1;                   // Create an instance                                             |
//--------------------------------------------------------------------------------------------------------
//----------
//Zmienne: |
//--------------------------------------------------------------------------------------------------------
int P;                                // Moc                                                             |
int natezenie = 0;                    // Natężenie prądu                                                 |
int napiecie = 0;                     // Napięcie                                                        |
float energia = 0;                    // Energia                                                         |
int energia_delta = 0;                // Wysylane musi byc int                                           |
boolean pobiera_prad = false;         // Zmienna pomocnicza do wyznaczania czasu pradu.                  |
unsigned int licznik_czasu = 1;       // Zmienna odpowiedzialna za zliczanie czasu pobierania pradu.     |
char Sensor1CharMsg[8];               // Tablica do wysyłania znaków                                     | 
//--------------------------------------------------------------------------------------------------------
//---------------------------------------
// Program start / jednorazowe komendy: |                                                                
//--------------------------------------------------------------------------------------------------------
void setup()							        //													                                           |
{										              //													                                           |
    Serial.begin(9600);	  				// Debugging only										                                   |
    Serial.println("setup");			// Wypisanie 'setup' ?	 								                               |
    emon1.voltage(5, 238.5, 0.94);// Voltage: input pin, calibration, phase_shift ?234.26?	             |
    emon1.current(1, 30);         // Current: input pin, calibration.						                         |
										              //													                                           |
                                  // Initialise the IO and ISR																           |
                                  // vw_set_ptt_inverted(true); 			                                   |
    vw_setup(2000);	 					    // Bits per sec / Częstotliwosc nadawania				                       |
}										              //													                                           |
//--------------------------------------------------------------------------------------------------------
//-----------------------------------------------------
// Początek pętli / program bd wykonywał się w pętli: |
//--------------------------------------------------------------------------------------------------------
void loop()								            //													                                       |
{										                  //													                                       |
	float Irms = emon1.Irms;            // Obliczenie Irms za pomocą biblioteki emon1				               |
	float Vrms = emon1.Vrms;	          // Obliczenie Vrms za pomoca biblioteki emon1				               |
	emon1.calcVI(20,2000);    		      // Calculate all. No.of half wavelengths (crossings), time-out	   |
	P = Irms * Vrms;					          // Moc wylicznona z napiecia i natezenia					                 |
	natezenie = Irms * 1000; 		      // Robie z zmiennej float inta, aby moc wysłac w postaci char	     |
    napiecie  = Vrms * 100;        		// Napiecie Vrms zwiekszam, aby zrobic float w odbiorniku	         |
	                                    //funkcja, która umożliwia mi wysłanie więcej niż jedną zmienna		 |
  sprintf(Sensor1CharMsg, "%d, %d ,%d.",natezenie, napiecie, P);//						                           |
  //Serial.println(napiecie);
  Serial.println(Irms);
  Serial.println(Vrms);
	                                    //																						                     |
	// Poczatek wysylania wiadomosci															                                         |
		vw_send((uint8_t *)Sensor1CharMsg, strlen(Sensor1CharMsg));//						                             |
		vw_wait_tx(); // Wait until the whole message is gone									                               |
	// Koniec wysylania wiadomości																                                         |
    delay(665);							// Opoznienie ustawione na podstawie testow				                           |
}										                  //												                                         |
//--------------------------------------------------------------------------------------------------------

