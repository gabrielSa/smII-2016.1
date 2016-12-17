
#include <msp430.h>

#define __I2C__

//#define endereco_escravo 0x3F
#define endereco_escravo 0x27

#define CLK 8000000
#define lcd_segunda_linha 0x40

#define SCL BIT6
#define SDA BIT7

#ifndef __I2C__
//DEFINE OS PINOS DE SAIDA
#define lcd_d4 P1OUT_bit.P0
#define lcd_d5 P1OUT_bit.P3
#define lcd_d6 P1OUT_bit.P4
#define lcd_d7 P1OUT_bit.P5

#define lcd_rs P2OUT_bit.P0

#define lcd_enable P2OUT_bit.P2


//DIRECAO DOS PINOS
#define lcd_d4_dir P1DIR_bit.P0
#define lcd_d5_dir P1DIR_bit.P3
#define lcd_d6_dir P1DIR_bit.P4
#define lcd_d7_dir P1DIR_bit.P5

#define lcd_rs_dir P2DIR_bit.P0

#define lcd_enable_dir P2DIR_bit.P2

#endif
#ifdef __I2C__

#define rs_bit                   (0x01)
#define rw_bit                   (0x02)
#define enable_bit               (0x04)
#define back_light               (0x08)
#define db4_bit                  (0x10)
#define db5_bit                  (0x20)
#define db6_bit                  (0x40)
#define db7_bit                  (0x80)


void initI2Cpins(void);
void initI2C(void);
void enviaI2C(void);

#endif


#define go_2_linha() lcd_Envia_Byte(comando, 0x80|0x40);
#define go_1_linha() lcd_Envia_Byte(comando, 0x80|0);


#define comando 0
#define dados 1


//MODOS 
//CONFIGURACAO DE DISPLAY
#define lcd_8x5 0
#define lcd_10x5 4
#define lcd_1_linha 0
#define lcd_2_linha 8

//CONTROLE DISPLAY
#define lcd_cursor_piscante 1
#define lcd_cursor_fixo 0
#define lcd_cursor_ativo 2
#define lcd_cursor_desativo 0
#define lcd_ligado 4
#define lcd_desligado 0

static void lcd_Envia_4Bits(int dado);

void lcd_Envia_Byte(int tipo, int dado);

void init_Lcd(int modo1, int modo2);
