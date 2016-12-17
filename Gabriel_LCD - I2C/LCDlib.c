
#include "LCDlib.h"

char modo_lcd2;
/*
#ifndef __I2C__
union ubyte
{
  char _byte;
  struct
  {
    char b0 :1;
    char b1 :1;
    char b2 :1;
    char b3 :1;
    char b4 :1;
    char b5 :1;
    char b6 :1;
    char b7 :1;
  } bit;
};
#endif
*/
void lcd_Envia_String(unsigned char *c){
  
  while(*c){
    lcd_Envia_Byte(dados, *c);
    c++;
  }
}



void lcd_Posiciona(unsigned char x,unsigned char y){
  char endereco;
  
  if(y) endereco = lcd_segunda_linha; else endereco = 0;
  endereco += x-1;
  lcd_Envia_Byte(comando, 0x80|endereco);
}



static void lcd_Envia_4Bits(int dado){
#ifndef __I2C__
  /*
  union ubyte aux;
  aux._byte = dado;
  */
    //COLOCA OS 4 BITS NA SAIDA
  *P1OUT &= ~0xF0;
  P1OUT |= dado;
  /*lcd_d4 = aux.bit.b4;
  lcd_d5 = aux.bit.b5; 
  lcd_d6 = aux.bit.b6; 
  lcd_d7 = aux.bit.b7;
  */
  
  //PULSA O ENABLE
  lcd_enable = 1;
  lcd_enable = 0;
#endif
#ifdef __I2C__
  //COLOCA OS 4 BITS NA SAIDA
  UCB0TXBUF &= ~(db4_bit + db5_bit + db6_bit + db7_bit);
  UCB0TXBUF |= (dado<<4);
  enviaI2C();
  
  //PULSA O ENABLE  
  UCB0TXBUF |= enable_bit;
  enviaI2C();
  UCB0TXBUF &= ~enable_bit;
  enviaI2C();
  
#endif
}



void lcd_Envia_Byte(int tipo, int dado)
{
  #ifndef __I2C__
  lcd_rs = 0;                            //APAGA O PINO RS
  lcd_rs = tipo;                         //VERIFICA SE O QUE VAI SER ENVIADO SERA UM DADO OU UM COMANDO
  
  __delay_cycles(0.001*CLK);                   //AGUARDA 1 milisegundo
  lcd_enable = 0;                       //DESATIVA O PINO ENABLE
#endif
#ifdef __I2C__
  //UCB0TXBUF &= ~rw_bit;                        //APAGA  O PINO RW
  UCB0TXBUF &= ~(rs_bit+rw_bit);                        //APAGA  O PINO RS E RW
  enviaI2C();
  
  //lcd_rs = tipo;                         //VERIFICA SE O QUE VAI SER ENVIADO SERA UM DADO OU UM COMANDO
  UCB0TXBUF |= tipo;
  enviaI2C();
  
  __delay_cycles(0.001*CLK);                   //AGUARDA 1 milisegundo
  UCB0TXBUF &= ~enable_bit;
  enviaI2C();
#endif
  
  lcd_Envia_4Bits(dado >> 4);           //ENVIA A PRIMEIRA PARTE DO BYTE
  lcd_Envia_4Bits(dado & 0x0F);         //ENVIA A SEGUNDA PARTE DO BYTE
  
  /*
  lcd_Envia_4Bits((dado & 0x0F)<<4);    //ENVIA A PRIMEIRA PARTE DO BYTE
  lcd_Envia_4Bits(dado & 0xF0);         //ENVIA A SEGUNDA PARTE DO BYTE
  */
}



#ifdef __I2C__
void initI2Cpins(){
    //SAIDA DIGITAL
  P1DIR |= SCL;
  P1DIR |= SDA;
  
  P1SEL |= SCL;             //SCL
  P1SEL |= SDA;             //SDA
  //FUNCAO ALTERNATIVA 2
  P1SEL2 |= SCL;
  P1SEL2 |= SDA;
}



void initI2C(){
  
  while (UCB0CTL1 & UCTXSTP);                   //ESPERA O BIT DE STOP
  UCB0CTL1 |= UCSWRST;                          //DESABILITA O I2C
  
  UCB0CTL0 = UCMST+UCSYNC+UCMODE_3;             //MESTRE + SINCRONO + I2C
  
  UCB0CTL1 = UCSWRST+UCSSEL_2+UCTR;             //DESABILITA O I2C + SMCLK + MODO TRANSMISSAO
  
  UCB0I2CSA = endereco_escravo;                 //ENDERECO DO ESCRAVO
 // UCB0I2COA = 0x01A5;                         //ENDERECO PROPRIO 
  
  UCB0BR0 = 12;                                 //CLK/12
  UCB0BR1 = 0;
   
  UCB0CTL1 &= ~UCSWRST;                     //ATIVA O I2C
  
  UCB0TXBUF = 0;
  
  //IE2 |= UCB0TXIE;                              //ATIVA A INTERRUPCAO TX
}

void enviaI2C(){
  while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
  UCB0CTL1 |= UCTXSTT;                  //CODICAO DE INICIO
  UCB0TXBUF = UCB0TXBUF;
  while (UCB0CTL1 & UCTXSTT);             // Ensure stop condition got sent
  UCB0CTL1 |= UCTXSTP;                  //CODICAO DE PARADA
}
#endif



void init_Lcd(int modo1, int modo2)
{
#ifndef __I2C__
  //CONFIGURA OS PINOS COMO SAIDA
  lcd_enable_dir = 1;
  lcd_rs_dir = 1;
  lcd_d4_dir = 1;
  lcd_d5_dir = 1;
  lcd_d6_dir = 1;
  lcd_d7_dir = 1;
  
  //APAGA TODOS OS PINOS
  lcd_d4 = 0;
  lcd_d5 = 0;
  lcd_d6 = 0;
  lcd_d7 = 0;
  lcd_enable = 0;
  lcd_rs = 0;
#endif
#ifdef __I2C__
  initI2Cpins();                //INICIA OS PINOS DO I2C
  initI2C();                    //INICIA O I2C
  //APAGA TODOS OS PINOS
  UCB0TXBUF = back_light;
  enviaI2C();
#endif
  
  __delay_cycles(0.040*CLK);                   //AGUARDA 15 milisegundo
  
  //CONFIGURA O LCD PARA OPERAR NO MODO DE 4BITS
  char ctr;
  for(ctr = 0; ctr <3;ctr++){             //ENVIA TRES VEZES 0x03
    lcd_Envia_4Bits(0x3);
    __delay_cycles(0.005*CLK);                   //AGUARDA 5 milisegundo
  }
  lcd_Envia_4Bits(0x2);                   //E ENVIA 0x02
  
  //CONFIGURA O LCD
  lcd_Envia_Byte(comando,0x20|modo1);           //CONFIGURACAO DO DISPLAY
  lcd_Envia_Byte(comando,0x08|modo2);           //CONTROLE LIGA/DESLIGA DO DISPLAY
  modo_lcd2 = 0x08|modo2;
  lcd_Envia_Byte(comando,1);                    //APAGA A TELA
  lcd_Envia_Byte(comando,6);                    //CONFIGURA MODO - SENTIDO DE DESLOCAMENTO PARA A DIREITA
}
