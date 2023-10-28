
// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = HS     // Oscillator Selection bits (RC oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON     // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define _XTAL_FREQ 20000000
#define RS RC2
#define EN RC1
void init(void);
void i2c_init(const unsigned long);
void i2c_write(unsigned char);
int i2c_read(int);
void i2c_start();
void i2c_wait();
void i2c_stop();
void lcd_command(unsigned char);
void lcd_data(unsigned char);
int bcd_2_dec(int);
int dec_2_bcd(int);
void settime(void);
void update(void);
char msg1[5]={"TIME:"};
char msg2[5]={"DATE:"};
int i,j,k,l;
int sec=55;
int min=59;
int hour=23;
int date=28;
int month=12;
int year =20;
char sec1,sec2,min1,min2,hour1,hour2,date1,date2,month1,month2,year1,year2;
void main()
{
    init();
    i2c_init(100);
    settime();
    while(1)
    {
        update();
        sec1=sec/10;
        sec2=sec%10;
        min1=min/10;
        min2=min%10;
        hour1=hour/10;
        hour2=hour%10;
        date1=date/10;
        date2=date%10;
        month1=month/10;
        month2=month%10;
        year1=year/10;
        year2=year%10;
        
        lcd_command(0x80);
        for(i=0;i<5;i++) lcd_data(msg1[i]);
        lcd_data(hour1+'0');
        lcd_data(hour2+'0');
        lcd_data(0x2D);
        lcd_data(min1+'0');
        lcd_data(min2+'0');
        lcd_data(0x2D);
        lcd_data(sec1+'0');
        lcd_data(sec2+'0');
        lcd_command(0xC0);
        for(i=0;i<5;i++) lcd_data(msg2[i]);
        lcd_data(date1+'0');
        lcd_data(date2+'0');
        lcd_data(0x2D);
        lcd_data(month1+'0');
        lcd_data(month2+'0');
        lcd_data(0x2D);
        lcd_data(year1+'0');
        lcd_data(year2+'0');
    __delay_ms(500);
    }
    
}

void init(void)
{
    TRISD=0x00;
    TRISC=0x18;
    PORTD=0x00;
    lcd_command(0x38);
      __delay_ms(5);
      lcd_command(0x38);
      __delay_ms(5);
      lcd_command(0x38);
      __delay_ms(5);
      lcd_command(0x38);
      __delay_ms(5);
      lcd_command(0x0C);
      __delay_ms(5);
      lcd_command(0x06);
      __delay_ms(5);
      lcd_command(0x01);
      __delay_ms(5);
}
void lcd_command(unsigned char i)
{
    RS=0;
    PORTD=i;
    EN=1;
    //__delay_ms)(1);
    EN=0;
    __delay_ms(5);
}
void lcd_data(unsigned char i)
{
    RS=1;
    PORTD=i;
    EN=1;
    //__delay_ms)(1);
    EN=0;
    __delay_ms(5);
}
void i2c_init(const unsigned long feq_k)
{
    SSPCON=0x28;
    SSPSTAT=0x00;
    SSPCON2=0x00;
   SSPADD = (_XTAL_FREQ/(4*feq_k*100))-1;
}
void i2c_wait()
{
    while(SSPCON2 & 0x1F || SSPSTAT & 0x04);
}
void i2c_start()
{
   i2c_wait();
    SEN=1;
}
void i2c_stop()
{
    i2c_wait();
    PEN=1;
}
void i2c_write(unsigned char temp)
{
    i2c_wait();
    SSPBUF=temp;
}
int i2c_read(int ack)
{
    int value;
    i2c_wait();
    RCEN=1;
    i2c_wait();
    value=SSPBUF;
    i2c_wait();
    ACKDT=(ack)?0:1;
    ACKEN=1;
    return value;
    
}
int dec_2_bcd(int temp)
{ 
return ((temp/10)<<4)+(temp%10);

}
int bcd_2_dec(int temp)
{ 
return ((temp>>4)*10)+(temp&0x0F);
}
void settime(void)
{
    i2c_start();
    i2c_write(0xD0);
    i2c_write(0);
    i2c_write(dec_2_bcd(sec));
    i2c_write(dec_2_bcd(min));
    //i2c_write(0x40 | (dec_2_bcd(hour)));
    i2c_write(dec_2_bcd(hour));
    i2c_write(1);
    i2c_write(dec_2_bcd(date));
    i2c_write(dec_2_bcd(month));
    i2c_write(dec_2_bcd(year));
    i2c_stop();
}
void update(void)
{
    i2c_start();
    i2c_write(0xD0);
    i2c_write(0);
    i2c_stop();
    i2c_start();
    i2c_write(0xD1);
    sec=(bcd_2_dec(i2c_read(1)));
    min=(bcd_2_dec(i2c_read(1)));
    hour=(bcd_2_dec(i2c_read(1)));
    //hour=(bcd_2_dec(0xBF & (i2c_read(1))));
    (i2c_read(1));
    date=(bcd_2_dec(i2c_read(1)));
    month=(bcd_2_dec(i2c_read(1)));
    year=(bcd_2_dec(i2c_read(1)));
    i2c_stop();
    i2c_start();
    i2c_write(0xD1);
    i2c_read(1);
    i2c_stop();
}