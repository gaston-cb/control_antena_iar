#include <Arduino.h>
#include "../include/control_motores/control_motores.h"

#include "pinout_ard_uno.h" 
#include "../include/lecturas_encoders/lectura_encoders.h"
#include <Ethernet.h>
#include "tiempo.h"
#include <LiquidCrystal_I2C.h> 
#define ETHERNET_IP_LOCAL 0 //0 -> IP POR DHCP 
#define PORT_GPREDICT 4533 
#define PORT_STELLARIUM 10000
#define NUMBER_CLOCK 0 
#define HOUR_CLOCK  0 
#define MINUTE_CLOCK  0 
#define SEGUNDO_CLOCK  0 
#define MILISECOND_CLOCK  10 




                            //1 -> ip ASIGNADA POR EL PROGRAMADADOR 
#if ETHERNET_IP_LOCAL      //if  ETHERNET_IP_LOCAL ==1 
  byte ip [] = {127,0,0,0}
  byte mask_net[] ={127,0,0,0} ; 
  byte gateway [] ={127,0,0,0} ; 
  byte DNS [] ={127,0,0,0}     ; 
#endif
// prototipo de funciones 
void Asignar_Azimuth_Declinacion(EthernetClient &ptr, String &cad_leida); 
String az_dec_pos_actual();



int mover_posicion_az = 9000 ; 
int mover_posicion_h = 9000 ; 

EthernetServer stellarium(PORT_STELLARIUM) ;
unsigned long int RA12HS = 0x80000000  ; // 4bytes  
long int DEC10 = 0x40000000  ; //4 bytes 

EthernetServer Gpredict(PORT_GPREDICT) ; 
LiquidCrystal_I2C lcd (ADRRESS_LCD_I2C,COLS_LCD,FILAS_LCD) ; 

// variables externas 
extern int azimuth ; 
extern int altura ;



void setup() 
{

  // debugger -- 
  Serial.begin(9600) ; 
  /**** inicializacion de puertos del motor como salida  *****/
  init_pins_motores() ; 
  autocalibracion()  ;
  //inicializacion de LCD  
  lcd.init() ; 
  lcd.backlight() ; 
  //lcd.print("hola mundillo") ;
  
  // inicializacion de ethernet 
  Ethernet.init(PINSS) ; 
  byte mac [] = {0x00, 0xCD, 0xEF, 0xEE, 0xAA, 0xBC}; // dirmac
#if ETHERNET_IP_LOCAL 
  if (Ethernet.begin(mac,ip,DNS,gateway,mask_net) == 0)
  {
   Serial.print(F("Fallo DHCP"));    
  }else 
  {
   Serial.print(Ethernet.localIP());
  }
#else 
  if (Ethernet.begin(mac) == 0)
  {
    lcd.print(F("Fallo DHCP"));    
  }else 
  {
    lcd.print(Ethernet.localIP());
  }
#endif
  // inicio scheduler -- > 
 Base_tiempo() ; 
TimerStart(NUMBER_CLOCK,HOUR_CLOCK,MINUTE_CLOCK,SEGUNDO_CLOCK,MILISECOND_CLOCK) ; 
}

void loop() 
{
  timerEvent() ; 
  EthernetClient cliente_gpr = Gpredict.available() ;
  EthernetClient cliente_s = stellarium.available() ; 
  long int dec ; 
  unsigned long int RA = 0 ; 
  uint8_t sunP[20] ; // vector datos recibidos -- sunposition 
  char state_con = 0 ; 
  
  
  
  if (cliente_gpr)
  { 
    while (cliente_gpr.connected())
    { 
      timerEvent() ;
      String cadena = "" ;      
      if (cliente_gpr.available())
      {
        //timerEvent() ;  
        char c = cliente_gpr.read()  ;   
        if (c == 'P')
        {
          
           cliente_gpr.print("RPRT 0") ; // lectura correcta 
           Asignar_Azimuth_Declinacion(cliente_gpr,cadena) ;             
           cliente_gpr.flush() ;// VACIADO DE BUFFER 
           cadena ="" ;   
        }else if (c == 'p')
        {      
          cadena = az_dec_pos_actual() ;       
          cliente_gpr.print(cadena);
          cliente_gpr.flush() ; 
          cadena ="" ;     
        }else if (c=='q' || c=='Q' )
        {                
          cliente_gpr.flush() ; 
          cliente_gpr.stop()  ;
          cadena ="" ; 
          break ;          
        }else if (c=='S')
        {
          cliente_gpr.flush() ;  
          cliente_gpr.stop() ;
          cadena = "" ; 
        } 
     }
  }
  cliente_gpr.stop() ; 
  delay(10) ; 
  mover_posicion_az = 9000 ; 
  mover_posicion_h= 9000 ; 
 }
  
 if (cliente_s)
 {  
   while (cliente_s.connected())
   {
    timerEvent() ; 
    
    char i = 0 ; 
    while (cliente_s.available())
    {
      //Serial.println(cliente_s.read(),HEX) ; 
      sunP[i] = cliente_s.read() ;  
      i = i + 1 ;          
      state_con = 1 ; 
    }
    if(state_con == 1)
    {
      dec = 0x00000000 | (long (sunP[19])<<24) | (long (sunP[18])<<16) | (long (sunP[17])<<8) | (long (sunP[16])<<0);
      RA  = 0x00000000 | (long (sunP[15])<<24) | (long (sunP[14])<<16) | (long (sunP[13])<<8) | (long (sunP[12])<<0);
      state_con = 0 ; 
      // declinacion en angulo 
      //ra en angulo 
      // RA12HS -- 12hs 
      unsigned long int hsra = (((RA*12.0)/RA12HS) * 10000)*15   ;
      float hsrf  = ((RA*12.0)/RA12HS)*15 ;      
      float declinacion = (90.0/DEC10)*dec ;  
      
      Serial.print("declinacion: ") ; Serial.println(declinacion) ; 
      Serial.print("ascencion recta: ") ; Serial.println(hsrf) ; 
      Serial.print("ascencion recta entera: ") ; Serial.println(hsra) ; 
      cliente_s.write(sunP,20) ;   
    }
     
   }
   cliente_s.stop() ; 
   delay(10) ; 
   // change ref1 and ref2  in future part of the project 
 }
 













}




void Asignar_Azimuth_Declinacion(EthernetClient &ptr, String &cad_leida)
{
  /*
   * Esta funcion esta destinada a recoger los datos del 
   * Gpredict y guardarlos en una variable para poder 
   * global para poder compararlos. Se realiza una conversion a valor 
   * entero y se comparan entre ellos para obtener la posicion actual. Si son 
   * coincidentes se detiene el motor, si no, se gira a la derecha o izquierda segun 
   * el caso
   * 
  */
float az = 0  ;
float alt = 0  ;
ptr.read(); //espacio en blanco  
while(ptr.available())
{ 
   char c = ptr.read() ; 
   cad_leida+= c==','?'.':c;   
}


az = (cad_leida.substring(0,cad_leida.indexOf(' '))).toFloat();
alt = (cad_leida.substring(cad_leida.indexOf(' '))).toFloat(); 

//conversion a valor entero 
mover_posicion_az =  27000 - (az*100) ; 
mover_posicion_h =  (alt*100) ;  


}


String az_dec_pos_actual()
{
  Serial.print("azimuth: "); Serial.println(azimuth) ; 

  return String(String(270.00 - azimuth/100.0) +'\n'+String(altura/100.0))+'\n' ; 
}