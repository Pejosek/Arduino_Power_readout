/*
 * Przemysław Cichocki 154812
 * Projekt inżynierski - Bezprzewodowy pomiar energii elektrycznej
 */

//-------------
//Biblioteki: |
//---------------------------------------------------------------------------------------------------------
#include <VirtualWire.h>			          // Biblioteka umożliwia wysylanie i odbior prze RF			          |
#include <LiquidCrystal.h>			        // Bibiloteka sterujaca wyswietlaczem						                  |
//---------------------------------------------------------------------------------------------------------
// initialize the library with the numbers of the interface pins									                        |
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);    // Wybrane odpowiednie piny w keylad LCD shield				            |
//---------------------------------------------------------------------------------------------------------                                                                                                        
//-------------------
//Zmienne globalne: |
//---------------------------------------------------------------------------------------------------------
const float okres = (1 / 360); 		      // Stały przypisany okres pomiarow						                    |
const float cena = 0.57;			          // 57 groszy za 1kWh										                          |
float delta_E = 0;					            // Delta energii											                            |
float energia = 0;      			          // Energia												                                |
float licznik_czasu = 0; 			          // Czas wyrazony w sekundach								                      |
float czas_na_godz = 0;  			          // Czas na godz											                              |
int   lcd_key     = 0;				          // Wykorzystanie klawiszy									                        |
int   adc_key_in  = 0;				          // Wykorzystanie klawiszy									                        |
float Irms = 0;						              // Natężenie pradu										                            |
float natezenie = 0;				            // Odebrana zmianna / prad / trafia do tej zmiennej		          	|
float intnafloat = 0;				            // Zmienna wykorzystana do zamiany natezenia na float			        |	
float napiecie = 0;					            // Napiecie												                                |
float   P = 0;						                // Moc													                                  |
int   P_naint = 0;					            // Wyswietlanie Mocy w int									                      |
float koszt = 0;					              // Ile do zaplaty											                            | 
boolean prawda = false;
float prog = 1;
//---------------------------------------------------------------------------------------------------------
//RF Transmission container																		                                            |
char Sensor1CharMsg[8]; 			              // Tablica dla odbieranych informacji						              |
//																							                                                          |
// Przypisanie wartosci do zmiennych klawiszy: 													                                  |
#define btnRIGHT  0					                //														                                |
#define btnUP     1					                //														                                |
#define btnDOWN   2					                //														                                |
#define btnLEFT   3					                //														                                |
#define btnSELECT 4					                //														                                |
#define btnNONE   5					                //														                                |
//Funkcja do rozpoznawania ktory klawisz zostal nacisniety											                          |
	int read_LCD_buttons(){                   //														                                |
		adc_key_in = analogRead(0);             //														                                |
		                        		            //														                                |
		// For V1.1 us this threshold	          //														                                |
		if (adc_key_in < 50)   return btnRIGHT;	//  											                                    |
		if (adc_key_in < 250)	 return btnUP;		//						                                      	        |
		if (adc_key_in < 450)  return btnDOWN; 	//											                                      |
		if (adc_key_in < 650)  return btnLEFT; 	//											                                      |
		if (adc_key_in < 850)  return btnSELECT;//											                                      |
	}											                    //											                                      |
//Wiemy ktory klawisz zostal nacisniety															                                      |
//---------------------------------------------------------------------------------------------------------

//-------------------------------------------------
// Poczatek programu / rozkazy wykonane tylko raz: |
//---------------------------------------------------------------------------------------------------------
void setup() {						                  //														                                |
	Serial.begin(9600);				                //														                                |
	lcd.begin(16, 2);				                  // Zainicjowanie dysplaya									                    |
	lcd.clear();					                    // Odświerzanie wyswietlacza / czyszczenie					          |
	vw_set_ptt_inverted(true); 		            // Required for DR3100										                    |
	vw_setup(2000);  				                  // Bits per sec 											                        |
	vw_rx_start();  				                  // Zaczynanie odbierannia									                    |
}// END void setup											                                                                  |
//---------------------------------------------------------------------------------------------------------

//--------------------
//Rozpoczecie pętli: |
//---------------------------------------------------------------------------------------------------------
void loop(){							                            //												                          |
	lcd.clear();						                            // Odświerzanie wyswietlacza							          |
	//Warunek, kiedy ma liczyc czas:		                //													                        |
	if((natezenie) > 100)						                    // Jezeli prad > 0.1 A to zaczynamy liczyc				  |
	{									                                  //													                        |
		licznik_czasu++;					                        // Licznik czasu /s									                |
		delta_E = (P / 3600) / 1000;	                    // Delta energii										                |
	} 									                                //													                        |
	else								                                //													                        |
	{									                                  //													                        |
		delta_E = 0;					                            // Zeruje dla pewnosci									            |
	}									                                  //													                        |
  energia = energia + delta_E;
	czas_na_godz = licznik_czasu / 3600;                // Moment przeliczania sekund na godziny					  |
	lcd_key = read_LCD_buttons();		                    // Wyswietlanie na podstawie wcisnietego przycisku  |
	switch (lcd_key){               	                  // depending on which button was pushed             |
		case btnRIGHT:{             	                    // push button "RIGHT" and show the word on the scr |
            lcd.setCursor(0,0);			                  // Ustaw kursor na displayuy [0, 0]						      |
              lcd.print("Cena za 1kWh = ");	          // 												                          |
              lcd.print(cena);			                  // Wyświetl czas										                |
              //lcd.setCursor(14,0);		              // Ustaw kursor										                  |
              //lcd.print("/s");																                                          |
			//																				                                                          |
              lcd.setCursor(0,1);		                  //													                        |
              lcd.print("Koszt = ");	                //													                        |
              lcd.print(koszt,4);		                  // Czas wyswietlam w godiznach i 4 miejsca po				|
              lcd.setCursor(14,1);		                //													                        |
              lcd.print("zl");			                  //													                        |
            break;						                        //													                        |
       }								                              //													                        |
       case btnLEFT:{					                        // Przypadek przycisku LEFT								          |
              lcd.setCursor(0,0);		                  // Ustaw kursor na displayuy [0, 0]						      |
              lcd.print("Czas = ");		                // 													                        | 
              lcd.print(licznik_czasu,0);             // Wyświetl czas										                |
              lcd.setCursor(14,0);		                // Ustaw kursor										                  |	
              lcd.print("/s");			                  //													                        |
				                                              //																			            |
              lcd.setCursor(0,1);		                  //													                        |
              lcd.print("Czas = ");		                //													                        |
              lcd.print(licznik_czasu / 3600 ,4);     // Czas wyswietlam w godiznach i 4 miejsca po	      |
              lcd.setCursor(14,1);		                //													                        |
              lcd.print("/h");		                  	//													                        |
             break;						                        //													                        |
       }    							                            //												                        	|
       case btnUP:{						                        // Przyoadek wcisnietego przycisku UP				      	|
              lcd.setCursor(0,0);		                  //													                        |
              lcd.print("I = ");		                  //													                        |
              intnafloat = natezenie / 1000;         // Dziele przez 10^3 poniewaz nad wysyla wieksza war|
              lcd.print(intnafloat,4);	              // Cztery miejsca po przecinku							        |
              lcd.setCursor(15,0);		                //													                        |
              lcd.print("A");			                    //													                        |
										                                  //													                        |
              lcd.setCursor(0,1);		                  // 													                        |
              lcd.print("V = "); 		                  //													                        |
              lcd.print(napiecie/100);	              //													                        |
              lcd.setCursor(15,1);		                //													                        |
              lcd.print("V");			                    //													                        |
             break;						                        //													                        |
		}								                                  //													                        |
       case btnDOWN:{					                        // Przycisk wcisniecia przycisku DOWN					      |
              lcd.setCursor(0,0);		                  //													                        |
              lcd.print("P = ");		                  //													                        |
              lcd.print(P);		                  //													                        |
              lcd.setCursor(15,0);		                //													                        |
              lcd.print("W");			                    //													                        |
										                                  //													                        |
              lcd.setCursor(0,1);		                  //													                        |
              lcd.print("E = ");		                  //													                        |
              lcd.print(energia, 4);	                //													                        |
              lcd.setCursor(13,1);		                //												                        	|
              lcd.print("kWh");			                  //													                        |
             break;						                        //													                        |
		}								                                  //													                        |
       case btnSELECT:{					                      //													                        |
             lcd.print("SELECT");  		                //  push button "SELECT" and show the word on the sc|
             break;						                        //													                        |
		}								                                  //													                        |
       case btnNONE:{					                        //													                        |
             lcd.print("NONE  ");  		                //  No action  will show "None" on the screen				|
             break;						                        //													                        |
		}								                                  //												                        	|
	}									                                  //													                        |
	//---------------
	//Obliczenia: 	|																	
	//-------------------------------------------------
	Irms 	= (natezenie / 1000);	                        //			                                            |
	P_naint = P;					                              //			                                            |
	energia = energia + delta_E;	                      //			                                            |
	koszt 	= energia * cena;		                        //		                                              |
	//-------------------------------------------------											                                |
	uint8_t buf[VW_MAX_MESSAGE_LEN];	                  // Trzeba sie dowiedziec								            |
	uint8_t buflen = VW_MAX_MESSAGE_LEN;                // Trzeba sie dowiedziec								            |
	                                                    // Non-blocking	Cala operacja odiberania i przypisyw|
	//---------------------------------------------------------------------------------------------------
	if (vw_get_message(buf, &buflen))                 	//													                        |
	{									                                  //													                        |
		int i;							                              //													                        |
			                                                // Message with a good checksum received, dump itp. |
			for (i = 0; i < buflen; i++)                    //													                        |
			{            				                            //													                        |
				                                              // Fill Sensor1CharMsg Char array with corresponding|
				                                              // chars from buffer.   														|
				Sensor1CharMsg[i] = char(buf[i]);             //											                            |
			}									                              //											                            |
       //																					                                                        |
		// Convert Sensor1CharMsg Char array to integer											                                  |
		int X = atoi(strtok((char*)buf, ","));            // Look for a comma, the return the data before 	  |
		int Y = atoi(strtok(NULL, ","));                  // same as above										                |
		int Z = atoi(strtok(NULL, "."));                  // Look for a period, then return data before it		|
		natezenie = X;					                          //													                        |
		napiecie = Y;					                            //													                        |
		P = Z;							                              //													                        |
		                                                  // DEBUG 																			      |
		                                                  //Serial.print("Sensor 1: ");												|	
		                                                  //Serial.println(natezenie);												|
										                                  //												                        	|
		                                                  // END DEBUG																		  	|
										                                  //													                        |
		                                                  // Clear Sensor1CharMsg char array for coming messag|
		                                                  // This needs to be done otherwise problems will occ|
		                                                  // when the incoming messages has less digits than t|
		                                                  // one before. 																		  |
			for (i = 0; i < 8; i++)		                      //													                        |
			{							                                  //													                        |
				Sensor1CharMsg[i] = 0;	                      //													                        |
			}							                                  //												                          |
       delay(1000);						                        //												                        	|
   }									                                //													                        |
   //Serial.println(millis());
  Serial.println(delta_E,7);
  Serial.println(energia,7);
  Serial.println(P);
  Serial.println(natezenie);
  Serial.println(Irms);
  
   //------------------------------------------------------------------------------------------------------
}										                                  //																							    |
//---------------------------------------------------------------------------------------------------------
