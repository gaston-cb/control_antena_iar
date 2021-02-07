#include <Arduino.h> 
#include "../pinout_ard_uno.h"
#include "../lecturas_encoders/lectura_encoders.h"

#define MOTOR_AZ 1 
#define MOTOR_H 2 

#define PRIMER_SENTIDO_CAL_AZ 1  
#define SEGUNDO_SENTIDO_CAL_AZ 2  
#define PRIMER_SENTIDO_CAL_H 1  
#define SEGUNDO_SENTIDO_CAL_H 2  
#define STOP_MOTOR_1_2 0 

//------------------------ funciones locales en este archivo----------------------------//  

void mover_antena(char _sentido_giro_az, char _sentido_giro_h) ; 
void assign_value_autocal() ; 
void function_compare_autocalibracion() ; 
void assignar_sentidos_motores() ; 


//--------------------------------------------------------------------------------------// 
extern int azimuth ; 
extern int altura ; 


// variables locales al archivo  
int ult4ad[4] ;  // ultimos 4 valores del ad0 y ad1 respectivamente  
/*
 * ult4ad[0] = primer_valor_AD_A0   
 * ult4ad[1] = segundo_valor_AD_A0
 * ult4ad[2] = primer_valor_AD_A1  
 * ult4ad[3] = segundo_valor_AD_A1
*/

int calibracion_encoders[4] ; 
/*
 * calibracion_encoders[0] : primer valor lectura encoders  -- lectura azimuth  
 * calibracion_encoders[1] : segundo valor lectura encoders -- lectura azimuth 
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
 
char movimiento_motor_1[2] ; 
/*
 * motor de azimut 
 * movimiento_motor_1[0]: sentido  oeste - este 
 * movimiento_motor_1[1]: sentido  este  - oeste 
*/
char movimiento_motor_2[2] ; 
/*
 * motor de altura 
 * movimiento_motor_2[0]: sentido   
 * movimiento_motor_2[1]: sentido 
*/

int resH  ; 
int resAz  ; 




enum _state_antena   
{
    AUTOCAL ,  
    NO_AUTOCAL ,  
} antena ; 


struct boolean_ad
{
    unsigned char max_az :1 ;
    unsigned char min_az :1 ; 
    unsigned char min_h  :1 ; 
    unsigned char max_h  :1 ;     
}min_max={0,0,0,0}   ; 


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
    delay(1000) ; 
    function_compare_autocalibracion() ; 
    while (estado_autocalibracion[0]!=0 || estado_autocalibracion[1]!= 0)
    {
            delay(1000) ;     
            function_compare_autocalibracion() ;             
    }
    //ASIGNACIÓN DE MOTORES 
    assignar_sentidos_motores() ; 
    antena = NO_AUTOCAL ; 
    #if DEBUG==1
    // depuración por puerto serie . 
        Serial.print("calibracion encoders az: ") ; Serial.print(calibracion_encoders[0]);Serial.print(" ");  Serial.println(calibracion_encoders[1]) ;
        Serial.print("calibracion encoders h: ") ; Serial.print(calibracion_encoders[2]);Serial.print(" "); Serial.println(calibracion_encoders[3]) ;
        Serial.print("movimiento_motor_1: ") ; Serial.print(movimiento_motor_1[0],DEC) ;Serial.print(" ");  Serial.println(movimiento_motor_1[1],DEC) ; 
        Serial.print("movimiento_motor_2: ") ; Serial.print(movimiento_motor_2[0],DEC) ; Serial.print(" "); Serial.print(movimiento_motor_2[1],DEC) ; 
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
    // estado_autocalibracion[] ={ 1,1}  ; 
    leer_encoders() ; 
    ult4ad[1] = azimuth ; 
    ult4ad[3] = altura  ;
    
    
    char flags_status = 'x' ;  
    //motor az ;  motor altura -- flag = 0x33 ambos en estado estacionario 
    //flags que sirven para avisar que se llego a un estado limite dentro de 
    //las comparaciones 


     // motor de azimut  
    if (ult4ad[1]<100  && min_max.min_az == 0 )
    {
        min_max.min_az = (abs(ult4ad[0]-ult4ad[1])<10)?1:0 ;  
        flags_status = (min_max.min_az==1)?1:'x';  

    }else if(ult4ad[1]>800 && min_max.max_az == 0 )
    {
        //min_max.max_az = 1 ; 
        min_max.max_az = (abs(ult4ad[0]-ult4ad[1])<10)?1:0 ; 
        flags_status = (min_max.max_az==1)?1:'x'; 
    } 



    // motor de altura 

    if (ult4ad[3]<100 && min_max.min_h == 0)
    {
        min_max.min_h = (abs(ult4ad[3]-ult4ad[2])<10)?1:0 ;  
        if (flags_status==1)
        {
            flags_status = (min_max.min_h==1) ?0x33:1;  
        }else 
        {
           flags_status = (min_max.min_h==1) ?2:'x';  
        }
    }else if(ult4ad[3]>800 && min_max.max_h == 0)
    {
        min_max.max_h = (abs(ult4ad[3]-ult4ad[2])<10)?1:0 ;  
        if (flags_status==1)
        {
            flags_status = (min_max.max_h==1) ?0x33:1;  
        }else 
        {
           flags_status = (min_max.max_h==1) ?2:'x';  
        }        
    }

    ult4ad[0] = ult4ad[1] ; 
    ult4ad[2] = ult4ad[3] ; 
        
    switch (flags_status)
    {
     case 1:
        if(estado_autocalibracion[0]==1)
        {
            calibracion_encoders[0] = ult4ad[0] ; 
            estado_autocalibracion[0]=2 ; 
            mover_antena(MOTOR_AZ,SEGUNDO_SENTIDO_CAL_AZ) ;
        }else if(estado_autocalibracion[0]==2) 
        {
            calibracion_encoders[1] = ult4ad[0] ; 
            estado_autocalibracion[0]=0 ; 
            mover_antena(MOTOR_AZ,STOP_MOTOR_1_2) ; 
        }
        break ; 
    case 2:
        if(estado_autocalibracion[1]==1)
        {
//            Serial.print("est_cal = 1 ") ; 
            calibracion_encoders[2] = ult4ad[2] ; 
            estado_autocalibracion[1] = 2 ; 
            mover_antena(MOTOR_H,SEGUNDO_SENTIDO_CAL_AZ) ; 
        }else if(estado_autocalibracion[1]==2)
        {
            calibracion_encoders[3] = ult4ad[2] ;
  //          Serial.print("est_cal = 2 ") ;
            estado_autocalibracion[1] = 0 ; 
            mover_antena(MOTOR_H,STOP_MOTOR_1_2) ; 
        } 
        break ;
    case 0x33: 
        if (estado_autocalibracion[0]==1 && estado_autocalibracion[1]==1)
        {
            calibracion_encoders[0] = ult4ad[0] ; 
            calibracion_encoders[2] = ult4ad[2] ; 
            estado_autocalibracion[0] = 2 ; 
            estado_autocalibracion[1] = 2 ; 
            mover_antena(MOTOR_AZ,SEGUNDO_SENTIDO_CAL_AZ) ; 
            mover_antena(MOTOR_H,SEGUNDO_SENTIDO_CAL_H)   ; 

        }else if(estado_autocalibracion[0]==2 && estado_autocalibracion[1]==2)
        {
            calibracion_encoders[1] = ult4ad[0] ; 
            calibracion_encoders[3] = ult4ad[2] ;     
            estado_autocalibracion[0] = 0 ; 
            estado_autocalibracion[1] = 0 ; 
            mover_antena(MOTOR_AZ,STOP_MOTOR_1_2) ; 
            mover_antena(MOTOR_H,STOP_MOTOR_1_2)   ; 
        }
    } 
    


} 


void assignar_sentidos_motores()
{
    // motor de azimuth  (cal_enc[0]-->primer sentido )
    /*
     * calibracion_encoders[0] < calibracion_encoders[1] --> sentido = 1 --> sentido este - oeste  
     * calibracion_encoders[0] > calibracion_encoders[1] --> sentido = 1 --> sentido oeste - oeste  
     * 
    */
    int aux ; 
    #if DEBUG==1
        Serial.println("asig sent motor") ;  
        Serial.print("calibracion encoders az: ") ; Serial.print(calibracion_encoders[0]);Serial.print(" ");  Serial.println(calibracion_encoders[1]) ;
        Serial.print("calibracion encoders h: ") ; Serial.print(calibracion_encoders[2]);Serial.print(" "); Serial.println(calibracion_encoders[3]) ;         
        Serial.println("end assig ") ; 
    #endif
    if(calibracion_encoders[0] < calibracion_encoders[1])
    {
      movimiento_motor_1[0] = SEGUNDO_SENTIDO_CAL_AZ ; 
      movimiento_motor_1[1] = PRIMER_SENTIDO_CAL_AZ;
      
    }else 
    {
      movimiento_motor_1[0] = PRIMER_SENTIDO_CAL_AZ ; 
      movimiento_motor_1[1] = SEGUNDO_SENTIDO_CAL_AZ;
      
      aux = calibracion_encoders[0]   ; 
      calibracion_encoders[0] = calibracion_encoders[1] ; 
      calibracion_encoders[1] = aux ;   
    }

    if(calibracion_encoders[2] < calibracion_encoders[3])
    {
      movimiento_motor_2[0] = PRIMER_SENTIDO_CAL_H; 
      movimiento_motor_2[1] = SEGUNDO_SENTIDO_CAL_H; 
    }else 
    {

      movimiento_motor_2[1] = PRIMER_SENTIDO_CAL_H; 
      movimiento_motor_2[0] = SEGUNDO_SENTIDO_CAL_H; 
      aux = calibracion_encoders[2]   ; 
      calibracion_encoders[2] = calibracion_encoders[3] ; 
      calibracion_encoders[3] = aux ;   
    }
    resH = abs((100*((90.0)/(calibracion_encoders[2] - calibracion_encoders[3])))); 
    resAz = (100*((180.0 )/(calibracion_encoders[1]- calibracion_encoders[0]))); 



}




void control_motores(int ref_1,int ref_2)
{
    int error_h  = ref_2 - altura ; 
    int error_az = ref_1 - azimuth  ; 
    #if DEBUG == 1 
        Serial.print("resoluciónH: ") ; Serial.println(abs(resH)) ; 
        Serial.print("resoluciónZ: ") ; Serial.println(resAz) ; 
        Serial.print("error alt: ") ; Serial.println(error_h) ; 
        Serial.print("error az: ") ; Serial.println(error_az) ; 
    #endif

    // control ON - OFF azimut 
    if(abs(error_az)<resAz)
    {
        mover_antena(MOTOR_AZ,STOP_MOTOR_1_2) ; 
    }else if(error_az>resAz)
    {
        mover_antena(MOTOR_AZ,movimiento_motor_1[0]) ; 
    }else if(error_az<-resAz)
    {
        mover_antena(MOTOR_AZ,movimiento_motor_1[1]) ; 
    } 

    // control ON - OFF altura
    
    if(abs(error_h)<resH)
    {
        mover_antena(MOTOR_H,STOP_MOTOR_1_2) ; 
    }else if(error_h>resH)
    {
        mover_antena(MOTOR_H,movimiento_motor_2[0]) ;     
    }else if(error_h<-resH)
    { 
        mover_antena(MOTOR_H,movimiento_motor_2[1]) ; 
    } 
}
