#include <Arduino.h>

#include "pinout_ard_uno.h" 
#include "rtc.h"
#include <Ethernet.h>
#include <encoders.h>
#include "gpredict.h"
#include <LiquidCrystal_I2C.h> 
#define ETHERNET_IP_LOCAL 0 //0 -> IP POR DHCP 
#define PORT_GPREDICT 4533 
#define PORT_STELLARIUM 10000
#define SIZE_BUFFER_RECEIVE_GPREDICT 20  
#define I2C_ADDRESS_LCD 0x27  



                           //1 -> ip ASIGNADA POR EL PROGRAMADADOR 
#if ETHERNET_IP_LOCAL      //if  ETHERNET_IP_LOCAL ==1 
  byte ip [] = {127,0,0,0}
  byte mask_net[] ={127,0,0,0} ; 
  byte gateway [] ={127,0,0,0} ; 
  byte DNS [] ={127,0,0,0}     ; 
#endif




EthernetServer stellarium(PORT_STELLARIUM) ;
EthernetServer Gpredict(PORT_GPREDICT) ; 
LiquidCrystal_I2C lcd (ADRRESS_LCD_I2C,COLS_LCD,FILAS_LCD) ; 

unsigned long int RA12HS = 0x80000000  ; // 4bytes  
long int DEC10 = 0x40000000  ; //4 bytes 



void setup() 
{

  // debugger -- 
  Serial.begin(9600) ; 
  Serial.println("inicio de programa ") ; 
  Ethernet.init(PINSS) ; 
  lcd.init() ; 
  lcd.backlight() ;  
  delay(100) ; 
  byte mac [] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // dirmac
#if ETHERNET_IP_LOCAL 
  if (Ethernet.begin(mac,ip,DNS,gateway,mask_net) == 0)
  {
   Serial.print(F("Fallo DHCP"));    
  }else 
  {
   Serial.print(Ethernet.localIP());
  }
#else 
  Serial.println("ethernet obrains dhcp ip") ; 
  if (Ethernet.begin(mac) == 0)
  {
    lcd.print(F("Fallo DHCP"));    
  }else 
  {
    lcd.print(Ethernet.localIP());
  }

#endif
  if(initRTC() == -1){
    lcd.setCursor(0,1) ; 
    lcd.print("reinicie equipo") ; 
    while(1){ 
      delay(1000) ; 
    }
  }
  initEncoders() ; 

}


// superloop structure  
void loop() 
{
  char buffer_receive[50] ; 
  char response_data[20]  ; 
  EthernetClient cliente_gpr = Gpredict.available() ;
  loop_test()  ; 
  if (cliente_gpr)
  { 
 
    while (cliente_gpr.connected())
    { 
     
      if (cliente_gpr.available())
      {
        uint8_t index = 0 ; 
        int8_t respuesta ; 
        while(cliente_gpr.available())
        { 
          char c_a = (char )cliente_gpr.read() ; 
          buffer_receive[index] = c_a ; 
          index++ ; 
        }
        decoderGpredict(buffer_receive,50); //add field for a response pointer   
        memset(&buffer_receive,'\0',50) ; 
        respuesta = responseGpredict(response_data) ; 
        if (respuesta == 0 ){
          Serial.println("enviando respuesta") ; 
          cliente_gpr.print(response_data); 
          cliente_gpr.flush() ; 
        }else if (respuesta == -1){
          cliente_gpr.flush() ; 
          cliente_gpr.stop() ; 
        }else if (respuesta == 1){
          Serial.println ("error en dato enviado por el Gpredict") ; 
          cliente_gpr.flush() ; 
          cliente_gpr.stop() ; 
        }
    
     }
  }
  cliente_gpr.stop() ; 
  delay(10) ; 
 
 }
 





}

