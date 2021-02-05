#include "Arduino.h" 
#include "../pinout_ard_uno.h"


int azimuth ; 
int altura ;
extern enum _state_antena 
{
    AUTOCAL ,  
    NO_AUTOCAL, 
} antena ;

// variables externas --- Función de autocalibracion  
extern int calibracion_encoders[4] ; 





void leer_encoders()
{
    // int scope_h  ; 
    // int scope_az ; 
    
    int lect_az = analogRead(PINENCODERAZ) ; 
    int lect_h  = analogRead(PINENCODERH) ; 
    if (antena==AUTOCAL) 
    {
      altura = lect_h ; 
      azimuth = lect_az; 
      return  ; 
    }
    // transformación de coordenadas 
    
    altura  = (100*((90.0)/(calibracion_encoders[2] - calibracion_encoders[3]))) *(lect_h - calibracion_encoders[3]) ; 
    azimuth = (100*((180.0 )/(calibracion_encoders[1]- calibracion_encoders[0]))) *(lect_az - calibracion_encoders[0]) ; 
  #if DEBUG==1
    Serial.print("altura: ") ; Serial.println(altura) ; 
    Serial.print("az: ") ; Serial.println(azimuth) ; 
    
  #endif  


}


