/*
*   
*   Maquina de timer -
*   Cantidad de relojes - 8 : modificable desde "globales.h". Se debe cambiar #define RELOJES  por el numero deseado
*   Autor : Gaston Valdez 
*   Fecha de prueba : 22/3/2020 - Funciono de manera correcta
*   Prueba - apagado y encendido de leds

*   Nota- Todos estos relojes se suponen bien usados y no hay ningun tipo
*   de check en las funciones. Es deber del programador revisar que no exista
*   ningun fallo de comunicacion entre ellas.

*   No existe comprobacion de TimerStart, POR TANTO DEBE INGRESARSE EL TIEMPO CORRECTAMNETE (ver que ms<1000).
*   Si ms>1000 entonces contara mas de un segundo, la funcion no contaria el tiempo correctamente
*
*   Cada reloj utilizado dentro de TimerEvent() debe cerrarse: hay dos formas distintas
*    1 - poniendo el reloj que se vencio todos sus elementos a cero (es decir TimerStart(n_reloj,0,0,0,0))
*    2 - Usando timerClose(n_reloj)
*    Es importante el cierre de los relojes, ya que ejecutara el case mas de una vez, y el software no es capaz de cambiarlo.
*    
*/

#include <Arduino.h>
#include "../../include/control_motores/control_motores.h"
#include "../../include/lecturas_encoders/lectura_encoders.h"


#define RELOJES 8  
volatile char dec_milis = 0 ;
unsigned int  timer[RELOJES][4]; // vector de relojes
char timer_activo[RELOJES] = {0, 0, 0, 0, 0, 0, 0, 0};
char FlagRepEvent[RELOJES] = {88, 88, 88, 88, 88, 88, 88, 88} ;

extern int ref1 ; 
extern int ref2 ; 




void Base_tiempo()
{
  /*  
   *  Configuracion de registros y flags para habilitar interrupciones
   *  por timer usando timer2 de atmega 328p
   *  Base de tiempo 100 us generacion de interrupcion
  */
  
  SREG = (SREG & 0b01111111);
  TCNT2 = 0 ;
  TIMSK2 = TIMSK2 | 0b00000010 ;
  TCCR2A = 0b00000010;
  TCCR2B = 0b00000011; // 0.5 MhZ n= 32 
  OCR2A = 49;
  SREG = (SREG & 0b01111111) | 0b10000000 ;

}



void TimerStart(char n_reloj, char h, char m, char s, int ms)
{
  /* 
   * Funcion TimerStart:  inicializa y configura un reloj
   *  recibe hora, minutos , segundos y milisegundos para
   *  configurar un reloj dentro de la matriz
  */

  SREG = (SREG & 0b01111111); // deshabilitar int
  char aux = 0 ;
  aux = (timer[n_reloj][0] == 0 && timer[n_reloj][1] == 0  && timer[n_reloj][2] == 0 && timer[n_reloj][3] == 0) ? 1 : 2 ;
  if (aux == 1)
  {
    aux = 0 ;
    // modificar FlagRepEvent
    while (FlagRepEvent[aux] != (n_reloj + 1) && aux < RELOJES - 1)
    {
      aux = aux + 1 ;
    }
    FlagRepEvent[aux] = 88 ;

  }
  aux = 0;


  timer[n_reloj][0] = h  ;
  timer[n_reloj][1] = m  ;
  timer[n_reloj][2] = s ;
  timer[n_reloj][3] = ms  ;

  if (ms != 0 || s != 0 || m != 0 || h != 0)
  {
    while (timer_activo[aux] != 0 && aux < RELOJES) {
      aux = aux + 1;
    }
    timer_activo[aux] = n_reloj + 1  ;
  }
  SREG = (SREG & 0b01111111) | 0b10000000 ; // habilitar int
}


void AnalizoTimer(char n_reloj, char index)
{
  /*
   AnalizoTimer - No usada por el usuario
   Funcion invocada por la interrupcion
   Descuenta el tiempo hasta cero y pone un
   flag para generacion de eventos

*/
  timer[n_reloj][3] = (timer[n_reloj][3] != 0) ? timer[n_reloj][3] - 1 : 0 ;
  if (timer[n_reloj][3] == 0)
  {
    if (timer[n_reloj][0] == 0 && timer[n_reloj][1] == 0  && timer[n_reloj][2] == 0) // se vence reloj  -> flagRepEvent[n0_reloj] = reloj + 1 -> N° de reloj (1 a 8)
    {
      FlagRepEvent[index] = n_reloj + 1  ;
      timer_activo[index] = 0 ;
    } else if (timer[n_reloj][2] != 0) // segundos
    {
      timer[n_reloj][2] = timer[n_reloj][2] - 1  ;
      timer[n_reloj][3] = 999 ;
    } else if (timer[n_reloj][1] != 0) // minutos
    {
      timer[n_reloj][1] =  timer[n_reloj][1] - 1  ;
      timer[n_reloj][2] = 59 ;
      timer[n_reloj][3] = 999 ;
    } else if (timer[n_reloj][0] != 0) // horas
    {
      timer[n_reloj][0] =  timer[n_reloj][0] - 1  ;
      timer[n_reloj][1] =  59 ;
      timer[n_reloj][2] = 59 ;
      timer[n_reloj][3] = 999 ;
    }
  }
}




void timerClose(char n_reloj)
{
/* 
   timerClose() - Usada por el usuario
   Funcion que cierra el numero de reloj
   Pone a cero todo la fila del reloj
   Modifica FlagRepEvent y timer_activo
*/

  SREG = (SREG & 0b01111111); // deshabilitar int
  char aux = TCNT2 ;
  char i = 0 ;
  timer[n_reloj][0] = 0 ;
  timer[n_reloj][1] = 0 ;
  timer[n_reloj][2] = 0 ;
  timer[n_reloj][3] = 0 ;

  while (timer_activo[i] != n_reloj + 1 && i < RELOJES - 1)
  {
    i = i + 1  ;
  }
  timer_activo[i] = 0 ;

  // ordenar timer activo y cambiar FlagRepEvent
  i = 0 ;
  for (i = 0; i < RELOJES; i++)
  {
    if (FlagRepEvent[i] == n_reloj + 1)
    {
      FlagRepEvent[i] = 88 ;
    }
    if (timer_activo[i] == 0)
    {
      char k = 1 ;
      while (timer_activo[i + k] == 0 && (i + k) < RELOJES)
      {
        k = k + 1 ;
      }
      timer_activo[i] = timer_activo[i + k] ;
      timer_activo[i + k] = 0 ;
    }
  }

  TCNT2 = aux ;
  SREG = (SREG & 0b01111111) | 0b10000000 ; // habilitar int
}

void timerCloseAll() 
{
  /*
   * Desactivar todos los relojes activos 
  */
  char n_reloj = 0 ; 
  for(n_reloj;n_reloj<RELOJES;n_reloj++){
      timer[n_reloj][0] = 0 ;
      timer[n_reloj][1] = 0 ;
      timer[n_reloj][2] = 0 ;
      timer[n_reloj][3] = 0 ;
      FlagRepEvent[n_reloj] = 88 ; 
      timer_activo[n_reloj] = 0 ; 
  }
  
}





void timerStop(char n_reloj)
{
  /*
   Funcion que para el reloj
   Puede seguir contando luego
   Modifica timer_activo y flagrepevent
   Reinicio de reloj - funcion timer_marcha
   La funcion timer_marcha no contempla ninguna validacion de reloj
   Debe ser realizada por el programador y/o modificar este codigo fuente
*/

  SREG = (SREG & 0b01111111); // deshabilitar int
  char i = 0 ;
  // buscar el timer_activo -- desativarlo --
  while (timer_activo[i] != n_reloj + 1 && i < RELOJES  )
  {
    i = i + 1  ;
  }
  timer_activo[i] = 0 ;
  i = 0 ;
  for (i = 0; i < RELOJES; i++)
  {
    if (FlagRepEvent[i] = n_reloj + 1)
    {
      FlagRepEvent[i] = 88 ;
    }
    if (timer_activo[i] == 0)
    {
      char k = 1 ;
      while (timer_activo[i + k] == 0 && (i + k) < 8)
      {
        k = k + 1 ;
      }
      timer_activo[i] = timer_activo[i + k] ;
      timer_activo[i + k ] = 0 ;
    }
  }

  SREG = (SREG & 0b01111111) | 0b10000000 ; // habilitar int
}

void timer_marcha(char n_reloj )
{
/*
   timer_marcha - Usada por usuario
   vuelve a descontar de un contador que se paro.
   con timerStop
*/

  char aux = 0 ;
  while (timer_activo[aux] != 0 && aux < RELOJES )
  {
    aux = aux + 1;
  }
  timer_activo[aux] = n_reloj + 1  ;

}



ISR(TIMER2_COMPA_vect)
{
 /*
     Funcion de Interrupcion
    Ocurre cada 100us
    idea - contar 10 interrupciones - > 1 ms
    Reloj con horas - minutos - segundos - milisegundos

*/
  SREG = (SREG & 0b01111111); // deshabilitar int
  char i = 0 ;
  char cant = 0 ;
  dec_milis = dec_milis + 1 ;
  if (dec_milis == 10)
  {
    dec_milis = 0 ;                             //reinicio contador
    while (timer_activo[i] != 0 && i < RELOJES)
    {
      AnalizoTimer(timer_activo[i] - 1, i) ;
      i = i + 1 ;
    }
    /*
       Acomoda el vector timer_activo en caso de algun reloj vencido
       Primeros valores - > relojes activos
       No hay correspondencia entre los relojes y el indice del vector
    */
    for (cant = 0; cant < RELOJES; cant++)
    {
      if (timer_activo[cant] == 0)
      {
        char k = 1 ;
        while (timer_activo[cant + k] == 0 && (cant + k) < RELOJES)
        {
          k = k + 1 ;
        }
        timer_activo[cant] = timer_activo[cant + k] ;
        timer_activo[cant + k ] = 0 ;
      }
    }

  }

  SREG = (SREG & 0b01111111) | 0b10000000 ; // habilitar int

} // END INTERRUPT



void timerEvent()
{
/*
 *  Funcion de eventos -
 *  Cuando levanta el flag - inicializa evento segun FlagRepEvent
*/
  char k ; 
  for (k = 0; k < RELOJES; k++) {
    switch (FlagRepEvent[k])
    {
      case 1:
            leer_encoders() ; 
            control_motores(ref1,ref2) ;  
            break ;
      case 2:
            break ;
      case 3:       
            break ;
      case 4 :          
            break ;         
      case 5: 
            break ;
      case 6 :           
            break ;
      case 7 :            
            break ;
      case 8 :           
            break ;
      default:
            break ;
    }
  }
}