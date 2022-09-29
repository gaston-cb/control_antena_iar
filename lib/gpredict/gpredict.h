#include <Arduino.h>


void decoderGpredict(char *data_receiver,uint16_t length_buffer) ; 
/**
 * @brief Respuesta a Gpredict 
 * 
 * @param buffer_response [OUT]
 * @return uint8_t 0 .. RESPONSE 
 *                -1 .. QUIT 
 *                 1 .. NOT RESPONSE   
 */
uint8_t responseGpredict(char *buffer_response)                  ; 