#include <reg52.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

/****************************************************/

//sbit WAVE_IN = P1^3;

uint T;
uint count = 0;

void timer_init(void)
{
    TMOD = 0x15;
    TH0 = TL0 = 0;
    TH1 = 0x4c;
    TL1 = 0x00;
    ET1 = TR1 = 1;
    ET0 = TR0 = 1;
    EA = 1;
}

void timer0(void) interrupt 1
{
    count++;
}

void timer1(void) interrupt 3
{
    static uchar t;
    TH1 = 0x4c;
    TL1 = 0x00;
    
    
    if (++t == 20)
    {
        TR1 = TR0 = 0;
        T = (TH0 << 8 | TL0) + 65536 * count;
        count = 0;
        TL0 = TH0 = 0;
        TR1 = TR0 = 1;

        t = 0;
    }
}


uint get_T(void)
{
    return 1000000 / T;
}

uint get_frq(void)
{
    return T;
}

/**********************************************/

uchar usart_buf[40];
uchar cmd_ok = 0;

void usart_init(ulong bps)
{
    SCON = 0x50;
    TH2 = RCAP2H = 65536-11059200/bps/32 >> 8;
    TL2 = RCAP2L = 65536-11059200/bps/32;
    T2CON = 0x34;
    EA = ES = 1;   
}

void usart_interrupt(void) interrupt 4
{
    static uchar top;
 
    if (RI)
    {
        if (cmd_ok)
            return;
        if (top == 20)
            top = 0;
        
        if (SBUF == '\n')
        {
            usart_buf[top] = 0;
            if (top && usart_buf[top - 1] == '\r')
                usart_buf[top - 1] = 0;
            top = 0;
            cmd_ok = 1;
        }
        else
            usart_buf[top++] = SBUF;
        
        RI = 0;
    }
}

char putchar(char ch)
{
    SBUF = ch;
    while (!TI);
    TI = 0; 
    return ch;
}

/***************************************************/

#define have_cmd(cmd) !strcmp(usart_buf, cmd)

void main(void)
{
    timer_init();
    usart_init(9600);
    
    while (1)
    {
        if (cmd_ok == 1)
        {
            if (have_cmd("AA"))
                printf("frq: %uHz  t: %uus\r\n", get_frq(), get_T());
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
                    printf("%gHz\r\n", t/1000.);
            }
            else if (have_cmd("D"))
            {
                uint t = get_T();
                
                if (t > 1000)
                    printf("OVER\r\n");
                else
                    printf("%gHz\r\n", t/1000.);
            }
            else
                printf("unknown cmd\r\n");
            cmd_ok = 0;
        }
    }
}
