#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#include <time.h>
#include <RTClib.h>

#define SERVER_NTC "gps.iar.unlp.edu.ar"
#define PORT_NTC 8888
#define NTP_PACKET_SIZE 48
#define ZERO_FOR_UNIX_TIME 2208988800UL
#define ALARMS_PER_DAY "12:00:00" // 24:00:00 
#define FRECUENCY_CLOCK 1  // OPCION 32K , 4K, .. 

static EthernetUDP udp ; 
static RTC_DS3231 rtc ; 
static byte packet_buffer_ntc[NTP_PACKET_SIZE] ; 

static struct tm* getDateNTC() ; 
static void  sendNTPPacket() ; 

// uint8_t -1
// uint8_t  1 
int8_t initRTC() {
    struct tm *ts ; 
    int8_t response ; 
    if (!rtc.begin()){
        return  -1 ;  
    }
    udp.begin(8888) ; 

    ts = getDateNTC() ; 
    while (ts == NULL){
        ts = getDateNTC() ; 
        delay(1000) ; 
            
    }
    
    if (ts!=NULL){
        Serial.println("return is not null "); 
        Serial.print("fecha: ") ; Serial.print (ts->tm_year+1900) ; Serial.print("/") ; 
        Serial.print(ts->tm_mon+1) ; Serial.print ("/") ; Serial.print(ts->tm_mday) ; 
        Serial.print(" hora: ") ; 
        Serial.print( ts->tm_hour) ; Serial.print(": ") ; 
        Serial.print( ts->tm_min) ; Serial.print(": ") ; 
        Serial.print( ts->tm_sec) ; Serial.println() ; 

        rtc.adjust(DateTime(ts->tm_year+1900,ts->tm_mon+1,ts->tm_mday,ts->tm_hour,ts->tm_min,ts->tm_sec  )) ;         
        response =  1 ; 
        rtc.writeSqwPinMode(DS3231_SquareWave1Hz) ; 
    }else if (ts == NULL ){ 
        Serial.println("TS IS NULL "); 
    }

    return response ; 
}


// void updateRTC() ; 


static struct tm* getDateNTC(){ 
    struct tm *ts= NULL ; 
    byte obtain_date[NTP_PACKET_SIZE] ;    
    uint8_t response ;
    sendNTPPacket() ; 
    response = udp.parsePacket() ; 
    if(!response){
        Serial.println("caase if ") ; 
        ts = NULL ; 
    }else{
        Serial.println("caase else ") ; 

        udp.read(obtain_date,NTP_PACKET_SIZE) ; 
        unsigned long int highword = word(obtain_date[40],obtain_date[41]) ; 
        unsigned long int lowword = word(obtain_date[42],obtain_date[43]) ; 
        unsigned long int secsSince1990 = highword<<16 | lowword  ; 
        time_t raw_seconds =(time_t) (secsSince1990 - ZERO_FOR_UNIX_TIME ) ; 
        raw_seconds = raw_seconds- UNIX_OFFSET ; 
        ts = localtime(&raw_seconds) ; 
    }
    return ts ; 
}



static void  sendNTPPacket(){ 
    memset(packet_buffer_ntc,0,NTP_PACKET_SIZE) ; 
    packet_buffer_ntc[0] = 0b11100011 ; 
    packet_buffer_ntc[1] = 0 ; 
    packet_buffer_ntc[2] = 6 ; 
    packet_buffer_ntc[3] = 0xEC ; 
    packet_buffer_ntc[12] = 49 ; 
    packet_buffer_ntc[13] = 0x4E ; 
    packet_buffer_ntc[14] = 49 ; 
    packet_buffer_ntc[15] = 52 ; 
    const char time_server[] = "time.nist.gov" ; 
    udp.beginPacket(time_server,123) ; 
    udp.write(packet_buffer_ntc,NTP_PACKET_SIZE)  ; 
    udp.endPacket() ; 
   // delay(10) ; 

}



void getRTC(){ 



}