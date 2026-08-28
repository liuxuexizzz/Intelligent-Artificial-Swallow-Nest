
#ifndef __ATK_MW8266D_H
#define __ATK_MW8266D_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/ESP8266/atk_mw8266d_uart.h"

/* 引脚定义 */
#define ATK_MW8266D_RST_GPIO_PORT           GPIOB
#define ATK_MW8266D_RST_GPIO_PIN            GPIO_PIN_0
#define ATK_MW8266D_RST_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PC口时钟使能 */

/* IO操作 */
#define ATK_MW8266D_RST(x)                  do{ x ?                                                                                     \
                                                HAL_GPIO_WritePin(ATK_MW8266D_RST_GPIO_PORT, ATK_MW8266D_RST_GPIO_PIN, GPIO_PIN_SET) :  \
                                                HAL_GPIO_WritePin(ATK_MW8266D_RST_GPIO_PORT, ATK_MW8266D_RST_GPIO_PIN, GPIO_PIN_RESET); \
                                            }while(0)

/* 错误代码 */
#define ATK_MW8266D_EOK         0   /* 没有错误 */
#define ATK_MW8266D_ERROR       1   /* 通用错误 */
#define ATK_MW8266D_ETIMEOUT    2   /* 超时错误 */
#define ATK_MW8266D_EINVAL      3   /* 参数错误 */

/* 操作函数 */
void atk_mw8266d_hw_reset(void);                                            /* ATK-MW8266D硬件复位 */
uint8_t atk_mw8266d_send_at_cmd(char *cmd, char *ack, uint32_t timeout);    /* ATK-MW8266D发送AT指令 */
uint8_t atk_mw8266d_init(uint32_t baudrate);                                /* ATK-MW8266D初始化 */
uint8_t atk_mw8266d_restore(void);                                          /* ATK-MW8266D恢复出厂设置 */
uint8_t atk_mw8266d_at_test(void);                                          /* ATK-MW8266D AT指令测试 */
uint8_t atk_mw8266d_set_mode(uint8_t mode);                                 /* 设置ATK-MW8266D工作模式 */
uint8_t atk_mw8266d_sw_reset(void);                                         /* ATK-MW8266D软件复位 */
uint8_t atk_mw8266d_ate_config(uint8_t cfg);                                /* ATK-MW8266D设置回显模式 */
uint8_t atk_mw8266d_join_ap(char *ssid, char *pwd);                         /* ATK-MW8266D连接WIFI */
uint8_t atk_mw8266d_get_ip(char *buf);                                      /* ATK-MW8266D获取IP地址 */
uint8_t atk_mw8266d_connect_tcp_server(char *server_ip, char *server_port); /* ATK-MW8266D连接TCP服务器 */
uint8_t atk_mw8266d_enter_unvarnished(void);                                /* ATK-MW8266D进入透传 */
void atk_mw8266d_exit_unvarnished(void);                                    /* ATK-MW8266D退出透传 */
uint8_t atk_mw8266d_connect_atkcld(char *id, char *pwd);                    /* ATK-MW8266D连接原子云服务器 */
uint8_t atk_mw8266d_disconnect_atkcld(void);                                /* ATK-MW8266D断开原子云服务器连接 */
void select_transmode(int a);  // 选择透传模式

#endif
