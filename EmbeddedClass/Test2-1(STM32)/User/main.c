#include "stm32f10x.h"
#include "output.h"
#include "bsp_TiMbase.h"
#include "bsp_usart.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

void send_square_wave(uint frq)
{
    uint arr;
    
    arr = 500000 / frq;
    
    BASIC_TIM_Init(arr);
}

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
}
    
#define have_cmd(cmd) !strcmp(usart_buf, cmd)

int main(void)
{
    uint frq;
    
    usart_init(9600);
    output_init();
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
