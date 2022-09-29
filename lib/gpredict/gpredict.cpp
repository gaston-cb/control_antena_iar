#include <Arduino.h>
#if defined(_cplusplus)
extern "C" {
#endif

#define COMMAND_GPREDICT_POSITION_READ 'P'
#define COMMAND_GPREDICT_POSITION_SEND 'p'
#define COMMAND_GPREDICT_POSITION_STOP 'S'
#define COMMAND_GPREDICT_POSITION_ERROR 'e'


///commands response 
#define COMMAND_GPREDICT_POSITION_RESPONSE "RPRT 0"
#define ERROR_COMMAND_DATA_RECEIVED -3 
#define QUIT_COMMAND_CONNECTION -1 
#define RESPONSE_GPREDICT 0
#define RESPONSE_GPREDICT_ERROR 1 ///USING FOR TESTING 



static char _command_receive ; 
uint8_t responseData ; 





void decoderGpredict(char *data_receiver, uint16_t length_buffer){ 
    char *decode_response ; 
    float az, h ; 
    char first_letter ; 
    Serial.println("data received") ; 
    Serial.println(data_receiver) ;     
    if (data_receiver[0] == COMMAND_GPREDICT_POSITION_READ){
        /// receive a position decoders 
        decode_response = strtok(&data_receiver[1]," ") ; 
        if(decode_response)
        {
           az = atof(decode_response) ;            
        }
        decode_response = strtok(NULL, " ");
        if(decode_response){
           h = atof(decode_response) ; 
        }
        _command_receive = COMMAND_GPREDICT_POSITION_READ ; 
    }else if (data_receiver[0] == COMMAND_GPREDICT_POSITION_SEND){
        _command_receive = COMMAND_GPREDICT_POSITION_SEND ; 
    }else if (data_receiver[0] == 'q' || data_receiver[0] == 'Q' || data_receiver[0] == 'S'){
        _command_receive = COMMAND_GPREDICT_POSITION_STOP ; 
    }else { 
        _command_receive = 'e' ; 
    }

    memset(&data_receiver,'\0',length_buffer) ; //clean buffer  

}


int8_t responseGpredict(char *buffer_response){ 
    int8_t response = ERROR_COMMAND_DATA_RECEIVED; 
    switch (_command_receive)
    {
    case COMMAND_GPREDICT_POSITION_READ:
        memcpy(buffer_response,COMMAND_GPREDICT_POSITION_RESPONSE,8 ) ; 
        response = RESPONSE_GPREDICT ; 
        break;
    case COMMAND_GPREDICT_POSITION_SEND: 
        memcpy(buffer_response,"125.33\r\n33.22\r\n",20)  ;  ///FIXME: CHANGE TO RESPONSE FOR READ LAST POSITION INTO LIBRARY  
        response = RESPONSE_GPREDICT ; 
        break ;
    case  COMMAND_GPREDICT_POSITION_STOP: 
        buffer_response = NULL ; 
        response = QUIT_COMMAND_CONNECTION ; 
        break ; 
    case  COMMAND_GPREDICT_POSITION_ERROR: 
        response = RESPONSE_GPREDICT_ERROR; 
        buffer_response = NULL ; 
    
        break ;        
    default:
        response = RESPONSE_GPREDICT_ERROR +1 ; 
        buffer_response = NULL ; 
        break;
    }


    return response ; 
}


#if defined(_cplusplus)
}
#endif