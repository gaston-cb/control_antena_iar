
/**** PINES PARA EL CONTROL DE LOS MOTORES  ***/

// motor de cenit 
#define MOTOR_1_S1  9
#define MOTOR_1_S2  10
// motor de azimuth 
#define MOTOR_2_S1  5 
#define MOTOR_2_S2  6 

/* PINES ETHERNET */
#define PINSS 4
#define PINRESET 3  

/* PINES ENCODERS*/
#define PINENCODERAZ A0   //MEDIDA DE AZIMUTH 
#define PINENCODERH  A1   //MEDIDA DE ALTURA 

// flags para utilizar depuracioon 
#define TIMER_CLOCKS 0 // para depurar las aplicaciones sin timers establecidos  
#define DEBUG  1      // depuración de aplicaciones usando puerto serie .  
