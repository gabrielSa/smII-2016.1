
#include <msp430.h>
#include "LCDlib.h"
//#include "LCDlib.c"
#include "uart.h"
//#include "uart.c"
#include "ring_buffer.h"
//#include "ring_buffer.c"
char msb, lsb;
unsigned char minhastring[2];
char recebido = 0;

void int2ascii(char entr);

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  /*
  BCSCTL1 = CALBC1_8MHZ;      // Set range
  DCOCTL = CALDCO_8MHZ;       // Set DCO step + modulation
  BCSCTL2 = DIVS_3;           // SMCLK / 8
  IFG1 &= ~OFIFG;             // Flag de falha do oscilador resetada
  */

  configureClock();
  initializePORT1();
  
  uart_config_t config;
    config.baud = 9600;

    if(uart_init(&config) != 0)		//If error in the uart config, trap!
    	while(1);
  

  

  init_Lcd(lcd_10x5|lcd_2_linha, lcd_cursor_fixo|lcd_cursor_desativo|lcd_ligado);
  

//char n = i;

      //for(i = '1'; i < n+50;i++){
    //lcd_Envia_Byte(dados, i);
    //lcd_Envia_Byte(dados, 'B');
  //}

  //lcd_Envia_String("Rita Maria ");
  lcd_Envia_String("Trabalho Micro  ");
  //lcd_Posiciona(1,2);
  go_2_linha();
  //lcd_Envia_Byte(0, 0x80|0x40);
  //lcd_Envia_String("Oscar Sa");
  lcd_Envia_String("II ");
  _BIS_SR(GIE);                   // GIE enable
  __delay_cycles(4*CLK);                   //AGUARDA 15 milisegundo


  minhastring[0] = '0';
  minhastring[1] = '0';

  while(1){
      /*uart_putchar('t');
      __delay_cycles(0.80*CLK);                   //AGUARDA 15 milisegundo
      P1OUT ^= BIT0;
      recebido = uart_getchar();
*/

      do{
          uart_putchar('t');
          __delay_cycles(1*CLK);                   //AGUARDA 15 milisegundo
          recebido = uart_getchar();
          P1OUT ^= BIT0;
      }while(recebido == -1);

/*
                  uart_putchar('t');
                              __delay_cycles(0.80*CLK);                   //AGUARDA 15 milisegundo
                              P1OUT ^= BIT0;
                              recebido = uart_getchar();
                              //minhastring[0] = recebido;

                              if(recebido != -1){*/
                                  __delay_cycles(.01*CLK);                   //AGUARDA 15 milisegundo
                                  //minhastring[1] = uart_getchar();
                                  int2ascii(recebido);
                                  go_1_linha();
                                  lcd_Envia_String("Temperatura: ");
                                  //go_2_linha();
                                  lcd_Envia_Byte(dados, minhastring[0]); lcd_Envia_Byte(dados, minhastring[1]);
                                  recebido = -1;
                        //}


                  uart_putchar('h');
            __delay_cycles(0.80*CLK);                   //AGUARDA 15 milisegundo
            P1OUT ^= BIT0;
            recebido = uart_getchar();
            //minhastring[0] = recebido;

            if(recebido != -1){
                __delay_cycles(.01*CLK);                   //AGUARDA 15 milisegundo
                //minhastring[1] = uart_getchar();
                int2ascii(recebido);
                go_2_linha();
                lcd_Envia_String("Humidade: ");
                //go_2_linha();
                lcd_Envia_Byte(dados, minhastring[0]); lcd_Envia_Byte(dados, minhastring[1]);
                recebido = -1;
      }

  }

}

void int2ascii(char entr){
    msb = (entr / 10);
    lsb = (entr - (msb*10));
    msb += '0';
    lsb += '0';
    minhastring[0] = msb;
    minhastring[1] = lsb;


}

