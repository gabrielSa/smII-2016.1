#include "io430.h"

//P1.4 - ENTRADA DA CAPTURA - ENC
#define ENCODER BIT4
//P1.5 - ENCODER DO SENTIDO - STD
#define PINO_SENTIDO BIT5
//P1.6 - LED DO SENTIDO
#define LEDS BIT6
//
#define SENTIDO (P1IN & PINO_SENTIDO)


float getRpm(void);
void init_encoder(int N_enc, int clock, float initPos);
float getAngulo(void);
__interrupt void encoder(void);
__interrupt void porta1(void);