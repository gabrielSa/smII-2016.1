
#include "io430.h"
#include "Encoder.h"


int Presc = 1;                  //ID_0
int Fclk = 800000;              //SMCLK - TASSEL_2
int N_encoders  = 304;          //NUMERO DE ENCODERS DO MOTOR POR VOLTA


float posicaoInicial = 0;
unsigned int furos_percorrido = 0;

//VARIAVEIS PARA O ENCODER
int clk_anterior = 0;
unsigned int clk_percorrido = 0;
//int ja_estourado = 0;
int estouros = 0;
int tempo;

float rotacaopm = 0;            //RPM
float anguloEncoder = 0;


float getRpm(){
  
  if (clk_percorrido!=0)
      rotacaopm = 60*Fclk/(float)(Presc*N_encoders*clk_percorrido);             //CALCULO DA ROTACAO POR MINUTO
  
  return rotacaopm;
  
}

float getAngulo(){
  
  anguloEncoder = furos_percorrido*360/N_encoders + posicaoInicial;
  
  if (anguloEncoder > 360)
    anguloEncoder -= 360;
  
  return anguloEncoder;
  
}

void init_encoder(int N_enc, int clock, float initPos)
{
  N_encoders = N_enc;
  Fclk = clock;
  posicaoInicial = initPos;
  
  
  //CONFIGURA A INTERRUPCAO DA PORTA DO ENCODER
  P1SEL &= ~ENCODER;                  //ENCODER COMO ENTRADA DIGITAL
  P1DIR &= ~ENCODER;                 //ENCODER COMO I/O
  //P1REN |=  ENCODER;                 //ATIVA O RESISTOR INTERNO
  P1IES &= ~ENCODER;                 //TRANSICAO DE SUBIDA
  P1IE  |=  ENCODER;                 //ATIVA A INTERRUPCAO
  
  //CONFIGURA O PINO DO SENTIDO
  P1DIR &= ~PINO_SENTIDO;                 //PINO DO SENTIDO COMO ENTRADA DIGITAL
  P1SEL &= ~PINO_SENTIDO;                 //PINO DO SENTIDO COMO I/O
  
  //CONFIGURA O TIMER
  TACCR0 = 1024;                //PERIODO DO TIMER
  TACTL = TASSEL_2+MC_1+TAIE;        //SMCLK + CONTAGEM PROGRESSIVA + INTERRUPCAO
}


#pragma vector = PORT1_VECTOR
__interrupt void encoder(void)
{
  //NAO COLOCAR NADA ANTES!!!!!!!!! 
  if((P1IFG & ENCODER)==ENCODER)        //SE A INTERRUPCAO OCORREU NA PORTA DO ENCODER
  {
    tempo = TAR;
    //NAO COLOCAR NADA ANTES!!!!!!!!!    
    if(SENTIDO)         //ACAO PARA SENTIDO POSITIVO
    {        
      //P1OUT |= LEDS;
    }else               //ACAO PARA SENTIDO NEGATIVO
    {
      //P1OUT &= ~LEDS;
    }
    
    //CALCULO PARA VERIFICAR A QUANTIDADE DE CLOCKS PERCORRIDOS
    clk_percorrido = tempo - clk_anterior + estouros*TACCR0;
    clk_anterior = tempo;
    
    P1IFG &= ~ENCODER;          //APAGA A FLAG DE INTERRUPCAO
      
    //RESETA AS FLAGS DO ESTOURO
    estouros = 0;
//    ja_estourado = 0;
  }
  furos_percorrido==N_encoders ? furos_percorrido=0 : furos_percorrido++;       //SE O NUMERO DE FUROS PERCORRIDOS FOR IGUAL AO NUMERO DE FUROS DO ENCODER A 
                                                                                //VARIAVEL "furos_percorrido" RESETA, SE NAO INCREMENTA
}


#pragma vector = TIMER0_A1_VECTOR
__interrupt void porta1(void){
  switch(TAIV){
  case 0x02:    //TACCR1
    
    break;
    
  case 0x04:
    
    break;
    
  case 0x06:    //TACCR3
    
    break;
    
  case 0x08:    //TACCR4
    
    break;
    
  case 0x0A:    /*/ESTOURO DO TIMER; TAIFG
    //SE NAO TINHA OCORRIDO UM ESTOURO DO TIMER ANTES
    if(ja_estourado == 0){      //E ESSE EH O PRIMEIRO ESTOURO
       clk_anterior -= TACCR0;  //ALTERA O CALCULO
       ja_estourado = 1;        //E SETA A FLAG INDICANDO QUE JA OCORREU UM ESTOURO
    }
    //SE JA TIVER OCORIDO UM ESTOURO
    else*/
      estouros++;      //INCREMENTA A VARIAVEL ESTOURO PARA O CALCULO
    break;
  }
}