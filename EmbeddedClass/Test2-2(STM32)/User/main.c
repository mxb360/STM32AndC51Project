#include "stm32f10x.h"
#include "input.h"
#include "bsp_TiMbase.h"
#include "bsp_usart.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

#define WAVE_IN  GPIO_ReadInputDataBit(IN_GPIO_PORT, IN_GPIO_PIN)

uint count1 = 0;
uint count2 = 0;
ulong high_count;
ulong low_count;

uint get_T(void)
{
    return high_count + low_count;
}

uint get_frq(void)
{
    uint t = high_count + low_count;
    if (t)
        return 1000000 / t;
    return 0;
}

uint get_high(void)
{
    return high_count;
}

#define have_cmd(cmd) !strcmp(usart_buf, cmd)

void cmd_resolve(void)
{
    char buf[20];
    
    if (have_cmd("AA"))
        sprintf(buf, "frq: %uHz  t: %uus high: %uus\r\n", get_frq(), get_T(), get_high());
    else if (have_cmd("A"))
    {
        uint frq = get_frq();
        
        if (frq > 1000)
            sprintf(buf, "OVER\r\n");
        else
            sprintf(buf, "%uHz\r\n", frq);
    }       
    else if (have_cmd("B"))
    {
        uint frq = get_frq();
        
        if (frq < 1000)
            sprintf(buf, "LOW\r\n");
        else if (frq > 10000)
            sprintf(buf, "OVER\r\n");
        else
            sprintf(buf, "%gkHz\r\n", frq/1000.);
    }
    else if (have_cmd("C"))
    {
        uint t = get_T();
        
        if (t > 1000)
            sprintf(buf, "OVER\r\n");
        else
            sprintf(buf, "%gms\r\n", t/1000.);
    }
    else if (have_cmd("D"))
    {
        uint t = get_high();
        
        if (t > 1000)
            sprintf(buf, "OVER\r\n");
        else
            sprintf(buf, "%gms\r\n", t/1000.);
    }
    else
        sprintf(buf, "unknown cmd\r\n");
    usart_send_string(buf);
}

int main(void)
{
    uchar count;
    
    input_init();
    timer_init();
    usart_init(9600);
    
    while (WAVE_IN);
    while (!WAVE_IN);
    
    while (1)
    {    
        TIM_Cmd(BASIC_TIM1, DISABLE); //TR1 = 0;
        TIM_Cmd(BASIC_TIM0, ENABLE);  //TR0 = 1;
        while (WAVE_IN);        
        TIM_Cmd(BASIC_TIM0, DISABLE); //TR0 = 0;
        TIM_Cmd(BASIC_TIM1, ENABLE); //TR1 = 1;
        while (!WAVE_IN);
        TIM_Cmd(BASIC_TIM1, DISABLE);//TR1 = 0;
        TIM_Cmd(BASIC_TIM0, ENABLE);//TR0 = 1;
        
        if (++count == 10)
        {
            TIM_Cmd(BASIC_TIM0, DISABLE);
            TIM_Cmd(BASIC_TIM1, DISABLE);
            //TR0 = TR1 = 0;
            high_count = (TIM_GetCounter(BASIC_TIM0) + count1 * 65536)/10;
            low_count  = (TIM_GetCounter(BASIC_TIM1) + count2 * 65536)/10;
            count = count1 = count2 = 0;
            
            TIM_Cmd(BASIC_TIM0, DISABLE);
            TIM_Cmd(BASIC_TIM1, DISABLE);
            while (WAVE_IN);
            while (!WAVE_IN);
            TIM_SetCounter(BASIC_TIM0, 0);
            TIM_SetCounter(BASIC_TIM1, 0);
        }
    }
}

