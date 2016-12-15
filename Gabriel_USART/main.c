
#include "io430.h"
#define RXLED BIT6
#define UART_RX BIT1
#define UART_TX BIT2

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
   P1SEL  |= UART_RX+UART_TX;
   P1SEL2 |= UART_RX+UART_TX;

   P1OUT = 0x00;

   UCA0CTL1 &= ~UCSWRST;            //APAGA O RESET DA USART

   UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
   __enable_interrupt();
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    IFG2 &= ~UCA0RXIFG; // Clear RX flag

    if (UCA0RXBUF == 'A') // 'a' received?
    {
       P1OUT |= RXLED;
    }
    if (UCA0RXBUF == 'a') // 'a' received?
    {
       P1OUT &= ~RXLED;
    }

}


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    P1DIR |= RXLED;
    P1OUT &= ~RXLED;

	
    ConfigUsart();

    UCA0TXBUF = 'A';

      while(1){

      }
}
