#ifndef __BSP_TIMEBASE_H
#define __BSP_TIMEBASE_H


#include "stm32f10x.h"


/********************基本定时器TIM参数定义，只限TIM6、7************/

#define            BASIC_TIM1                   TIM6
#define            BASIC_TIM_APBxClock_FUN1     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK1               RCC_APB1Periph_TIM6
#define            BASIC_TIM_Period1            (1000-1)
#define            BASIC_TIM_Prescaler1         71
#define            BASIC_TIM_IRQ1               TIM6_IRQn
#define            BASIC_TIM_IRQHandler1        TIM6_IRQHandler

#define            BASIC_TIM0                   TIM7
#define            BASIC_TIM_APBxClock_FUN0     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK0               RCC_APB1Periph_TIM7
#define            BASIC_TIM_Period0            1000-1
#define            BASIC_TIM_Prescaler0         71
#define            BASIC_TIM_IRQ0               TIM7_IRQn
#define            BASIC_TIM_IRQHandler0        TIM7_IRQHandler

/**************************函数声明********************************/

void timer_init(void);


#endif	/* __BSP_TIMEBASE_H */


