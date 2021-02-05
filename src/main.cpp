#include <Arduino.h>
#include "../include/control_motores/control_motores.h"

#include "pinout_ard_uno.h" 
#include "../include/lecturas_encoders/lectura_encoders.h"




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

  // posicion cenit 
 

     


  // inicializacion de ethernet 

  //inicializacion de display LCD 




}

void loop() 
{
  int ref1 = 9000 ; 
  int ref2 = 9000 ; 
  delay(1000) ; 
  leer_encoders() ; 
  control_motores(ref1,ref2) ;
}