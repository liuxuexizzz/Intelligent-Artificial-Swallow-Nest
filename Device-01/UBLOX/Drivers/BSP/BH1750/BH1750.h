#ifndef __BH1750_H
#define __BH1750_H 

#include "./SYSTEM/sys/sys.h"

// GPIO端口和引脚配置
#define BH1750_IIC_SCL_GPIO_PORT            GPIOB
#define BH1750_IIC_SCL_GPIO_PIN             GPIO_PIN_6
#define BH1750_IIC_SCL_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define BH1750_IIC_SDA_GPIO_PORT            GPIOB
#define BH1750_IIC_SDA_GPIO_PIN             GPIO_PIN_7
#define BH1750_IIC_SDA_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

// IO操作宏定义
#define BH1750_IIC_SCL(x)                   do{ x ?                                                                                             \
                                                    HAL_GPIO_WritePin(BH1750_IIC_SCL_GPIO_PORT, BH1750_IIC_SCL_GPIO_PIN, GPIO_PIN_SET) :    \
                                                    HAL_GPIO_WritePin(BH1750_IIC_SCL_GPIO_PORT, BH1750_IIC_SCL_GPIO_PIN, GPIO_PIN_RESET);   \
                                                }while(0)

#define BH1750_IIC_SDA(x)                   do{ x ?                                                                                             \
                                                    HAL_GPIO_WritePin(BH1750_IIC_SDA_GPIO_PORT, BH1750_IIC_SDA_GPIO_PIN, GPIO_PIN_SET) :    \
                                                    HAL_GPIO_WritePin(BH1750_IIC_SDA_GPIO_PORT, BH1750_IIC_SDA_GPIO_PIN, GPIO_PIN_RESET);   \
                                                }while(0)

#define BH1750_IIC_READ_SDA()               HAL_GPIO_ReadPin(BH1750_IIC_SDA_GPIO_PORT, BH1750_IIC_SDA_GPIO_PIN)
                                                                                                
#define SlaveAddress                        0x46 // BH1750的I2C地址

/* 函数声明 */   				
extern unsigned char    BUF[8]; 																								
extern int     dis_data;                       //变量		
extern int   mcy;              //表示进位标志位

void Init_BH1750(void);
void  Single_Write_BH1750(unsigned char REG_Address);//单个写入数据
unsigned char Single_Read_BH1750(unsigned char REG_Address);   //单个读取内部寄存器数据
void  mread(void);         //连续的读取内部寄存器数据
float read_BH1750(void);

#endif
