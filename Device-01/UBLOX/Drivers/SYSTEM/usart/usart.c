/**
 ****************************************************************************************************
 * @file        usart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       串口初始化代码(一般是串口1)，支持printf
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211103
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/ESP8266/atk_mw8266d_uart.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include <string.h>
#include "./BSP/ESP8266/atk_mw8266d.h"

/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if 1

#if (__ARMCC_VERSION >= 6010050)            /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


/* FILE 在 stdio.h里面定义. */
FILE __stdout;

// UASRT1
/* MDK下需要重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */

    USART_UX->DR = (uint8_t)ch;             /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}
#endif
/******************************************************************************************/

#if USART_EN_RX /*如果使能了接收*/

_SaveData Save_Data; // GPS数据对象

uint16_t point1 = 0; // RX_Buffer 接收到的数据长度

/* 接收缓冲, 最大USART_REC_LEN个字节. */
uint8_t USART_RX_BUF[USART_REC_LEN]; 

/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
*/
uint16_t USART_RX_STA=0;       //接收状态标记	  

uint8_t aRxBuffer;  /* HAL库使用的串口 单8位数据 接收缓冲 */

UART_HandleTypeDef g_uart1_handle;  /* UART句柄 */

/**
 * @brief       串口X初始化函数
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @note        注意: 必须设置正确的时钟源, 否则串口波特率就会设置异常.
 *              这里的USART的时钟源在sys_stm32_clock_init()函数中已经设置过了.
 * @retval      无
 */
void usart_init(uint32_t baudrate)
{
    /*UART 初始化设置*/
    g_uart1_handle.Instance = USART_UX;                                       /* USART_UX */
    g_uart1_handle.Init.BaudRate = baudrate;                                  /* 波特率 */
    g_uart1_handle.Init.WordLength = UART_WORDLENGTH_8B;                      /* 字长为8位数据格式 */
    g_uart1_handle.Init.StopBits = UART_STOPBITS_1;                           /* 一个停止位 */
    g_uart1_handle.Init.Parity = UART_PARITY_NONE;                            /* 无奇偶校验位 */
    g_uart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;                      /* 无硬件流控 */
    g_uart1_handle.Init.Mode = UART_MODE_TX_RX;                               /* 收发模式 */
    HAL_UART_Init(&g_uart1_handle);                                           /* HAL_UART_Init()会使能UART1 */

    
    HAL_UART_Receive_IT(&g_uart1_handle,(uint8_t *)&aRxBuffer, RXBUFFERSIZE); 
}

/**
 * @brief       UART底层初始化函数
 * @param       huart: UART句柄类型指针
 * @note        此函数会被HAL_UART_Init()调用
 *              完成时钟使能，引脚配置，中断配置
 * @retval      无
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;

    if (huart->Instance == USART_UX)                            /* 如果是串口1，进行串口1 MSP初始化 */
    {
        USART_TX_GPIO_CLK_ENABLE();                             /* 使能串口TX脚时钟 */
        USART_RX_GPIO_CLK_ENABLE();                             /* 使能串口RX脚时钟 */
        USART_UX_CLK_ENABLE();                                  /* 使能串口时钟 */

        gpio_init_struct.Pin = USART_TX_GPIO_PIN;               /* 串口发送引脚号 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* IO速度设置为高速 */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct);
                
        gpio_init_struct.Pin = USART_RX_GPIO_PIN;               /* 串口RX脚 模式设置 */
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;    
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct);   /* 串口RX脚 必须设置成输入模式 */
        
#if USART_EN_RX
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);                      /* 使能USART1中断通道 */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 3, 3);              /* 组2，最低优先级:抢占优先级3，子优先级3 */
#endif
    }
		else if (huart->Instance == ATK_MW8266D_UART_INTERFACE)                 /* 如果是ATK-MW8266D UART */
    {
        ATK_MW8266D_UART_TX_GPIO_CLK_ENABLE();                              /* 使能UART TX引脚时钟 */
        ATK_MW8266D_UART_RX_GPIO_CLK_ENABLE();                              /* 使能UART RX引脚时钟 */
        ATK_MW8266D_UART_CLK_ENABLE();                                      /* 使能UART时钟 */
        
        gpio_init_struct.Pin    = ATK_MW8266D_UART_TX_GPIO_PIN;             /* UART TX引脚 */
        gpio_init_struct.Mode   = GPIO_MODE_AF_PP;                          /* 复用推挽输出 */
        gpio_init_struct.Pull   = GPIO_NOPULL;                              /* 无上下拉 */
        gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;                     /* 高速 */
        HAL_GPIO_Init(ATK_MW8266D_UART_TX_GPIO_PORT, &gpio_init_struct);    /* 初始化UART TX引脚 */
        
        gpio_init_struct.Pin    = ATK_MW8266D_UART_RX_GPIO_PIN;             /* UART RX引脚 */
        gpio_init_struct.Mode   = GPIO_MODE_INPUT;                          /* 输入 */
        gpio_init_struct.Pull   = GPIO_NOPULL;                              /* 无上下拉 */
        gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;                     /* 高速 */
        HAL_GPIO_Init(ATK_MW8266D_UART_RX_GPIO_PORT, &gpio_init_struct);    /* 初始化UART RX引脚 */
        
        HAL_NVIC_SetPriority(ATK_MW8266D_UART_IRQn, 0, 0);                  /* 抢占优先级0，子优先级0 */
        HAL_NVIC_EnableIRQ(ATK_MW8266D_UART_IRQn);                          /* 使能UART中断通道 */
        
        __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);                          /* 使能UART接收中断 */
        __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);                          /* 使能UART总线空闲中断 */
    }
}

/**
 * @brief       串口数据接收回调函数
                数据处理在这里进行
 * @param       huart:串口句柄
 * @retval      无
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART_UX) /* 如果是串口1 */
    {
        if (aRxBuffer == '$')
        {
            point1 = 0;
        }
        USART_RX_BUF[point1++] = aRxBuffer;
				
				// 确定是否收到"GPRMC/GNRMC"这一帧数据
        if (USART_RX_BUF[0] == '$' && USART_RX_BUF[4] == 'M' && USART_RX_BUF[5] == 'C') 
        {
            if (aRxBuffer == '\n')
            {
                memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length); // 清空
                memcpy(Save_Data.GPS_Buffer, USART_RX_BUF, point1); // 保存数据
                Save_Data.isGetData = true;
                point1 = 0;
                memset(USART_RX_BUF, 0, USART_REC_LEN); // 清空
            }
        }

        if (point1 >= USART_REC_LEN)
        {
            point1 = USART_REC_LEN;
        }

        // 		USART_RX_STA|=0x8000;	//接收完成了
        // 		if((USART_RX_STA&0x8000)==0)//接收未完成
        // 		{
        // 			if(USART_RX_STA&0x4000)//接收到了0x0d
        // 			{
        // 				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
        // 				else USART_RX_STA|=0x8000;	//接收完成了 			//bit31表明是否接收到0x0a(\n)
        // 			}
        // 			else //还没收到0X0D
        // 			{
        // 				if(Res==0x0d)USART_RX_STA|=0x4000;						//bit30表明是否接收到0x0d(\r)
        // 				else
        // 				{
        // 					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
        // 					USART_RX_STA++;
        // 					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收
        // 				}
        // 			}
        // 		}
    }
}

/**
 * @brief       串口X中断服务函数
                注意,读取USARTx->SR能避免莫名其妙的错误
 * @param       无
 * @retval      无
 */
void USART_UX_IRQHandler(void)
{

    HAL_UART_IRQHandler(&g_uart1_handle);                               /* 调用HAL库中断处理公用函数 */

    while (HAL_UART_Receive_IT(&g_uart1_handle,(uint8_t *)&aRxBuffer, RXBUFFERSIZE) != HAL_OK)     /* 重新开启中断并接收数据 */
    {
        /* 如果出错会卡死在这里 */
    }
}

/******** UBLOX 函数区*******/
uint8_t Hand(char *a)                   // 串口命令识别函数
{ 
    if(strstr(USART_RX_BUF,a)!=NULL)
	    return 1;
	else
		return 0;
}

void CLR_Buf(void)                           // 串口缓存清理
{
	memset(USART_RX_BUF, 0, USART_REC_LEN);      //清空
  point1 = 0;                    
}

void clrStruct()
{
	Save_Data.isGetData = false;
	Save_Data.isParseData = false;
	Save_Data.isUsefull = false;
	memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      //清空
	memset(Save_Data.UTCTime, 0, UTCTime_Length);
	memset(Save_Data.latitude, 0, latitude_Length);
	memset(Save_Data.N_S, 0, N_S_Length);
	memset(Save_Data.longitude, 0, longitude_Length);
	memset(Save_Data.E_W, 0, E_W_Length);
	
}

void errorLog(int num)
{
	
	while (1)
	{
	  	printf("ERROR%d\r\n",num);
	}
}

void parseGpsBuffer(void)
{
	char* subString;
	char* subStringNext;
	char i = 0;
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
		printf("**************\r\n");
		printf(Save_Data.GPS_Buffer);

		
		for (i = 0 ; i <= 6 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					errorLog(1);	//解析错误
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
						case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//获取UTC时间
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	//获取N/S
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//获取经度信息
						case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	//获取E/W

						default:break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if(usefullBuffer[0] == 'A')
						Save_Data.isUsefull = true;
					else if(usefullBuffer[0] == 'V')
						Save_Data.isUsefull = false;

				}
				else
				{
					errorLog(2);	//解析错误
				}
			}


		}
	}
}

//void printGpsBuffer(void)
//{
//		if (Save_Data.isParseData)
//		{
//			Save_Data.isParseData = false;
//			
//			printf("Save_Data.UTCTime = ");
//			printf(Save_Data.UTCTime);
//			printf("\r\n");

//		if(Save_Data.isUsefull)
//		{
//			Save_Data.isUsefull = false;
//			printf("Save_Data.latitude = ");
//			printf(Save_Data.latitude);
//			printf("\r\n");


//			printf("Save_Data.N_S = ");
//			printf(Save_Data.N_S);
//			printf("\r\n");

//			printf("Save_Data.longitude = ");
//			printf(Save_Data.longitude);
//			printf("\r\n");

//			printf("Save_Data.E_W = ");
//			printf(Save_Data.E_W);
//			printf("\r\n");
//		}
//		else
//		{
//			printf("GPS DATA is not usefull!\r\n");
//		}
//		
//	}
//}




void uploadGpsBuffer(void)
{
		if (Save_Data.isParseData)
		{
			Save_Data.isParseData = false;
			
			atk_mw8266d_uart_printf("Save_Data.UTCTime = ");
			atk_mw8266d_uart_printf(Save_Data.UTCTime);
			atk_mw8266d_uart_printf("\r\n");

		if(Save_Data.isUsefull)
		{
			Save_Data.isUsefull = false;
			atk_mw8266d_uart_printf("Save_Data.latitude = ");
			atk_mw8266d_uart_printf(Save_Data.latitude);
			atk_mw8266d_uart_printf("\r\n");


			atk_mw8266d_uart_printf("Save_Data.N_S = ");
			atk_mw8266d_uart_printf(Save_Data.N_S);
			atk_mw8266d_uart_printf("\r\n");

			atk_mw8266d_uart_printf("Save_Data.longitude = ");
			atk_mw8266d_uart_printf(Save_Data.longitude);
			atk_mw8266d_uart_printf("\r\n");

			atk_mw8266d_uart_printf("Save_Data.E_W = ");
			atk_mw8266d_uart_printf(Save_Data.E_W);
			atk_mw8266d_uart_printf("\r\n");
		}
		else
		{
			atk_mw8266d_uart_printf("GPS DATA is not usefull!\r\n");
		}
		
	}
}




#endif
