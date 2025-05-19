#include "ov2640.h"
#include "ov2640cfg.h"

// 全局变量
uint8_t image_buffer[QVGA_WIDTH * QVGA_HEIGHT]; // QVGA RGB565图像缓冲区

// 写寄存器序列
static void OV2640_WriteRegList(const uint8_t reglist[][2], uint16_t size)
{
    for(uint16_t i = 0; i < size; i++)
    {
        SCCB_WR_Reg(reglist[i][0], reglist[i][1]);
        HAL_Delay(1);
    }
}

uint8_t ov2640_init(void)
{
		uint16_t reg,i;
	OV2640_PWDN = 0; //POWER ON
	HAL_Delay(10);
	OV2640_RST = 0; //复位OV2640
	HAL_Delay(10);
	OV2640_RST = 1;						   //结束复位
	
			SCCB_SDA_OUT();

	SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01); //操作sensor寄存器
	SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80); //软复位OV2640
	
		HAL_Delay(50);
	reg = SCCB_RD_Reg(OV2640_SENSOR_MIDH); //读取厂家ID 高八位
	reg <<= 8;
	reg |= SCCB_RD_Reg(OV2640_SENSOR_MIDL); //读取厂家ID 低八位
	
	if(reg !=OV2640_MID)   //读出来的厂家ID是错的
	{
			return 1;
	}
	
		reg = SCCB_RD_Reg(OV2640_SENSOR_PIDH); //读取厂家ID 高八位
	reg <<= 8;
	reg |= SCCB_RD_Reg(OV2640_SENSOR_PIDL); //读取厂家ID 低八位
	if (reg != OV2640_PID)
	{
		return 2;
	}
	
	return 0;
}


// 开始捕获
void OV2640_StartCapture(void)
{
    // 配置DCMI DMA传输
    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)image_buffer[0], QVGA_WIDTH * QVGA_HEIGHT);
}
// 停止捕获
void OV2640_StopCapture(void)
{
    HAL_DCMI_Stop(&hdcmi);
}

/**
  * @brief  Resets the OV2640 camera.
  * @param  None
  * @retval None
  */
void OV2640_Reset(void)
{
	/*OV2640有两组寄存器，设置0xFF寄存器的值为0或为1时可选择使用不同组的寄存器*/
  SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01);
  SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80);
}



/**
  * @brief  Configures the OV2640 camera in QQVGA mode.
  * @param  None
  * @retval None
  */
void OV2640_QQVGAConfig(void)
{
  uint32_t i;

  OV2640_Reset();
  HAL_Delay(200);

  /* Initialize OV2640 */
  for(i=0; i<(sizeof(OV2640_QQVGA)/2); i++)
  {
    SCCB_WR_Reg(OV2640_QQVGA[i][0], OV2640_QQVGA[i][1]);
    HAL_Delay(2);
  }
}

/*液晶屏的分辨率，用来计算地址偏移*/
uint16_t lcd_width=160, lcd_height=120;

/*摄像头采集图像的大小，改变这两个值可以改变数据量，
img_width和imgheight要求为4的倍数
但不会加快采集速度，要加快采集速度需要改成SVGA模式*/
uint16_t img_width=160, img_height=120;



/**
  * @brief  设置图像输出大小，OV2640输出图像的大小(分辨率),完全由该函数确定
  * @param  width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
  * @retval 0,设置成功，其他,设置失败
  */
uint8_t OV2640_OutSize_Set(uint16_t width,uint16_t height)
{
	uint16_t outh;
	uint16_t outw;
	uint8_t temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	outw=width/4;
	outh=height/4; 
	SCCB_WR_Reg(0XFF,0X00);	
	SCCB_WR_Reg(0XE0,0X04);	
  SCCB_WR_Reg(0X50,outw&0X00);		//配置v_divider hdivider
	SCCB_WR_Reg(0X5A,outw&0XFF);		//设置OUTW的低八位
	SCCB_WR_Reg(0X5B,outh&0XFF);		//设置OUTH的低八位
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	SCCB_WR_Reg(0X5C,temp);				//设置OUTH/OUTW的高位 
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
}

/**
  * @brief  配置OV2640为UXGA模式，并设置输出图像大小
  * @param  None
  * @retval None
  */
void OV2640_UXGAConfig(void)
{
  uint32_t i;
	
	/*摄像头复位*/
  OV2640_Reset();

	/*进行三次寄存器写入，确保配置写入正常
	(在使用摄像头长排线时，IIC数据线干扰较大，必须多次写入来保证正常)*/
  /* 写入寄存器配置 */
  for(i=0; i<(sizeof(OV2640_UXGA)/2); i++)
  {
    SCCB_WR_Reg(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);

  }
	  /* Initialize OV2640 */
  for(i=0; i<(sizeof(OV2640_UXGA)/2); i++)
  {
    SCCB_WR_Reg(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);

  }
	  /* Initialize OV2640 */
  for(i=0; i<(sizeof(OV2640_UXGA)/2); i++)
  {
    SCCB_WR_Reg(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);

  }


	/*设置输出的图像大小*/
	OV2640_OutSize_Set(img_width,img_height);   
}


/**
  * @brief  Configures the OV2640 camera in JPEG mode.
  * @param  JPEGImageSize: JPEG image size
  * @retval None
  */
void OV2640_JPEGConfig(ImageFormat_TypeDef ImageFormat)
{
  uint32_t i;

  OV2640_Reset();
  HAL_Delay(200);

  /* Initialize OV2640 */
  for(i=0; i<(sizeof(OV2640_JPEG_INIT)/2); i++)
  {
    SCCB_WR_Reg(OV2640_JPEG_INIT[i][0], OV2640_JPEG_INIT[i][1]);
  }

  /* Set to output YUV422 */
  for(i=0; i<(sizeof(OV2640_YUV422)/2); i++)
  {
    SCCB_WR_Reg(OV2640_YUV422[i][0], OV2640_YUV422[i][1]);
  }

  SCCB_WR_Reg(0xff, 0x01);
  SCCB_WR_Reg(0x15, 0x00);

  /* Set to output JPEG */
  for(i=0; i<(sizeof(OV2640_JPEG)/2); i++)
  {
    SCCB_WR_Reg(OV2640_JPEG[i][0], OV2640_JPEG[i][1]);
  }

  HAL_Delay(100);

  switch(ImageFormat)
  {
    case JPEG_160x120:
    {
      for(i=0; i<(sizeof(OV2640_160x120_JPEG)/2); i++)
      {
        SCCB_WR_Reg(OV2640_160x120_JPEG[i][0], OV2640_160x120_JPEG[i][1]);
      }
      break;
    }
    case JPEG_176x144:
    {
      for(i=0; i<(sizeof(OV2640_176x144_JPEG)/2); i++)
      {
        SCCB_WR_Reg(OV2640_176x144_JPEG[i][0], OV2640_176x144_JPEG[i][1]);
      } 
      break;
    }
    case JPEG_320x240:
    {
       for(i=0; i<(sizeof(OV2640_320x240_JPEG)/2); i++)
      {
        SCCB_WR_Reg(OV2640_320x240_JPEG[i][0], OV2640_320x240_JPEG[i][1]);
      }
      break;
    }
    case JPEG_352x288:
    {
      for(i=0; i<(sizeof(OV2640_352x288_JPEG)/2); i++)
      {
        SCCB_WR_Reg(OV2640_352x288_JPEG[i][0], OV2640_352x288_JPEG[i][1]);
      }
      break;
    }
    default:
    {
      for(i=0; i<(sizeof(OV2640_160x120_JPEG)/2); i++)
      {
        SCCB_WR_Reg(OV2640_160x120_JPEG[i][0], OV2640_160x120_JPEG[i][1]);
      }
      break;
    }
  }
}


/**
  * @brief  配置光线模式
  * @param  参数用于选择光线模式
  *         0x00 Auto     自动
  *         0x01 Sunny    光照
  *         0x02 Cloudy   阴天
  *         0x03 Office   办公室
  *         0x04 Home     家里

  * @retval None
  */
void OV2640_LightMode(uint8_t mode)
{
  switch(mode)
  {
    
    case 0:     //Auto
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0xc7, 0x00); //AWB on
    break;
    
    case 1:     //Sunny
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0xc7, 0x40); //AWB off
    SCCB_WR_Reg(0xcc, 0x5e);
    SCCB_WR_Reg(0xcd, 0x41);
    SCCB_WR_Reg(0xce, 0x54);
    
    break;
    
    case 2:    //Cloudy 
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0xc7, 0x40); //AWB off
    SCCB_WR_Reg(0xcc, 0x65);
    SCCB_WR_Reg(0xcd, 0x41);
    SCCB_WR_Reg(0xce, 0x4f);
    break;
    
    case 3:   //Office
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0xc7, 0x40); //AWB off
    SCCB_WR_Reg(0xcc, 0x52);
    SCCB_WR_Reg(0xcd, 0x41);
    SCCB_WR_Reg(0xce, 0x66);
    break;
    
    case 4:   //Home
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0xc7, 0x40); //AWB off
    SCCB_WR_Reg(0xcc, 0x42);
    SCCB_WR_Reg(0xcd, 0x3f);
    SCCB_WR_Reg(0xce, 0x71);
    break;

  }
}

/**
  * @brief  特殊效果
  * @param  参数用于选择光线模式
  *         0x00 Antique   复古  
  *         0x01 Bluish    偏蓝
  *         0x02 Greenish  偏绿 
  *         0x03 Reddish   偏红
  *         0x04 B&W       黑白
  *         0x05 Negative  反相
  *         0x06 B&W negative  黑白反相
  *         0x07 Normal     正常 

  * @retval None
  */
void OV2640_SpecialEffects(uint8_t mode)
{
  switch(mode)
  {
    case 0:
   // Antique
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x18);
    SCCB_WR_Reg(0x7c, 0x05);
    SCCB_WR_Reg(0x7d, 0x40);
    SCCB_WR_Reg(0x7d, 0xa6);
    break;
    
    case 1:    
    //Bluish
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x18);
    SCCB_WR_Reg(0x7c,0x05);
    SCCB_WR_Reg(0x7d, 0xa0);
    SCCB_WR_Reg(0x7d, 0x40);
    
    break;
    
    case 2:
    //Greenish
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x18);
    SCCB_WR_Reg(0x7c, 0x05);
    SCCB_WR_Reg(0x7d, 0x40);
    SCCB_WR_Reg(0x7d, 0x40);
    break;
    
    case 3:
   // Reddish
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x18);
    SCCB_WR_Reg(0x7c, 0x05);
    SCCB_WR_Reg(0x7d, 0x40);
    SCCB_WR_Reg(0x7d, 0xc0);
    break;
    
    case 4:
   // B&W
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x18);
    SCCB_WR_Reg(0x7c, 0x05);
    SCCB_WR_Reg(0x7d, 0x80);
    SCCB_WR_Reg(0x7d, 0x80);
    break;
    
    case 5:
    //Negative
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x40);
    SCCB_WR_Reg(0x7c, 0x05);
    SCCB_WR_Reg(0x7d, 0x80);
    SCCB_WR_Reg(0x7d, 0x80);
    
    break;
    
    case 6:
    //B&W negative
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x58);
    SCCB_WR_Reg(0x7c, 0x05);
    SCCB_WR_Reg(0x7d, 0x80);
    SCCB_WR_Reg(0x7d, 0x80);
    
    break;
    
    case 7:
    //Normal
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x00);
    SCCB_WR_Reg(0x7c, 0x05);
    SCCB_WR_Reg(0x7d, 0x80);
    SCCB_WR_Reg(0x7d, 0x80);
    
    break;

  }
}