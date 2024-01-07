#include "usart_printf.h"

/* 注意：此方式的printf函数重定向，需要在Keil里勾选 Use MicroLIB,否则程序会卡在printf调用上。
*/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)


/*******************************************************************************
*                           @2017-12-16
* Function Name  : Usart1_Printf_Init
* Description    : printf重定向初始化
* Input          : baudrate -- 串口波特率值
* Output         : none
* Return         : none
*******************************************************************************/
void Usart1_Printf_Init(uint32_t baudrate)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
  
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOD, ENABLE);

  /* Enable USART1 Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1时钟

  /* USART1 Pins configuration */
  /* Connect pin to Perihp */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);   // TX
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_1);   // RX

  /* Configure pins as AF pushpull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;        // 字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1;             // 一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;                // 无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件数据流控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	   // 收发模式
  USART_Init(USART1, &USART_InitStructure);                          // 初始化串口

  USART_Cmd(USART1, ENABLE);                                         //使能串口 
}

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until transmit data register is empty */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {}

  return ch;
}
