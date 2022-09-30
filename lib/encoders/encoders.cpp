#include <Arduino.h> 
#define I2C_ADDRESS_AZIMUTH //0B0000
#define I2C_ADDRESS_H       //0B0000 


typedef struct 
{
    float angle_az ; 
    float angle_h ; 
}position_t;

typedef struct{
    uint8_t address ; 
    //bytes_to_sent ///receive address  
}protocol_i2c_t ; 


protocol_i2c_t encoder_az  ; 
protocol_i2c_t encoder_h  ; 
static volatile unsigned long int count_time = 0 ; 
static position_t position_antenna ; 
static volatile uint8_t isr_flag = 0 ;  
void handle_port(void) ; 
void initEncoders(){ 
    pinMode(2,INPUT)  ; 
    attachInterrupt(digitalPinToInterrupt(2),handle_port,RISING) ; 

}


void loop_test(){ 
    if(isr_flag == 1){
        isr_flag = 0 ; 
        Serial.println("one_second") ; 
    }


}


void getPosition(float *az, float *h){ 
    *az  = position_antenna.angle_az ; 
    *h   = position_antenna.angle_h ; 
}



void handle_port(void){ 
    count_time++ ; 
    isr_flag = 1 ; 
}

//leer_posicion  
//enviar referencia 
// 