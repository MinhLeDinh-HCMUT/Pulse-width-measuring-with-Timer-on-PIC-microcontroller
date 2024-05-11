#include <16F877A.h>
#device ADC=16
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#use delay(crystal=20000000)
#define LCD_RS_PIN      PIN_D1
#define LCD_RW_PIN      PIN_D2
#define LCD_ENABLE_PIN  PIN_D3
#define LCD_DATA4       PIN_D4
#define LCD_DATA5       PIN_D5
#define LCD_DATA6       PIN_D6
#define LCD_DATA7       PIN_D7
#include <lcd.c>

int32 ovrflow=0;
#byte TMRO=0x01
#byte OPREG=0x81
#bit OPREG4=OPREG.4
#byte INTCON=0x0B
#byte PIE1=0x8C
#byte T1CON=0x10
#bit T1CON0=T1CON.0
#byte TMR1L=0x0E
#byte TMR1H=0x0F
#byte TRISA=0x85
#byte TRISD=0x88

#int_timer1
void ngat()
{
   ovrflow+=1;
   TMR1L=0;
   TMR1H=0;
}
void main()
{
   TRISA=0xff;
   TRISD=0x00;
   lcd_init();
   lcd_putc('\f');
   
   INTCON=0b11000000;
   PIE1=0b00000001;
   T1CON=0;
   TMR1L=0;
   TMR1H=0;
   OPREG=0b00101000; //low-to-high transition initially
   TMRO=0;
   
   int done=0;
   int32 valueT1,valueT2;
   float freq;
   
   while(TRUE)
   {
      while(TMRO<2);
      T1CON0=1;   //turn on timer1
      OPREG4=1;   //high-to-low transition
      while(TMRO<3);      
      valueT1=make16(TMR1H,TMR1L)+ovrflow*65536;
      OPREG4=0;   //low-to-high transition
      while(TMRO<4);
      T1CON0=0;   //turn off timer1
      valueT2=make16(TMR1H,TMR1L)+ ovrflow*65536;
      freq=5000000.0/valueT2;        
      if(done==0)
      {
         lcd_gotoxy(1,1);
         printf(lcd_putc,"FREQ:%fHz",freq);
         lcd_gotoxy(1,2);
         printf(lcd_putc,"HIGH:%lf%%",((double)valueT1/valueT2)*100);
         done=1;
      }
      ovrflow=0;
   }
}

