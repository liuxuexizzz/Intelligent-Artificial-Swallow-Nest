
#ifndef __ATK_MW8266D_UART_H
#define __ATK_MW8266D_UART_H

#include "./SYSTEM/sys/sys.h"

/* 引脚定义 */
#define ATK_MW8266D_UART_TX_GPIO_PORT           GPIOA
#define ATK_MW8266D_UART_TX_GPIO_PIN            GPIO_PIN_2
#define ATK_MW8266D_UART_TX_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* PC口时钟使能 */

#define ATK_MW8266D_UART_RX_GPIO_PORT           GPIOA
#define ATK_MW8266D_UART_RX_GPIO_PIN            GPIO_PIN_3
#define ATK_MW8266D_UART_RX_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* PD口时钟使能 */

#define ATK_MW8266D_UART_INTERFACE              USART2
#define ATK_MW8266D_UART_IRQn                   USART2_IRQn
#define ATK_MW8266D_UART_IRQHandler             USART2_IRQHandler
#define ATK_MW8266D_UART_CLK_ENABLE()           do{ __HAL_RCC_USART2_CLK_ENABLE(); }while(0) /* UART5时钟使能 */

/* UART收发缓冲大小 */
#define ATK_MW8266D_UART_RX_BUF_SIZE            128
#define ATK_MW8266D_UART_TX_BUF_SIZE            64

/* 操作函数 */
void atk_mw8266d_uart_printf(char *fmt, ...);       /* ATK-MW8266D UART printf */
void atk_mw8266d_uart_rx_restart(void);             /* ATK-MW8266D UART重新开始接收数据 */
uint8_t *atk_mw8266d_uart_rx_get_frame(void);       /* 获取ATK-MW8266D UART接收到的一帧数据 */
uint16_t atk_mw8266d_uart_rx_get_frame_len(void);   /* 获取ATK-MW8266D UART接收到的一帧数据的长度 */
void atk_mw8266d_uart_init(uint32_t baudrate);      /* ATK-MW8266D UART初始化 */

#endif
