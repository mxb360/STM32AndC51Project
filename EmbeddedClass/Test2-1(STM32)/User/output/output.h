#ifndef __OUTPUT_H
#define	__OUTPUT_H


#include "stm32f10x.h"

#define OUT_GPIO_PORT    	GPIOB			              /* GPIO�˿� */
#define OUT_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define OUT_GPIO_PIN		GPIO_Pin_5			        /* ���ӵ�SCLʱ���ߵ�GPIO */


void output_init(void);

#endif /* __OUTPUT_H */
