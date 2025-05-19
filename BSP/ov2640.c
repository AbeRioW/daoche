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
    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)image_buffer, QVGA_WIDTH * QVGA_HEIGHT);
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
	
	  for(i=0; i<(sizeof(OV2640_QQVGA)/2); i++)
  {
    SCCB_WR_Reg(OV2640_QQVGA[i][0], OV2640_QQVGA[i][1]);
    HAL_Delay(2);
  }
	
	  for(i=0; i<(sizeof(OV2640_QQVGA)/2); i++)
  {
    SCCB_WR_Reg(OV2640_QQVGA[i][0], OV2640_QQVGA[i][1]);
    HAL_Delay(2);
  }
}

/*液晶屏的分辨率，用来计算地址偏移*/
uint16_t lcd_width=320, lcd_height=240;

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


/**未测试*/
/**
  * @brief  设置图像尺寸大小,也就是所选格式的输出分辨率
  * @param  width,height:图像宽度和图像高度
  * @retval 0,设置成功，其他,设置失败
  */
uint8_t OV2640_ImageSize_Set(uint16_t width,uint16_t height)
{
	uint8_t temp;
	SCCB_WR_Reg(0XFF,0X00);
	SCCB_WR_Reg(0XE0,0X04);
	SCCB_WR_Reg(0XC0,(width)>>3&0XFF);		//设置HSIZE的10:3位
	SCCB_WR_Reg(0XC1,(height)>>3&0XFF);		//设置VSIZE的10:3位
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80;
	SCCB_WR_Reg(0X8C,temp);
	SCCB_WR_Reg(0XE0,0X00);
	return 0;
}

/**未测试*/
/**
  * @brief  设置图像输出窗口
  * @param  sx,sy,起始地址
						width,height:宽度(对应:horizontal)和高度(对应:vertical)
  * @retval 0,设置成功， 其他,设置失败
  */
void OV2640_Window_Set(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint16_t endx;
	uint16_t endy;
	uint8_t temp;
	endx=sx+width/2;	//V*2
 	endy=sy+height/2;

 	SCCB_WR_Reg(0XFF,0X01);
	temp = SCCB_RD_Reg(0X03);				//读取Vref之前的值
	temp&=0XF0;
	temp|=((endy&0X03)<<2)|(sy&0X03);
	SCCB_WR_Reg(0X03,temp);				//设置Vref的start和end的最低2位
	SCCB_WR_Reg(0X19,sy>>2);			//设置Vref的start高8位
	SCCB_WR_Reg(0X1A,endy>>2);			//设置Vref的end的高8位

	temp = SCCB_RD_Reg(0X32);				//读取Href之前的值
	temp&=0XC0;
	temp|=((endx&0X07)<<3)|(sx&0X07);
	SCCB_WR_Reg(0X32,temp);				//设置Href的start和end的最低3位
	SCCB_WR_Reg(0X17,sx>>3);			//设置Href的start高8位
	SCCB_WR_Reg(0X18,endx>>3);			//设置Href的end的高8位
}

//未测试
/**
  * @brief  设置图像开窗大小
						由:OV2640_ImageSize_Set确定传感器输出分辨率从大小.
						该函数则在这个范围上面进行开窗,用于OV2640_OutSize_Set的输出
						注意:本函数的宽度和高度,必须大于等于OV2640_OutSize_Set函数的宽度和高度
						     OV2640_OutSize_Set设置的宽度和高度,根据本函数设置的宽度和高度,由DSP
						     自动计算缩放比例,输出给外部设备.
  * @param  width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
  * @retval 0,设置成功， 其他,设置失败
  */
uint8_t OV2640_ImageWin_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)
{
	uint16_t hsize;
	uint16_t vsize;
	uint8_t temp;
	if(width%4)return 1;
	if(height%4)return 2;
	hsize=width/4;
	vsize=height/4;
	SCCB_WR_Reg(0XFF,0X00);
	SCCB_WR_Reg(0XE0,0X04);
	SCCB_WR_Reg(0X51,hsize&0XFF);		//设置H_SIZE的低八位
	SCCB_WR_Reg(0X52,vsize&0XFF);		//设置V_SIZE的低八位
	SCCB_WR_Reg(0X53,offx&0XFF);		//设置offx的低八位
	SCCB_WR_Reg(0X54,offy&0XFF);		//设置offy的低八位
	temp=(vsize>>1)&0X80;
	temp|=(offy>>4)&0X70;
	temp|=(hsize>>5)&0X08;
	temp|=(offx>>8)&0X07;
	SCCB_WR_Reg(0X55,temp);				//设置H_SIZE/V_SIZE/OFFX,OFFY的高位
	SCCB_WR_Reg(0X57,(hsize>>2)&0X80);	//设置H_SIZE/V_SIZE/OFFX,OFFY的高位
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
#if 1
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
#else
  for(i=0; i<(sizeof(OV2640_QQVGA)/2); i++)
  {
    SCCB_WR_Reg(OV2640_QQVGA[i][0], OV2640_QQVGA[i][1]);

  }
	  /* Initialize OV2640 */
  for(i=0; i<(sizeof(OV2640_QQVGA)/2); i++)
  {
    SCCB_WR_Reg(OV2640_QQVGA[i][0], OV2640_QQVGA[i][1]);

  }
	  /* Initialize OV2640 */
  for(i=0; i<(sizeof(OV2640_QQVGA)/2); i++)
  {
    SCCB_WR_Reg(OV2640_QQVGA[i][0], OV2640_QQVGA[i][1]);

  }

#endif


 // 注释掉的这三个函数未测试
//  OV2640_ImageSize_Set(img_width,img_height);
//  OV2640_ImageWin_Set(0,0,img_width,img_height);
//  OV2640_Window_Set(0,0,img_width,img_height);
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