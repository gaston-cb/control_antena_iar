#include <Arduino.h> 
#include "../pinout_ard_uno.h"

void mover_antena(char _sentido_giro_az, char _sentido_giro_h) ; 

void init_pins_motores()
{
    pinMode(MOTOR_1_S1,OUTPUT) ; 
    pinMode(MOTOR_2_S2,OUTPUT) ;
    pinMode(MOTOR_2_S1,OUTPUT) ;
    pinMode(MOTOR_2_S2,OUTPUT) ;
}





void autocalibracion()
{






}



void mover_antena(char _sentido_giro_az, char _sentido_giro_h)
{
 /*
  *
  *  pines 5 y 6 ---> eje de azimut 
  * _sentido_giro_az =  1 --> pin 5 alto /pin 6 bajo 
  * _sentido_giro_az =  2 --> pin 5 bajo /pin 6 alto  
  * _sentido_giro_az =  0 --> pin 5 bajo /pin 6 bajo 
  * 
  * pines 9 y 10 --> eje de cenith 
  * _sentido_giro_h =  1 --> pin 9 alto /pin 10 bajo 
  * _sentido_giro_h =  2 --> pin 9 bajo /pin 10 alto 
  * _sentido_giro_h =  0 --> pin 9 bajo /pin 10 bajo  
  *  
 */






}