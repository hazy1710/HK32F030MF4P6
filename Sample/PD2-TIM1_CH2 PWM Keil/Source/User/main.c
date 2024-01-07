/**
  ******************************************************************************
  * @file    main.c
  * @author  Alexander
  * @version V1.0
  * @date    2022-xx-xx
  * @brief   高级定时器-PWM互补输出带死区时间
  ******************************************************************************
  * @attention
  *
  * 实验平台:HK32F030M开发板 
  * 论坛    :https://bbs.21ic.com/iclist-1010-1.html
  *
  ******************************************************************************
  */ 
#include "hk32f030m.h" 
#include "bsp_led.h"
#include "bsp_AdvanceTim.h"
#include "systick_delay.h"

volatile uint32_t time = 0;  // ms 计时变量

uint8_t t = 0; 

#define SOFT_DELAY  Delay(0x0FFFFF);

void Delay(__IO uint32_t nCount);
void ADVANCE_TIM_Mode_Config(void);



/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */

static void PWM_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // PD1
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_3);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Disable;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  // PA1
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_3);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* ----------------   PWM Period And Duty Cycle  --------------- 
TIM_CLK = FCLK / (PSC+1)
PWM Frequency = FCLK / (PSC+1) / (ARR+1)
Duty Cycle = CCR1 / (ARR+1)
*/

void ADVANCE_TIM_Mode_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;

  // Enable Timer1 clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  // Auto-Reload ARR, Timer1 period, Timer1 frequency = Timer1 clock / (ARR + 1)
  TIM_TimeBaseStructure.TIM_Period = (256 - 1);
  // Prescaler PSC, Timer1 clock = FCLK / (PSC + 1)
  TIM_TimeBaseStructure.TIM_Prescaler = (32 - 1);
  // CR1_CKD, for dead-time insertion
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  // Counter mode
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  // RCR, Repetition Counter value
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  // Output Compare Mode 1
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  // Enable output
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  // Enable complementary output
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  // CCR1, Specifies the pulse value to be loaded into the Capture Compare Register
  TIM_OCInitStructure.TIM_Pulse = 4;
  // Output polarity
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  // Complementary output polarity
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  // Pin state during Idle state
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  // Complementary pin state during Idle state
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

  // Enable Timer1
  TIM_Cmd(TIM1, ENABLE);
  // Enable PWM output
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

int main(void)
{
  SOFT_DELAY   // Поскольку BKIN мультиплексируется на выводе PB5, а функцией включения PB по умолчанию является функция SWCLK SWD, чтобы предотвратить невозможность записи при следующем запуске, здесь добавлена задержка.
  SOFT_DELAY
	SOFT_DELAY
	
  uint8_t compare = 0;
	uint8_t up=1;

  delay_init();
  PWM_GPIO_Init();
  ADVANCE_TIM_Mode_Config();
  
    while (1)
	{
		if (compare==0) up=1;
		if (compare==255) up=0;
		if (up) TIM1->CCR2 = compare++;
		else TIM1->CCR2 = compare--;
    delay_ms(5);
	}
}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}




#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char* file , uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */	
       /* Infinite loop */
	
	while (1)
  {		
  }
}
#endif /* USE_FULL_ASSERT */


