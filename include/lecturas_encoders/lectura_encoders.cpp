#include "Arduino.h" 
#include "../pinout_ard_uno.h"


int azimuth ; 
int altura ;
extern enum _state_antena 
{
    AUTOCAL ,  
    NO_AUTOCAL, 
} antena ;


 // extern enum  
void leer_encoders()
{
    azimuth = analogRead(PINENCODERAZ) ; 
    altura  = analogRead(PINENCODERH) ; 
    if (antena==AUTOCAL) 
    {
      return  ; 
    }
    //transformacion de coordenadax 



}


