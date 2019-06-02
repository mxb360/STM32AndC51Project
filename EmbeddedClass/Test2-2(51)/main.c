#include <reg52.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

/****************************************************/

sbit WAVE_IN = P1^3;

uint count1 = 0;
uint count2 = 0;
ulong high_count;
ulong low_count;

void timer_init(void)
{
    TMOD = 0x11;
    TH0 = TL0 = 0;
    TH1 = TL1 = 0;
    ET1 = ET0 = 1;
    EA = 1;
}

void timer0(void) interrupt 1 
{ 
    if (++count1 == 10)
    {
        high_count = low_count = 0;
        count1 = 0;
    }        
}

void timer1(void) interrupt 3 
{ 
    if (++count2 == 10)
    {
        high_count = low_count = 0;
        count2 = 0;
    }  
}


uint get_T(void)
{
    return (high_count + low_count) * 12 / 11.0592;
}

uint get_frq(void)
{
    uint t = ((high_count + low_count) * 12 / 11.0592);
    if (t)
        return 1000000 / t;
    return 0;
}

uint get_high(void)
{
    return high_count * 12 / 11.0592;
}

/**********************************************/

uchar usart_buf[40];

void usart_init(ulong bps)
{
    SCON = 0x50;
    TH2 = RCAP2H = 65536-11059200/bps/32 >> 8;
    TL2 = RCAP2L = 65536-11059200/bps/32;
    T2CON = 0x34;
    EA = ES = 1;   
}

#define have_cmd(cmd) !strcmp(usart_buf, cmd)

char putchar(char ch)
{
    SBUF = ch;
    while (!TI);
    TI = 0; 
    return ch;
}

void cmd_resolve()
{
    if (have_cmd("AA"))
        printf("frq: %uHz  t: %uus high: %uus\r\n", get_frq(), get_T(), get_high());
    else if (have_cmd("A"))
    {
        uint frq = get_frq();
        
        if (frq > 1000)
            printf("OVER\r\n");
        else
            printf("%uHz\r\n", frq);
    }       
    else if (have_cmd("B"))
    {
        uint frq = get_frq();
        
        if (frq < 1000)
            printf("LOW\r\n");
        else if (frq > 10000)
            printf("OVER\r\n");
        else
            printf("%gkHz\r\n", frq/1000.);
    }
    else if (have_cmd("C"))
    {
        uint t = get_T();
        
        if (t > 1000)
            printf("OVER\r\n");
        else
            printf("%gms\r\n", t/1000.);
    }
    else if (have_cmd("D"))
    {
        uint t = get_high();
        
        if (t > 1000)
            printf("OVER\r\n");
        else
            printf("%gms\r\n", t/1000.);
    }
    else
        printf("unknown cmd\r\n");
}

void usart_interrupt(void) interrupt 4
{
    static uchar top;
 
    if (RI)
    {
        if (top == 20)
            top = 0;
        
        if (SBUF == '\n')
        {
            usart_buf[top] = 0;
            if (top && usart_buf[top - 1] == '\r')
                usart_buf[top - 1] = 0;
            top = 0;
            cmd_resolve();
        }
        else
            usart_buf[top++] = SBUF;
        
        RI = 0;
    }
}

/***************************************************/
void main(void)
{
    uchar count;
    
    timer_init();
    usart_init(9600);
    
    while (WAVE_IN);
    while (!WAVE_IN);
    
    while (1)
    {    
        TR1 = 0;
        TR0 = 1;
        while (WAVE_IN);        
        TR0 = 0;
        TR1 = 1;
        while (!WAVE_IN);
        TR1 = 0;
        TR0 = 1;
        
        if (++count == 10)
        {
            TR0 = TR1 = 0;
            high_count = ((TH0 << 8 | TL0) + count1 * 65536)/10;
            low_count  = ((TH1 << 8 | TL1) + count2 * 65536)/10;
            count = count1 = count2 = 0;
            
            TR0 = TR1 = 1;
            while (WAVE_IN);
            while (!WAVE_IN);
            TH1 = TL1 = TH0 = TL0 = 0;
        }
    }
}
