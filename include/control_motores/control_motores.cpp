#include <Arduino.h> 
#include "../pinout_ard_uno.h"
#include "../lecturas_encoders/lectura_encoders.h"

#define MOTOR_AZ 1 
#define MOTOR_H 2 

#define PRIMER_SENTIDO_CAL_AZ 1  
#define SEGUNDO_SENTIDO_CAL_AZ 2  
#define PRIMER_SENTIDO_CAL_H 1  
#define SEGUNDO_SENTIDO_CAL_H 2  




void mover_antena(char _sentido_giro_az, char _sentido_giro_h) ; 
void assign_value_autocal() ; 
void function_compare_autocalibracion() ; 


//-----------------------------------------------------------------------------------// 
extern int azimuth ; 
extern int altura ; 

int ult4ad[4] ; 
/*
 * ult4ad[0] = primer_valor_AD_A0   
 * ult4ad[1] = segundo_valor_AD_A0
 * ult4ad[2] = primer_valor_AD_A1  
 * ult4ad[3] = segundo_valor_AD_A1
*/

int calibracion_encoders[4] ; 
/*
 * calibracion_encoders[0]  : primer valor lectura encoders  -- lectura azimuth  
 * calibracion_encoders[1] : segundi valor lectura encoders -- lectura azimuth 
 * calibracion_encoders[2] : primer valor lectura encoders  -- lectura altura
 * calibracion_encoders[3] : segundo valor lectura encoders -- lectura altura
*/

char estado_autocalibracion[2] = {0,0} ;  // esta variable se usa para scheduler en calibracion  
/*
 * estado_autocalibracion[0] --> motor azimuth 
 *                           --> 1 ---> primer sentido 
 *                           --> 2 ---> segundo sentido 
 *                           --> 0 ---> fin calibracion eje azimuth 
 * estado_autocalibracion[1] --> motor de altura 
 *                           --> 1 ---> primer sentido 
 *                           --> 2 ---> segundo sentido 
 *                           --> 0 ---> fin calibracion eje azimuth 
 * 
 * 
*/                           
 


enum _state_antena   
{
    AUTOCAL ,  
    NO_AUTOCAL ,  
} antena ; 

//-------------------------------------------------------------------------------------------//




void init_pins_motores()
{
    pinMode(MOTOR_1_S1,OUTPUT) ; 
    pinMode(MOTOR_1_S2,OUTPUT) ;
    pinMode(MOTOR_2_S1,OUTPUT) ;
    pinMode(MOTOR_2_S2,OUTPUT) ;
}





void autocalibracion()
{
    
    assign_value_autocal() ;
    Serial.println("init_autocalibracion") ; 
    #if TIMER_CLOCKS 
        //using a scheduler function -- 
        // not using delayu 
    #else 
        delay(1000) ; 
        function_compare_autocalibracion() ; 
        while (estado_autocalibracion[0]!=0 || estado_autocalibracion[1]!= 0)
        {
            delay(1000) ;     
            function_compare_autocalibracion() ;             
        }
        Serial.println("fin autocalibracion") ; 

    #endif 
}


// n° motor -- sentido -- 
void mover_antena(char n_motor, char sentido_giro)
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

    if(n_motor == MOTOR_AZ) 
    {
        switch (sentido_giro)
        {
         case 1:
            digitalWrite(MOTOR_1_S1,HIGH) ; 
            digitalWrite(MOTOR_1_S2,LOW) ;
            break;
        case 2: 
            digitalWrite(MOTOR_1_S1,LOW) ; 
            digitalWrite(MOTOR_1_S2,HIGH) ;
            break;
        case 0:
            digitalWrite(MOTOR_1_S1,LOW) ; 
            digitalWrite(MOTOR_1_S2,LOW) ;
            break; 
        }
    }else if(n_motor==MOTOR_H)
    {
        switch (sentido_giro)
        {
         case 1:
            digitalWrite(MOTOR_2_S1,HIGH) ; 
            digitalWrite(MOTOR_2_S2,LOW) ;
            break;
        case 2: 
            digitalWrite(MOTOR_2_S1,LOW) ; 
            digitalWrite(MOTOR_2_S2,HIGH) ;
            break;
        case 0:
            digitalWrite(MOTOR_2_S1,LOW) ; 
            digitalWrite(MOTOR_2_S2,LOW) ;
            break; 
        }

    } 



}

void assign_value_autocal()
{
    leer_encoders() ; 
    ult4ad[0] = azimuth ; 
    ult4ad[2] = altura  ; 
    estado_autocalibracion[0] = 1 ; 
    estado_autocalibracion[1] = 1 ; 
    mover_antena(MOTOR_AZ,PRIMER_SENTIDO_CAL_AZ); 
    mover_antena(MOTOR_H,PRIMER_SENTIDO_CAL_H); 
     
}

void function_compare_autocalibracion()
{
    leer_encoders() ; 
    ult4ad[1] = azimuth ; 
    ult4ad[3] = altura  ;
    char flags_status ='x';   // x indica que no entro por ningun estado de los dos --   
    if (abs(ult4ad[0]-ult4ad[1])<10)   //diferencia de |0.025|mv 
    {
        Serial.println("flag = 1 ") ; 
        flags_status = 1; 
        
        //estado_autocalibracion[0] = 2 ; 
        //mover_antena(MOTOR_AZ,SEGUNDO_SENTIDO_CAL_AZ) ; 
    }
    if (abs(ult4ad[2]-ult4ad[3])<10)   //diferencia de |0.025|mv 
    {  
       Serial.println("flag = 2 ") ;  
       flags_status = 2; 
       //estado_autocalibracion[1] = 2 ; 
      // mover_antena(MOTOR_H,SEGUNDO_SENTIDO_CAL_AZ) ; 
    }
    //flags_status =  ((abs(ult4ad[0]-ult4ad[1])<10) && (abs(ult4ad[2]-ult4ad[3])<10))?3:flags_status ; 
    Serial.print("val AD: ") ; Serial.print(ult4ad[0]) ; Serial.print(" ") ; 
    Serial.print(ult4ad[1]) ; Serial.print(" ") ; 
    Serial.print(ult4ad[2]) ; Serial.print(" ") ; 
    Serial.print(ult4ad[3]) ; Serial.println(" ") ; 

    Serial.print("cal_encoders: ") ; Serial.print(calibracion_encoders[0]) ; Serial.print("  ") ; 
    Serial.println(calibracion_encoders[2]) ; 

    ult4ad[0] = ult4ad[1] ; 
    ult4ad[2] = ult4ad[3] ; 
   
    switch (flags_status)
    {
     case 1:
        Serial.println("case 1") ; 
        if(estado_autocalibracion[0]==1)
        {
            calibracion_encoders[0] = ult4ad[0] ; 
            estado_autocalibracion[0]=2 ; 
            mover_antena(MOTOR_AZ,SEGUNDO_SENTIDO_CAL_AZ) ;

        }else if(abs(calibracion_encoders[0]-ult4ad[0])>500)
        {

            estado_autocalibracion[0]=0 ; 
            mover_antena(MOTOR_AZ,0) ; 
        }
        break ; 
    case 2:
        Serial.println("case 2") ; 
        if(estado_autocalibracion[1]==1)
        {
            calibracion_encoders[2] = ult4ad[2] ; 
            estado_autocalibracion[1] = 2 ; 
            mover_antena(MOTOR_H,SEGUNDO_SENTIDO_CAL_AZ) ; 
        }else if(abs(calibracion_encoders[2]-ult4ad[2])>500)
        {
            estado_autocalibracion[1] = 0 ; 
            mover_antena(MOTOR_H,0) ; 
        } 
        break ; 
    /*case 3: 
        Serial.println("case 3") ; 
        if (estado_autocalibracion[0] == 1 && estado_autocalibracion[1] == 1)
        {
            estado_autocalibracion[0] = 2 ; estado_autocalibracion[1] = 2 ; 
            mover_antena(MOTOR_H,SEGUNDO_SENTIDO_CAL_AZ) ;  
            mover_antena(MOTOR_AZ,SEGUNDO_SENTIDO_CAL_AZ) ;

        }else if (estado_autocalibracion[0] == 2 && estado_autocalibracion[1] == 2)
        {
            estado_autocalibracion[0] = 0 ; estado_autocalibracion[1] = 0; 
            mover_antena(MOTOR_H,0) ; 
            mover_antena(MOTOR_AZ,0) ;
        }
        break ; */  
    } 
    


} 
