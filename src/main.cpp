#include <Arduino.h>
#include "../include/control_motores/control_motores.h"

#include "pinout_ard_uno.h" 

void setup() 
{

  // debugger -- 
    Serial.begin(9600) ; 

 //  pinMode(10,OUTPUT) ; 
 //  digitalWrite(10,HIGH) ; 
  // inicio scheduler -- > 


  /**** inicializacion de puertos del motor como salida  *****/
  init_pins_motores() ; 
  autocalibracion()  ;

  // inicializacion de ethernet 

  //inicializacion de display LCD 




}

void loop() 
{

// timerEvent 

}