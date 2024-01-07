
#include "bsp_AdvanceTim.h" 

static void ADVANCE_TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  // Инициализация GPIO канала сравнения выходных данных
  RCC_AHBPeriphClockCmd(ADVANCE_TIM_CH1_GPIO_CLK, ENABLE);
  GPIO_PinAFConfig(ADVANCE_TIM_CH1_PORT, ADVANCE_TIM_CH1_PinSource, ADVANCE_TIM_CH1_GPIO_AF);
  
  GPIO_InitStructure.GPIO_Pin = ADVANCE_TIM_CH1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Schmit = GPIO_Schmit_Disable;
  GPIO_Init(ADVANCE_TIM_CH1_PORT, &GPIO_InitStructure);
  
  // Сравнение выходных данных инициализация GPIO дополнительного канала
  RCC_AHBPeriphClockCmd(ADVANCE_TIM_CH1N_GPIO_CLK, ENABLE);
  GPIO_PinAFConfig(ADVANCE_TIM_CH1N_PORT, ADVANCE_TIM_CH1N_PinSource, ADVANCE_TIM_CH1N_GPIO_AF);
  
  GPIO_InitStructure.GPIO_Pin = ADVANCE_TIM_CH1N_PIN;
  GPIO_Init(ADVANCE_TIM_CH1N_PORT, &GPIO_InitStructure);
  
  
  // Инициализация GPIO канала сравнения выходных данных тормозного канала
  RCC_AHBPeriphClockCmd(ADVANCE_TIM_BKIN_GPIO_CLK, ENABLE);
  GPIO_PinAFConfig(ADVANCE_TIM_BKIN_PORT, ADVANCE_TIM_BKIN_PinSource, ADVACNE_TIM_BKIN_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = ADVANCE_TIM_CH1N_PIN;
	
  // Из-за выдвижения штифта BKIN значение по умолчанию низкое
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(ADVANCE_TIM_BKIN_PORT, &GPIO_InitStructure);
}


///*
// * Примечание: В структуре TIM_TimeBaseInitTypeDef есть 5 элементов, и они есть только в регистре TIM6.
// * TIM_Prescaler и TIM_Period, поэтому при использовании TIM6 и TIM7 вам нужно только инициализировать эти два элемента，
// * Остальные три элемента - это таймер общего назначения и расширенный таймер.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            У всех есть
// *	TIM_CounterMode			     TIMx,x[6]Нет, есть и другие
// *  TIM_Period               У всех есть
// *  TIM_ClockDivision        TIMx,x[6]Нет, есть и другие
// *  TIM_RepetitionCounter    TIMx,x[1]Только иметь
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ---------------- Расчет периода ШИМ-сигнала и рабочего цикла--------------- */
// ARR: Автоматическая перезагрузка значения регистра
//CLK_cnt: Тактовая частота счетчика, равная Fck_int/(psc+1) =72M/(psc+1)
//Период ШИМ-сигнала T= (ARR+1) * (1/CLK_cnt) = (ARR+1)* (PSC+1) /72 М
//Рабочий цикл P=CCR/(ARR+1)

static void ADVANCE_TIM_Mode_Config(void)
{
  // Включите часы таймера, то есть внутренние часы CK_INT=72M
	ADVANCE_TIM_APBxClock_FUN(ADVANCE_TIM_CLK,ENABLE);

/*--------------------Инициализация структуры временной базы-------------------------*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// Автоматически перезагружает значение регистра и генерирует обновление или прерывание после накопления частоты TIM_Period+1
	TIM_TimeBaseStructure.TIM_Period=ADVANCE_TIM_PERIOD;	
	//Часы, которые управляют счетчиком CNT=Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= ADVANCE_TIM_PSC;	
	// Коэффициент деления тактовой частоты, который необходимо использовать при настройке времени простоя
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// Режим подсчета счетчика, установленный на обратный отсчет
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// Повторите значение счетчика, не волнуйтесь, если это бесполезно
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// Инициализировать таймер
	TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);


	/*--------------------Инициализация структуры сравнения выходных данных-------------------*/		
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// Сконфигурирован как режим ШИМ 1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	// Включение вывода
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// Включен дополнительный вывод
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
	// Установите размер рабочего цикла
	TIM_OCInitStructure.TIM_Pulse = ADVANCE_TIM_PULSE;
	// Настройка полярности уровня выходного канала
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	// Конфигурация полярности уровня дополнительного выходного канала
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	// Настройка полярности уровня холостого хода выходного канала
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	// Конфигурация полярности уровня холостого хода дополнительного выходного канала
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(ADVANCE_TIM, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(ADVANCE_TIM, TIM_OCPreload_Enable);

	/*-------------------Инициализация структуры тормозов и мертвых зон-------------------*/
	// Для получения подробной информации об элементах структуры тормозов и мертвой зоны, пожалуйста, обратитесь к описанию реестра BDTR
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
	// На выходе сравнивается конфигурация времени простоя сигнала. Для конкретных расчетов, пожалуйста, обратитесь к описанию BDTR:UTG[7:0].
	// Заданное здесь время простоя составляет 152 секунды
  TIM_BDTRInitStructure.TIM_DeadTime = 11;
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
	// Когда вывод BKIN обнаруживает высокий уровень, выходной сигнал сравнения отключается, как если бы это был тормоз.
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
  TIM_BDTRConfig(ADVANCE_TIM, &TIM_BDTRInitStructure);
	
	// Включить счетчик
	TIM_Cmd(ADVANCE_TIM, ENABLE);	
	// Основной вывод включен, когда используется универсальный таймер, это предложение не требуется
	TIM_CtrlPWMOutputs(ADVANCE_TIM, ENABLE);
}

void ADVANCE_TIM_Init(void)
{
	ADVANCE_TIM_GPIO_Config();
	ADVANCE_TIM_Mode_Config();		
}

/*********************************************END OF FILE**********************/
