
#include "io430.h"


#define PWM BIT2                //P1.2 - SAIDA DO PWM
#define UART_RX BIT1

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    IFG2 &= ~UCA0RXIFG; // Clear RX flag
    
    TACCR1 = UCA0RXBUF*4;

}


void ConfigUsart(){
   UCA0CTL1 = UCSWRST;                  //ATIVA O RESET DA USART

   UCA0CTL0 = 0;                        //8BITS + ASSINCRONO + UART + LSB + PARIDADE OFF
   UCA0CTL1 = UCSSEL_2+UCSWRST;         //SELECAO DO CLK (SMCLK)

   //UCA0STAT |= UCLISTEN; // loop back mode enabled

   //CONFIGURA O BAUDE-RATE
   UCA0MCTL =  UCBRS2 + UCBRF0;
   UCA0BR0 = 0x6D;
   UCA0BR1 = 0x00;

   //CONFIGURA OS PINOS DA USART
   P1SEL  |= UART_RX;
   P1SEL2 |= UART_RX;

   P1OUT = 0x00;

   UCA0CTL1 &= ~UCSWRST;            //APAGA O RESET DA USART

   UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
   __enable_interrupt();
}


//CONFIGURA O PWM
void initPwm(void)      
{
  //COFIGURA PINO DO PWM
  P1DIR |= PWM;                         //PINO DO PWM COMO SAIDA DIGITAL
  P1SEL |= PWM;                         //FUNCAO ALTERNATIVA DO PINO PWM
  
  //CONFIGURA O PWM
  TACCR0 = 1024;                //PERIODO DO PWM - 1024 TOTAL
  TACCR1 = 1024;                //INICIA COM POTENCIA MAXIMO
  TACTL = TASSEL_2+MC_1+TAIE;        //SMCLK + CONTAGEM PROGRESSIVA + INTERRUPCAO
  TACCTL1 = OUTMOD_7;           //MODO DO PWM
}


int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  ConfigUsart();
  initPwm();                    //INICIA O PWM
  
  __enable_interrupt();         //HABILITA INTERRUPCOES NAO MASCARAVEIS
  
  //UCA0TXBUF = 'A';

  while(1);
}
