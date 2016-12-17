
#include <msp430.h>
#include "LCDlib.h"
#include "uart.h"
#include "ring_buffer.h"


#define TEMPERATURA 1
#define UMIDADE 0

char msb, lsb;
unsigned char minhastring[2];
char recebido = 0;

void int2ascii(char entr);
void Solicitar(int Solicitacao);
void Exibe(int manda);

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;


  configureClock();         //CONFIGURA O CLOCK
  initializePORT1();        //INICIALIZA A PORT1
  
  //CONFIGURA O BAUD RATE
  uart_config_t config;
    config.baud = 9600;

    //INICIA A UART
    if(uart_init(&config) != 0)		//If error in the uart config, trap!
    	while(1);
  
  //INICIA O LCD
  init_Lcd(lcd_10x5|lcd_2_linha, lcd_cursor_fixo|lcd_cursor_desativo|lcd_ligado);

//ENVIA O TEXTO INICIAL
  lcd_Envia_String("Trabalho Micro  ");
  go_2_linha();
  lcd_Envia_String("II ");
  _BIS_SR(GIE);                   // GIE enable
  __delay_cycles(4*CLK);                   //AGUARDA 4 segundos


  minhastring[0] = '0';
  minhastring[1] = '0';

  while(1){
      Solicitar( TEMPERATURA );
      Exibe( TEMPERATURA );


      Solicitar( UMIDADE );
      Exibe( UMIDADE );
  }

}

//FUNCAO PARA CONVERTER O VALOR QUE CHEGA PARA ASCII
void int2ascii(char entr){
    msb = (entr / 10);
    lsb = (entr - (msb*10));
    msb += '0';
    lsb += '0';
    minhastring[0] = msb;
    minhastring[1] = lsb;


}

void Exibe( int manda ){
    __delay_cycles(.01*CLK);                   //AGUARDA 10 milisegundo
    int2ascii(recebido);                       //CONVERTE O VALOR RECEBIDO PARA ASCII
    if(manda){                                 //VERIFICA SE EH PRA EXIBIR A TEMPERATURA OU A HUMIDADE
        go_1_linha();                          //EXIBE A TEMPERATURA NA LINHA 1
        lcd_Envia_String("Temperatura: ");
    }else{
        go_2_linha();                          //EXIBE A TEMPERATURA NA LINHA 1
        lcd_Envia_String("Humidade: ");
    }
   //ENVIA OS VALORES
    lcd_Envia_Byte(dados, minhastring[0]); lcd_Envia_Byte(dados, minhastring[1]);
    recebido = -1;      //RESETA A VARIAVEL RECEBIDA
}

void Solicitar(int Solicitacao){
    char k;
    //VERIFICA SE FOI PEDIDO TEMPERATURA OU HUMIDADE
    if(Solicitacao)
        k = 't';
    else
    k = 'h';

    //FICA ENVIANDO O BITE DE PEDIDO ATE RECEBER RESPOSTA
    do{
              uart_putchar(k);                          //ENVIA O BITE DE SOLICITACAO
              __delay_cycles(1*CLK);                   //AGUARDA 1 segundo
              recebido = uart_getchar();               //VERIFICA O QUE RECEBEU
              P1OUT ^= BIT0;                           //FICA PISCANDO O LED
          }while(recebido == -1);

}
