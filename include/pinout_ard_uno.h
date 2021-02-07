
/**** PINES PARA EL CONTROL DE LOS MOTORES  ***/

// motor de cenit 
#define MOTOR_1_S1  5
#define MOTOR_1_S2  6
// motor de azimuth 
#define MOTOR_2_S1  9 
#define MOTOR_2_S2  10 

/* PINES ETHERNET */
#define PINSS 4
#define PINRESET 3  

/* PINES ENCODERS*/
#define PINENCODERAZ A0   //MEDIDA DE AZIMUTH 
#define PINENCODERH  A1   //MEDIDA DE ALTURA 

// flags para utilizar depuracioon 
#define TIMER_CLOCKS 0 // para depurar las aplicaciones sin timers establecidos  
#define DEBUG  2      // depuración de aplicaciones usando puerto serie .  

// columnas - filas y direccion display LCD I2C 
#define COLS_LCD 16 
#define FILAS_LCD 2 
#define ADRRESS_LCD_I2C 0x3F 
