
#include "io430.h"

#define BOTAO BIT3
#define LED1 BIT0
#define LED2 BIT6
#define CLK 10000000

int pressionado = 0;
int inatividade = 0;
int LeveToque = 0;
int Fg8 = 0;

void Reseta_Estados();
void Desliga();
void Pareado();
void Busca_NovoPar();
void Localiza_Pares();
void Desligado();
void ConfigPins();
void ConfigTimer();


#pragma vector = PORT1_VECTOR
__interrupt void trata_p1(void)
{//DISPARA O TIMER 0
  
  
  
  if(P1IN&BOTAO)                //SE O BOTAO FOI SOLTO
  {    
    if(pressionado >= 4) //SE O BOTAO FICOU MAIS DE 4 SEGUNDOS PRESSIONADO E FOI SOLTO
    {
      LPM3_EXIT;         //SAI DO MODO DE BAIXO CONSUMO
    }
    else if(Fg8){
      Fg8 = 0;
    }else {
      LeveToque = 1;    //SE O BOTAO FOI SOLTO E FOI PRESSIONADO POR MENOS DE 4s ENTAO FOI APENAS UM LEVE TOQUE
    
    }
    
    P1IES |= BOTAO;        //MUDA A INTERRUPCAO PARA QUANDO O BOTAO FOR PRESSIONADO; BORDA DE DESCIDA
    
    TACTL &= ~MC_3;     //PARA O TIMER
    TAR = 0;            //REINICIA A CONTAGEM
    
  }
  else {                         //SE O BOTAO ESTA SENDO PRESSIONADO
    P1IES &= ~BOTAO;        //MUDA A INTERRUPCAO PARA QUANDO O BOTAO FOR SOLTO; BORDA DE SUBIDA
    
    //MUDA A OPERACAO DO TIMER PARA CONTAGEM CONTINUA
    TACTL &= ~MC_3;              //APAGA OS BITS DO MC
    TACTL |= MC_1;              //CONTA ATE CHEGAR EM TACCR0
  }

P1IFG =0;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void trata_timer(void)
{
  switch(TAIV)
  {
    case 0x02:          //INTERRUPCAO DO CANAL 1
      
      break;
    case 0x04:          //INTERRUPCAO DO CANAL 2
      
      break;
    case 0x06:          //INTERRUPCAO DO CANAL 3
      
      break;
    case 0x08:          //INTERRUPCAO DO CANAL 4
      
      break;
    case 0x0A:          //INTERRUPCAO DE ESTOURO DO TIMER
      pressionado += 4;         //SE O TIME ESTOROU PASSOU 4s
      LeveToque = 0;            //SE PASSOU MAIS QUE 4s ENTAO NAO FOI UM LEVE TOQUE NO BOTAO
      if(pressionado >= 8){//SE FOR PRESSIONADO POR MAIS DE 8s
        LPM3_EXIT;         //SAI DO MODO DE BAIXO CONSUMO
        Fg8 = 1;
      }
      break;
  }
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void trata_timer1(void)
{
  switch(TA1IV)
  {
    case 0x02:          //INTERRUPCAO DO CANAL 1
      
      break;
    case 0x04:          //INTERRUPCAO DO CANAL 2
      
      break;
    case 0x06:          //INTERRUPCAO DO CANAL 3
      
      break;
    case 0x08:          //INTERRUPCAO DO CANAL 4
      
      break;
    case 0x0A:          //INTERRUPCAO DE ESTOURO DO TIMER
      inatividade += 30;         //SE O TIME ESTOROU PASSOU 4s
            
      break;
  }
}


int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  //CONFIGURA O VLO COOMO FONTE DE CLOCK PRO ACLK
  //12KHz
  BCSCTL1 = ~XTS;        //LFXTCLK BAIXA FREQUENCIA
  BCSCTL3 = LFXT1S_2;   //VLO QUANDO XTS = 0
  
  ConfigPins();
  ConfigTimer();
  
  __enable_interrupt();
  
  //while(1);
  
  Desligado();

  return 0;
}

void ConfigTimer()
{                                       
  //TIMER 0     -       CLK 12KHZ
  //TIMER PARA O BOTAO 
  unsigned int tempo1 = 4*1200;
  TACTL = TASSEL_1+ID_0+MC_1+TAIE;      //ACLK + DIVISOR 1 + MODO PARADO + INTERRUPCAO DE ESTOURO
  TACCR0 = tempo1;                     //ESTOURO DO TIMER A CADA 4 SEGUNDOS; 4* CLK DO TIMER
  
  //TIMER 1     -       CLK 12KHZ/8 = 1,5KHZ
  //TIMER PARA A INATIVIDADE
  TA1CTL = TASSEL_1+ID_3+MC_0+TAIE;      //ACLK + DIVISOR 8 + MODO PARADO + INTERRUPCAO DE ESTOURO
  TA1CCR0 = 30*150;                    //ESTOURO DO TIMER A CADA 30 SEGUNDOS; 30* CLK DO TIMER
}

void ConfigPins()
{
  P1SEL &= ~(LED1+LED2+BOTAO);  //PINOS I/0
  P1DIR |= LED1+LED2;           //LEDS COMO SAIDA DIGITAL
  P1DIR &= ~BOTAO;              //BOTAO COMO ENTRADA DIGITAL
    
  P1IFG = 0;              //APAGA OS FLAGS DE INTERRUPCAO
  P1IES |= BOTAO;        //INTERRUPCAO NA BORDA DE DESCIDA; QUANDO O BOTAO EH PRESSIONADO 
  P1REN |= BOTAO;        //PULL UP INTERNO ATIVADO
  P1IE |= BOTAO;        //HABILITA A INTERRUPCAO
}

void Desligado()
{
  Reseta_Estados();
  
  /*-------------------------------------------------------------------------
 *   EXECULTA AS FUNCOES DO ESTADO
 *-------------------------------------------------------------------------*/
  //P1OUT &= ~(LED1+LED2);        //DESLIGA OS LEDS
  
  LPM3;                         // ENTRA NO MODO LPM3
  
  /*-------------------------------------------------------------------------
 *   MUDA O ESTADO
 *-------------------------------------------------------------------------*/
  
  if(pressionado >= 8)
  {
    Busca_NovoPar();
  }else if(pressionado >= 4)
  {
    Localiza_Pares();
  }
  
}

void Localiza_Pares()
{
  Reseta_Estados();
    
  TA1CTL &= ~MC_3;     //RESETA OS BITS DO MC
  TA1CTL |= MC_1;      //COMECA A CONTAGEM DO TIMER; PARA CALCULAR A INATIVIDADE
    
/*-------------------------------------------------------------------------
 *   EXECULTA AS FUNCOES DO ESTADO
 *-------------------------------------------------------------------------*/  
  while((inatividade < 30)&&(pressionado < 4))    //&&(~LeveToque)
  {
    
    //PISCA O LED 
    P1OUT |= LED1;                //LIGA O LED
    __delay_cycles(CLK);           //ESPERA METADE DA FREQUENCIA
    P1OUT &= ~LED1;               //DESLIGA O LED
    __delay_cycles(CLK);           //ESPERA METADE DA FREQUENCIA
    
    if(LeveToque){
     break; 
    }
    
  }
  
/*-------------------------------------------------------------------------
 *   MUDA O ESTADO
 *-------------------------------------------------------------------------*/
  if(LeveToque)
  {
    Pareado();
  }
  if((inatividade >= 30)||(pressionado >= 4))
  {
    Desliga();
  }
  
}

void Busca_NovoPar()
{
  Reseta_Estados();
  
  TA1CTL &= ~MC_3;     //RESETA OS BITS DO MC
  TA1CTL |= MC_1;      //COMECA A CONTAGEM DO TIMER; PARA CALCULAR A INATIVIDADE
  
  P1OUT |= LED1;        //LIGA O LED1
  P1OUT &= ~LED2;       //DESLIGA O LED2
  
/*-------------------------------------------------------------------------
 *   EXECULTA AS FUNCOES DO ESTADO
 *-------------------------------------------------------------------------*/
  while((inatividade < 30)&&(pressionado < 4))    //&&~(LeveToque)
  {
    //ALTERNA OS LEDS COM 2HZ
    __delay_cycles(CLK/4);
    P1OUT ^= LED1+LED2;
    
    if(LeveToque){
     break; 
    }
  }
   
  /*-------------------------------------------------------------------------
 *   MUDA O ESTADO
 *-------------------------------------------------------------------------*/
  if(LeveToque)
  {
    Pareado();
  }
  if((inatividade >= 60)||(pressionado >= 4))
  {
    Desliga();
  }
}

void Pareado()
{
  Reseta_Estados();
  
  /*-------------------------------------------------------------------------
 *   EXECULTA AS FUNCOES DO ESTADO
 *-------------------------------------------------------------------------*/
  P1OUT |= LED1;        //LIGA O LED1
  P1OUT &= ~LED2;       //DESLIGA O LED2
  
  while((pressionado < 4)&&(~LeveToque)) //
  
  /*-------------------------------------------------------------------------
 *   MUDA O ESTADO
 *-------------------------------------------------------------------------*/
  if(LeveToque)
  {
    Localiza_Pares();
  }
  if(pressionado >= 4)
  {
    Desliga();
  }
}

void Desliga()
{
  Reseta_Estados();
  
 /*-------------------------------------------------------------------------
 *   EXECULTA AS FUNCOES DO ESTADO
 *-------------------------------------------------------------------------*/
  P1OUT &= ~LED1;       //DESLIGA O LED1
  
  //PISCA O LED2 POR 5s EM 1HZ
  for(int i=0;i<5;i++)
  {
    P1OUT |= LED2;
    __delay_cycles(CLK/2);
    P1OUT &= ~LED2;
    __delay_cycles(CLK/2);
  }
  
 /*-------------------------------------------------------------------------
 *   MUDA O ESTADO
 *-------------------------------------------------------------------------*/
  Desligado();
}

void  Reseta_Estados()
{
 /*-----------
 *   TIMER0
 *------------*/
  TACTL &= ~MC_3;              //PARA A CONTAGEM DO TIMER0; APAGA OS BITS DO MC 
  TAR = 0;                    //REINICIA A CONTAGEM DO TIMER0
  
 /*-------------------------------------------------------------------------
 *   TIMER1
 *-------------------------------------------------------------------------*/
  TA1CTL &= ~MC_3;              //PARA A CONTAGEM DO TIMER1; APAGA OS BITS DO MC 
  TA1R = 0;                    //REINICIA A CONTAGEM DO TIMER1
  
 /*-------------------------------------------------------------------------
 *   RESETA AS FLAGS E APAGA OS LEDS
 *-------------------------------------------------------------------------*/
  LeveToque = 0;
  pressionado = 0;
  inatividade = 0;
  P1OUT &= ~(LED1+LED2);        //DESLIGA OS LEDS
}