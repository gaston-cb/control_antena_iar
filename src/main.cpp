#include <Arduino.h>
#include "../include/control_motores/control_motores.h"

#include "pinout_ard_uno.h" 

void setup() 
{
  /**** inicializacion de puertos del motor como salida  *****/
  //pinMode(MOTOR_1_S1,OUTPUT) ; 
  //pinMode(MOTOR_1_S2,OUTPUT) ;
  //pinMode(MOTOR_2_S1,OUTPUT) ;
 // pinMode(MOTOR_2_S2,OUTPUT) ;
  //autocalibracion() ; 
  // calibracion de antena 
  init_pins_motores() ; 
  autocalibracion()  ; //; -- se mantiene en 

  // inicializacion de ethernet 

  //inicializacion de display LCD 




}

void loop() 
{

// timerEvent 

}