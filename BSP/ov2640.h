#ifndef __OV2640_H__
#define __OV2640_H__

#include "sccb.h"
#include "dcmi.h"

/* Image Sizes enumeration */
typedef enum   
{
  BMP_QQVGA             =   0x00,	    /* BMP Image QQVGA 160x120 Size */
  BMP_QVGA              =   0x01,      /* BMP Image VGA 800*480 Size */
  JPEG_160x120          =   0x02,	    /* JPEG Image 160x120 Size */
  JPEG_176x144          =   0x03,	    /* JPEG Image 176x144 Size */
  JPEG_320x240          =   0x04,	    /* JPEG Image 320x240 Size */
  JPEG_352x288          =   0x05	    /* JPEG Image 352x288 Size */
}ImageFormat_TypeDef;


#define OV2640_PWDN  	    PGout(9)			//POWER DOWN控制信号
#define OV2640_RST  	    PGout(15)			//复位控制信号

#define OV2640_MID				0X7FA2        //厂家ID
#define OV2640_PID				0X2642        //设备ID

//当选择DSP地址(0XFF=0X00)时,OV2640的DSP寄存器地址映射表
#define OV2640_DSP_R_BYPASS     0x05
#define OV2640_DSP_Qs           0x44
#define OV2640_DSP_CTRL         0x50
#define OV2640_DSP_HSIZE1       0x51
#define OV2640_DSP_VSIZE1       0x52
#define OV2640_DSP_XOFFL        0x53
#define OV2640_DSP_YOFFL        0x54
#define OV2640_DSP_VHYX         0x55
#define OV2640_DSP_DPRP         0x56
#define OV2640_DSP_TEST         0x57
#define OV2640_DSP_ZMOW         0x5A
#define OV2640_DSP_ZMOH         0x5B
#define OV2640_DSP_ZMHH         0x5C
#define OV2640_DSP_BPADDR       0x7C
#define OV2640_DSP_BPDATA       0x7D
#define OV2640_DSP_CTRL2        0x86
#define OV2640_DSP_CTRL3        0x87
#define OV2640_DSP_SIZEL        0x8C
#define OV2640_DSP_HSIZE2       0xC0
#define OV2640_DSP_VSIZE2       0xC1
#define OV2640_DSP_CTRL0        0xC2
#define OV2640_DSP_CTRL1        0xC3
#define OV2640_DSP_R_DVP_SP     0xD3
#define OV2640_DSP_IMAGE_MODE   0xDA
#define OV2640_DSP_RESET        0xE0
#define OV2640_DSP_MS_SP        0xF0
#define OV2640_DSP_SS_ID        0x7F
#define OV2640_DSP_SS_CTRL      0xF8
#define OV2640_DSP_MC_BIST      0xF9
#define OV2640_DSP_MC_AL        0xFA
#define OV2640_DSP_MC_AH        0xFB
#define OV2640_DSP_MC_D         0xFC
#define OV2640_DSP_P_STATUS     0xFE
#define OV2640_DSP_RA_DLMT      0xFF 

//当选择传感器地址(0XFF=0X01)时,OV2640的DSP寄存器地址映射表
#define OV2640_SENSOR_GAIN       0x00
#define OV2640_SENSOR_COM1       0x03
#define OV2640_SENSOR_REG04      0x04
#define OV2640_SENSOR_REG08      0x08
#define OV2640_SENSOR_COM2       0x09
#define OV2640_SENSOR_PIDH       0x0A
#define OV2640_SENSOR_PIDL       0x0B
#define OV2640_SENSOR_COM3       0x0C
#define OV2640_SENSOR_COM4       0x0D
#define OV2640_SENSOR_AEC        0x10
#define OV2640_SENSOR_CLKRC      0x11
#define OV2640_SENSOR_COM7       0x12
#define OV2640_SENSOR_COM8       0x13
#define OV2640_SENSOR_COM9       0x14
#define OV2640_SENSOR_COM10      0x15
#define OV2640_SENSOR_HREFST     0x17
#define OV2640_SENSOR_HREFEND    0x18
#define OV2640_SENSOR_VSTART     0x19
#define OV2640_SENSOR_VEND       0x1A
#define OV2640_SENSOR_MIDH       0x1C
#define OV2640_SENSOR_MIDL       0x1D
#define OV2640_SENSOR_AEW        0x24
#define OV2640_SENSOR_AEB        0x25
#define OV2640_SENSOR_W          0x26
#define OV2640_SENSOR_REG2A      0x2A
#define OV2640_SENSOR_FRARL      0x2B
#define OV2640_SENSOR_ADDVSL     0x2D
#define OV2640_SENSOR_ADDVHS     0x2E
#define OV2640_SENSOR_YAVG       0x2F
#define OV2640_SENSOR_REG32      0x32
#define OV2640_SENSOR_ARCOM2     0x34
#define OV2640_SENSOR_REG45      0x45
#define OV2640_SENSOR_FLL        0x46
#define OV2640_SENSOR_FLH        0x47
#define OV2640_SENSOR_COM19      0x48
#define OV2640_SENSOR_ZOOMS      0x49
#define OV2640_SENSOR_COM22      0x4B
#define OV2640_SENSOR_COM25      0x4E
#define OV2640_SENSOR_BD50       0x4F
#define OV2640_SENSOR_BD60       0x50
#define OV2640_SENSOR_REG5D      0x5D
#define OV2640_SENSOR_REG5E      0x5E
#define OV2640_SENSOR_REG5F      0x5F
#define OV2640_SENSOR_REG60      0x60
#define OV2640_SENSOR_HISTO_LOW  0x61
#define OV2640_SENSOR_HISTO_HIGH 0x62


// 图像格式定义
#define IMAGE_FORMAT_RGB565  0
#define IMAGE_FORMAT_JPEG    1

// 分辨率定
#define QQVGA_WIDTH         160
#define QQVGA_HEIGHT        120
#define QVGA_WIDTH          160
#define QVGA_HEIGHT         120

extern uint8_t image_buffer[QVGA_WIDTH * QVGA_HEIGHT]; // QVGA RGB565图像缓冲区

uint8_t ov2640_init(void);
void OV2640_StartCapture(void);


void OV2640_Reset(void);
void OV2640_QQVGAConfig(void);
uint8_t OV2640_OutSize_Set(uint16_t width,uint16_t height);


void OV2640_UXGAConfig(void);
void OV2640_JPEGConfig(ImageFormat_TypeDef ImageFormat);

void OV2640_LightMode(uint8_t mode);
void OV2640_SpecialEffects(uint8_t mode);

void OV2640_WriteRegList(const uint8_t reglist[][2], uint16_t size);

// 停止捕获
void OV2640_StopCapture(void);
#endif


