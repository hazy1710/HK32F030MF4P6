#ifndef __BSP_ADVANCETIME_H
#define __BSP_ADVANCETIME_H


#include "hk32f030m.h"

/************高级定时器TIM参数定义，只限TIM1和TIM8************/
// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 这里我们使用高级控制定时器TIM1

#define            ADVANCE_TIM                   TIM1
#define            ADVANCE_TIM_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define            ADVANCE_TIM_CLK               RCC_APB2Periph_TIM1
// PWM 信号的频率 F = TIM_CLK/{(ARR+1)*(PSC+1)}
#define            ADVANCE_TIM_PERIOD            (100-1)
#define            ADVANCE_TIM_PSC               (64-1)
#define            ADVANCE_TIM_PULSE             4

#define            ADVANCE_TIM_IRQ               TIM1_UP_TRG_COM_IRQn
#define            ADVANCE_TIM_IRQHandler        TIM1_UP_TRG_COM_IRQHandler

// TIM1 输出比较通道
#define            ADVANCE_TIM_CH1_GPIO_CLK      RCC_AHBPeriph_GPIOD
#define            ADVANCE_TIM_CH1_PORT          GPIOD
#define            ADVANCE_TIM_CH1_PIN           GPIO_Pin_2
#define            ADVANCE_TIM_CH1_PinSource     GPIO_PinSource2
#define            ADVANCE_TIM_CH1_GPIO_AF       GPIO_AF_3    

// TIM1 输出比较通道的互补通道
#define            ADVANCE_TIM_CH1N_GPIO_CLK      RCC_AHBPeriph_GPIOA
#define            ADVANCE_TIM_CH1N_PORT          GPIOA
#define            ADVANCE_TIM_CH1N_PIN           GPIO_Pin_1
#define            ADVANCE_TIM_CH1N_PinSource     GPIO_PinSource1
#define            ADVANCE_TIM_CH1N_GPIO_AF       GPIO_AF_3

// TIM1 输出比较通道的刹车通道
#define            ADVANCE_TIM_BKIN_GPIO_CLK      RCC_AHBPeriph_GPIOB
#define            ADVANCE_TIM_BKIN_PORT          GPIOB
#define            ADVANCE_TIM_BKIN_PIN           GPIO_Pin_5
#define            ADVANCE_TIM_BKIN_PinSource     GPIO_PinSource5
#define            ADVACNE_TIM_BKIN_GPIO_AF       GPIO_AF_3


/**************************函数声明********************************/

void ADVANCE_TIM_Init(void);
void ADVANCE_TIM_Up(uint8_t t);


#endif	/* __BSP_ADVANCETIME_H */


