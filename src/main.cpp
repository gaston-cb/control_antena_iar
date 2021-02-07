#include <Arduino.h>
#include "../include/control_motores/control_motores.h"

#include "pinout_ard_uno.h" 
#include "../include/lecturas_encoders/lectura_encoders.h"
#include <Ethernet.h>
#include "tiempo.h"
#include <LiquidCrystal_I2C.h> 
#define ETHERNET_IP_LOCAL 0 //0 -> IP POR DHCP 
#define PORT_GPREDICT 4533 
#define PORT_STELLARIUM 4533 

                            //1 -> ip ASIGNADA POR EL PROGRAMADADOR 
#if ETHERNET_IP_LOCAL      //if  ETHERNET_IP_LOCAL ==1 
  byte ip [] = {127,0,0,0}
  byte mask_net[] ={127,0,0,0} ; 
  byte gateway [] ={127,0,0,0} ; 
  byte DNS [] ={127,0,0,0}     ; 
#endif

int ref1 = 9000 ; 
int ref2 = 9000 ; 


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
  //lcd.backlight() ; 
  lcd.print("hola mundillo") ;
  
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
    Serial.print(F("Fallo DHCP"));    
  }else 
  {
    Serial.print(Ethernet.localIP());
  }
#endif
  // inicio scheduler -- > 
  Base_tiempo() ; 
  TimerStart(0,0,0,0,10) ; 
}

void loop() 
{
  timerEvent() ; 
  EthernetClient cliente_gpr = Gpredict() ;
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
          
           cliente.print("RPRT 0") ; // lectura correcta 
           Asignar_Azimuth_Declinacion(cliente_gpr,cadena) ;             
           cliente.flush() ;// VACIADO DE BUFFER 
           cadena ="" ;   
        }else if (c == 'p')
        {      
          cadena = az_dec_pos_actual() ;       
          cliente_gpr.print(cadena);
          cliente_gpr.flush() ; 
          cadena ="" ;     
        }else if (c=='q' || c=='Q' )
        {                
          cliente.flush() ; 
          cliente.stop()  ;
          cadena ="" ; 
          break ;          
        }else if (c=='S')
        {
          cliente.flush() ;  
          cliente.stop() ;
          cadena = "" ; 
        } 
     }
  }
  cliente.stop() ; 
  delay(10) ; 
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
float azimuth = 0  ;
float declinacion = 0  ;
ptr.read(); //espacio en blanco  
while(ptr.available())
{ 
   char c = ptr.read() ; 
   cad_leida+= c==','?'.':c;   
}

azimuth = (cad_leida.substring(0,cad_leida.indexOf(' '))).toFloat();
declinacion = (cad_leida.substring(cad_leida.indexOf(' '))).toFloat(); 
//conversion a valor entero 
azimuth = int ((azimuth*1024)/360) ; 
declinacion = int ((declinacion*1024)/90) ;  
/*
Serial.print("azimuth_entero: ") ; 
Serial.println(gpredict_azimuth) ; 
Serial.print("declinacion_entero: ") ; 
Serial.println(gpredict_declinacion) ; 
*/ 

}


String az_dec_pos_actual()
{

  return String(azimuth/100) + " " + String(altura/100)
}