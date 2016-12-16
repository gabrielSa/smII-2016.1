
#include "io430.h"
#include "Encoder.h"
//#include "Encoder.c"


int clockTimer = 800000;              //SMCLK - TASSEL_2
int numeroEncoders  = 80;          //NUMERO DE ENCODERS DO MOTOR POR VOLTA


float posicaoEncoder = 0;


float rpm = 0;
float anguloGraus;

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  init_encoder(numeroEncoders, clockTimer, posicaoEncoder);

  __enable_interrupt();         //HABILITA INTERRUPCOES NAO MASCARAVEIS

  
  while(1){
    rpm = getRpm();
    anguloGraus = getAngulo();
  }
}
