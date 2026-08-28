/**
 ****************************************************************************************************
 * @file        demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MO1218模块测试实验
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
 ****************************************************************************************************
 */

#include "demo.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/DHT11/dht11.h"
#include "./BSP/BH1750/BH1750.h"
#include "./BSP/ESP8266/atk_mw8266d.h"

// 根据自己情况设置，WiFi名称与密码
#define DEMO_WIFI_SSID          "XinXun-2904"
#define DEMO_WIFI_PWD           "12345678"
#define DEMO_TCP_SERVER_IP      "94.74.103.177"
#define DEMO_TCP_SERVER_PORT    "8080"

//#define DEMO_WIFI_SSID          "T-Lab"
//#define DEMO_WIFI_PWD           "77714404"



/**
 * @brief       显示IP地址
 * @param       无
 * @retval      无
 */
static void demo_show_ip(char *buf)
{
    printf("IP: %s\r\n", buf);
}

/**
 * @brief       按键0功能，功能测试
 * @param       is_unvarnished: 0，未进入透传
 *                              1，已进入透传
 * @retval      无
 */
static void demo_key0_fun(uint8_t is_unvarnished)
{
    uint8_t ret;
    
    if (is_unvarnished == 0)
    {
        /* 进行AT指令测试 */
        ret = atk_mw8266d_at_test();
        if (ret == 0)
        {
            printf("AT test success!\r\n");
        }
        else
        {
            printf("AT test failed!\r\n");
        }
    }
    else
    {
        /* 通过透传，发送信息至TCP Server */
        atk_mw8266d_uart_printf("This ATK-MW8266D TCP Connect Test.\r\n");
    }
}

/**
 * @brief       按键1功能，切换透传模式
 * @param       is_unvarnished: 0，未进入透传
 *                              1，已进入透传
 * @retval      无
 */
static void demo_key1_fun(uint8_t *is_unvarnished)
{
    uint8_t ret;
    
    if (*is_unvarnished == 0)
    {
        /* 进入透传 */
        ret = atk_mw8266d_enter_unvarnished();
        if (ret == 0)
        {
            *is_unvarnished = 1;
            printf("Enter unvarnished!\r\n");
        }
        else
        {
            printf("Enter unvarnished failed!\r\n");
        }
    }
    else
    {
        /* 退出透传 */
        atk_mw8266d_exit_unvarnished();
        *is_unvarnished = 0;
        printf("Exit unvarnished!\r\n");
    }
}

/**
 * @brief       进入透传时，将接收自TCP Server的数据发送到串口调试助手
 * @param       is_unvarnished: 0，未进入透传
 *                              1，已进入透传
 * @retval      无
 */
static void demo_upload_data(uint8_t is_unvarnished)
{
    uint8_t *buf;
    
    if (is_unvarnished == 1)
    {
        /* 接收来自ATK-MW8266D UART的一帧数据 */
        buf = atk_mw8266d_uart_rx_get_frame();
        if (buf != NULL)
        {
            printf("%s", buf);
            /* 重开开始接收来自ATK-MW8266D UART的数据 */
            atk_mw8266d_uart_rx_restart();
        }
    }
}
/**
 * @brief       TCP透传模式设置，a=1透传功能测试，a=2进入透传
 * @param       is_unvarnished: 0，未进入透传
 *                              1，已进入透传
 * @retval      无
 */
void select_transmode(int a)
{
			uint8_t is_unvarnished = 0;
      if (a == 1)
      {

          /* 功能测试 */
          demo_key0_fun(is_unvarnished);

      }
      else if (a == 2)
      {
          /* 透传模式切换 */
          demo_key1_fun(&is_unvarnished);

      }
}

/**
 * @brief       例程演示入口函数
 * @param       无
 * @retval      无
 */
void demo_run(void)
{
    uint8_t ret;
    float light;
		
		// 初始化，GPS数据结构体
		clrStruct();

    /* 初始化dht11*/
    dht11_init();
		
		char ip_buf[16];
    //uint8_t key;
    uint8_t is_unvarnished = 0;
    /* 初始化ATK-MW8266D */
    ret = atk_mw8266d_init(115200);
    if (ret != 0)
    {
        printf("ATK-MW8266D init failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    printf("Joining to AP...\r\n");
    ret  = atk_mw8266d_restore();                               /* 恢复出厂设置 */
    ret += atk_mw8266d_at_test();                               /* AT测试 */
    ret += atk_mw8266d_set_mode(1);                             /* Station模式 */
    ret += atk_mw8266d_sw_reset();                              /* 软件复位 */
    ret += atk_mw8266d_ate_config(0);                           /* 关闭回显功能 */
    ret += atk_mw8266d_join_ap(DEMO_WIFI_SSID, DEMO_WIFI_PWD);  /* 连接WIFI */
    ret += atk_mw8266d_get_ip(ip_buf);                          /* 获取IP地址 */
    if (ret != 0)
    {
        printf("Error to join ap!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
		
    demo_show_ip(ip_buf);

    /* 连接TCP服务器 */
    ret = atk_mw8266d_connect_tcp_server(DEMO_TCP_SERVER_IP, DEMO_TCP_SERVER_PORT);
    if (ret != 0)
    {
        printf("Error to connect tcp server!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    
		
		select_transmode(2);//选择进入TCP透传模式
		
		
    /* 重新开始接收新的一帧数据 */
    atk_mw8266d_uart_rx_restart();
	
    while (1)
    {
        /* 获取并更新UBLOX模块数据 */
				parseGpsBuffer();
//        printGpsBuffer();

        // dht11
        uint8_t t = 0;
        uint8_t temperature;
        uint8_t humidity;
        /* 每100ms读取一次温湿度数据	*/
        dht11_read_data(&temperature, &humidity);   /* 读取温湿度 */
        printf("temperature: %d\r\n", temperature); /* 打印温度 */
        printf("humidity: %d\r\n", humidity);       /* 打印湿度 */
        delay_ms(1000);

        // BH1750
        Init_BH1750();
        light = read_BH1750();
        printf("light is ：%f lx ", light);
        delay_ms(1000);
				
				//ESP8266	
        /* 发送透传接收自TCP Server的数据到串口调试助手 */
        demo_upload_data(is_unvarnished);
				/* 透传数据到TCP服务器 */
//				atk_mw8266d_uart_printf("Test ok!\r\n");
				// GPS定位上传
				uploadGpsBuffer();
				// 温湿度上传
				atk_mw8266d_uart_printf("temperature: %d\r\n", temperature);
				atk_mw8266d_uart_printf("humidity: %d\r\n", humidity);
				// 光照上传
				atk_mw8266d_uart_printf("light：%f lx ", light);
        delay_ms(1000);
    }
}
