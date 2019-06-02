#include <reg52.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

/****************************************************/

sbit WAVE_OUT = P1^2;

uint T;
uint frq = 1000;

uchar tl0, th0;

void timer_init(void)
{
    TMOD |= 0x01;
    
    EA = ET0 = TR0 = 1;
}

void send_square_wave(uint frq)
{
    T = 65536 - 460800 / frq;
}
 
void timer0(void) interrupt 1
{
    tl0 = TH0 = T >> 8;
    th0 = TL0 = T;
    WAVE_OUT = !WAVE_OUT;
}

/**********************************************/

uchar usart_buf[40];
uchar cmd_ok = 0;

void usart_init(ulong bps)
{
    SCON = 0x50;
    TMOD |= 0x20;
    TH1 = 256-11059200/bps/32/12;
    TL1 = TH1;
    EA = ES = TR1 = 1;   
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

void set_frq(uint src_frq, int err_frq, uint *frq)
{    
    const char *str;
    uint v;
    
    if (err_frq < 0)
        str = "dec", v = abs(err_frq);
    else if (err_frq > 0)
        str = "inc", v = abs(err_frq);
    else
        str = "set", v = src_frq;
    
    *frq = src_frq + err_frq;
    send_square_wave(*frq);

    printf("%s %uHz; current %uHz\r\n", str, v, *frq);
    printf("T0: %bx %bx; T: %u\r\n", th0, tl0, T);
}
    
#define have_cmd(cmd) !strcmp(usart_buf, cmd)

void main(void)
{
    uint frq;
    usart_init(9600);
    timer_init();
    
    
    set_frq(1000, 0, &frq);

    while (1)
    {
        if (cmd_ok == 1)
        {
            if (have_cmd("A"))
            {
                if (frq + 100 <= 2000)
                    set_frq(frq, 100, &frq);
                else
                    printf("frq out of range\r\n");
            }       
            else if (have_cmd("B"))
            {
                if (frq - 100 >= 500)
                    set_frq(frq, -100, &frq);
                else
                    printf("frq out of range\r\n");
            }
            else if (have_cmd("C"))
                set_frq(1000, 0, &frq);
            else if (have_cmd("D"))
                set_frq(2000, 0, &frq);
            else if (have_cmd("E"))
                set_frq(2400, 0, &frq);
            else if (have_cmd("F"))
                set_frq(400, 0, &frq);
            else if (have_cmd("G"))
                set_frq(300, 0, &frq);
            else
                printf("unknown cmd\r\n");
            cmd_ok = 0;
        }
    }
}
