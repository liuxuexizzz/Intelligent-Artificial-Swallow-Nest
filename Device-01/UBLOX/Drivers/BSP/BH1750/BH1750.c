#include "./BSP/BH1750/BH1750.h"
#include "./SYSTEM/delay/delay.h"
unsigned char    BUF[8]; 
int mcy;     //½øÎ»±êÖ¾
/**
 * @brief       IIC½Ó¿ÚÑÓÊ±º¯Êý£¬ÓÃÓÚ¿ØÖÆIIC¶ÁÐ´ËÙ¶È
 * @param       ÎÞ
 * @retval      ÎÞ
 */
static inline void BH1750_iic_delay(void)
{
    delay_us(2);
}

/**
 * @brief       ²úÉúIICÆðÊ¼ÐÅºÅ
 * @param       ÎÞ
 * @retval      Î Þ
 */
void BH1750_iic_start(void)
{
    BH1750_IIC_SDA(1);
    BH1750_IIC_SCL(1);
    BH1750_iic_delay();
    BH1750_IIC_SDA(0);
    BH1750_iic_delay();
    BH1750_IIC_SCL(0);
    BH1750_iic_delay();
}

/**
 * @brief       ²úÉúIICÍ£Ö¹ÐÅºÅ
 * @param       ÎÞ
 * @retval      ÎÞ
 */
void BH1750_iic_stop(void)
{
    BH1750_IIC_SDA(0);
    BH1750_iic_delay();
    BH1750_IIC_SCL(1);
    BH1750_iic_delay();
    BH1750_IIC_SDA(1);
    BH1750_iic_delay();
}

void BH1750_SendACK(int ack)
{
	GPIO_InitTypeDef gpio_init_struct;
	
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;          /* ÍÆÍìÊä³ö */  
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;         /* ¸ßËÙ */
    gpio_init_struct.Pin    = BH1750_IIC_SDA_GPIO_PIN;  /* SDAÒý½Å */
    HAL_GPIO_Init(BH1750_IIC_SDA_GPIO_PORT, &gpio_init_struct);
	
	if(ack == 1)   //Ð´Ó¦´ðÐÅºÅ
		    BH1750_IIC_SDA(1);
	else if(ack == 0)
		    BH1750_IIC_SDA(0);
	else
		return;			
   BH1750_IIC_SCL(1);     //À­¸ßÊ±ÖÓÏß
  delay_us(5);                 //ÑÓÊ±
  BH1750_IIC_SCL(0);    //À­µÍÊ±ÖÓÏß
  delay_us(5);                //ÑÓÊ±
}

/**************************************
½ÓÊÕÓ¦´ðÐÅºÅ
**************************************/
int BH1750_RecvACK()
{
	GPIO_InitTypeDef gpio_init_struct;
	
	gpio_init_struct.Mode = GPIO_MODE_INPUT ;          /* ÍÆÍìÊä³ö */  
	gpio_init_struct.Pull = GPIO_PULLUP;     // Ê¹ÓÃÄÚ²¿ÉÏÀ­
	gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;         /* ¸ßËÙ */
   gpio_init_struct.Pin    = BH1750_IIC_SDA_GPIO_PIN;  /* SDAÒý½Å */
	HAL_GPIO_Init(BH1750_IIC_SDA_GPIO_PORT, &gpio_init_struct);
	
  BH1750_IIC_SCL(1);            //À­¸ßÊ±ÖÓÏß
  delay_us(5);                 //ÑÓÊ±	
 if(HAL_GPIO_ReadPin(BH1750_IIC_SDA_GPIO_PORT, BH1750_IIC_SDA_GPIO_PIN) == GPIO_PIN_SET)
    {
        mcy = 1; // Èç¹ûSDAÏßÎª¸ß£¬±íÊ¾Î´ÊÕµ½ACKÐÅºÅ
    }
    else
    {
        mcy = 0; // Èç¹ûSDAÏßÎªµÍ£¬±íÊ¾ÊÕµ½ACKÐÅºÅ
    }	
  BH1750_IIC_SCL(0);                    //À­µÍÊ±ÖÓÏß
  delay_us(5);                 //ÑÓÊ±
	gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP; 
  HAL_GPIO_Init(BH1750_IIC_SDA_GPIO_PORT, &gpio_init_struct);
	return mcy;
}

/**
 * @brief       IIC·¢ËÍÒ»¸ö×Ö½Ú
 * @param       dat: Òª·¢ËÍµÄÊý¾Ý
 * @retval      ÎÞ
 */
void BH1750_SendByte(uint8_t dat)
{
  uint8_t i;
  for (i = 0; i < 8; i++) // 8-bit counter
  {
    if (0x80 & dat)
      HAL_GPIO_WritePin(BH1750_IIC_SDA_GPIO_PORT, BH1750_IIC_SDA_GPIO_PIN, GPIO_PIN_SET);
    else
			HAL_GPIO_WritePin(BH1750_IIC_SDA_GPIO_PORT, BH1750_IIC_SDA_GPIO_PIN,GPIO_PIN_RESET);
    dat <<= 1;
    BH1750_IIC_SCL(1); // Pull clock line high
    delay_us(5);                                              // Delay
    BH1750_IIC_SCL(0); // Pull clock line low
    delay_us(5);                                             // Delay
  }
  BH1750_RecvACK(); // Receive ACK
}

uint8_t BH1750_RecvByte()
{
  uint8_t i;
  uint8_t dat = 0;
  uint8_t bit;
	
		GPIO_InitTypeDef gpio_init_struct;
	
		gpio_init_struct.Mode = GPIO_MODE_INPUT ;          /* ÍÆÍìÊä³ö */  
		gpio_init_struct.Pull = GPIO_PULLUP;     // Ê¹ÓÃÄÚ²¿ÉÏÀ­
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;         /* ¸ßËÙ */
    gpio_init_struct.Pin    = BH1750_IIC_SDA_GPIO_PIN;  /* SDAÒý½Å */
    HAL_GPIO_Init(BH1750_IIC_SDA_GPIO_PORT, &gpio_init_struct);
	
  HAL_GPIO_WritePin(BH1750_IIC_SDA_GPIO_PORT, BH1750_IIC_SDA_GPIO_PIN, GPIO_PIN_SET); // Enable internal pull-up, prepare for data reading
  for (i = 0; i < 8; i++) // 8-bit counter
  {
    dat <<= 1;
    BH1750_IIC_SCL(1); // Pull clock line high
    delay_us(5);                                              // Delay
			
		if (GPIO_PIN_SET == HAL_GPIO_ReadPin(BH1750_IIC_SDA_GPIO_PORT, BH1750_IIC_SDA_GPIO_PIN)) // Read data
      bit = 0x01;
    else
      bit = 0x00;  
    dat |= bit;
    BH1750_IIC_SCL(0);// Pull clock line low
    delay_us(5);                                            // Delay
  }
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(BH1750_IIC_SDA_GPIO_PORT, &gpio_init_struct);
  return dat;
}

void Single_Write_BH1750(unsigned char REG_Address)//REG_AddressÊÇÒªÐ´ÈëµÄÖ¸Áî
{
  BH1750_iic_start();                  //ÆðÊ¼ÐÅºÅ
  BH1750_SendByte(SlaveAddress);   //·¢ËÍÉè±¸µØÖ·+Ð´ÐÅºÅ
  BH1750_SendByte(REG_Address);    //Ð´ÈëÖ¸Áî
  BH1750_iic_stop();                   //·¢ËÍÍ£Ö¹ÐÅºÅ
}

/**
 * @brief       ³õÊ¼»¯IIC½Ó¿Ú
 * @param       ÎÞ
 * @retval      ÎÞ
 */
void Init_BH1750()
{
	BH1750_iic_start;
	GPIO_InitTypeDef gpio_init_struct;
	
  /* Enable GPIO clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_struct.Pull = GPIO_PULLUP;     // Ê¹ÓÃÄÚ²¿ÉÏÀ­
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init_struct.Pin = BH1750_IIC_SDA_GPIO_PIN | BH1750_IIC_SCL_GPIO_PIN;
  HAL_GPIO_Init(BH1750_IIC_SDA_GPIO_PORT, &gpio_init_struct);
	BH1750_SendByte(SlaveAddress);   //·¢ËÍÉè±¸µØÖ·+Ð´ÐÅºÅ 
  Single_Write_BH1750(0x01);
	BH1750_iic_stop;
  HAL_Delay(180); // Delay 180ms
}

void mread(void)
{   
  unsigned char i;	
  BH1750_iic_start();                          //ÆðÊ¼ÐÅºÅ
  BH1750_SendByte(SlaveAddress+1);         //·¢ËÍÉè±¸µØÖ·+¶ÁÐÅºÅ

  for (i=0; i<3; i++)                      //Á¬Ðø¶ÁÈ¡2¸öÊý¾Ý£¬´æ´¢µ½BUFÀïÃæ
  {
    BUF[i] = BH1750_RecvByte(0);          //BUF[0]´æ´¢¸ß8Î»£¬BUF[1]´æ´¢µÍ8Î»
    if (i == 2)
    {
      BH1750_SendACK(1);                //×îºóÒ»¸öÊý¾ÝÐèÒª»ØNOACK
    }
    else
    {		
      BH1750_SendACK(0);                //»ØÓ¦ACK
    }
  }
  BH1750_iic_stop();                          //Í£Ö¹ÐÅºÅ
  delay_ms(5);
}

float read_BH1750(void) 
{
    int dis_data; //·Ö±æÂÊ	
    float temp1;

    // ·¢ËÍÉÏµçÃüÁî(0x01)
    Single_Write_BH1750(0x01);
    // ·¢ËÍ¸ß·Ö±æÂÊÁ¬Ðø²âÁ¿ÃüÁî(0x10)
    Single_Write_BH1750(0x10);
    delay_ms(180);

    // Á¬Ðø¶Á³öÊý¾Ý£¬´æ´¢ÔÚBUFÖÐ
    mread();
    // ºÏ²¢Á½¸ö×Ö½ÚµÄÊý¾Ý
    dis_data = BUF[0];
    dis_data = (dis_data << 8) + BUF[1];

    // ¼ÆËã¹âÕÕ¶È
    temp1 = dis_data/1.2;

    return temp1;
}

