#ifndef __OUTPUT_H
#define	__OUTPUT_H


#include "stm32f10x.h"

#define IN_GPIO_PORT    GPIOB			              /* GPIO�˿� */
#define IN_GPIO_CLK 	RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define IN_GPIO_PIN		GPIO_Pin_6			        /* ���ӵ�SCLʱ���ߵ�GPIO */


void input_init(void);

#endif /* __OUTPUT_H */
