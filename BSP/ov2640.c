#include "ov2640.h"
#include "ov2640cfg.h"


#if 0
/*	const u8 *EFFECTS_TBL[7] = {
	"Normal", "Negative", "B&W", "Redish", "Greenish", "Bluish", "Antique"}; //7种特效
const u8 *JPEG_SIZE_TBL[9] = {
	"QCIF", "QQVGA", "CIF", "QVGA", "VGA", "SVGA", "XGA", "SXGA", "UXGA"}; //JPEG图片 9种尺寸
*/
// JPEG 尺寸支持列表
//const uint16_t jpeg_img_size_tbl[][2] = {
//	176, 144,	//QCIF
//	160, 120,	//QQVGA
//	352, 288,	//CIF
//	320, 240,	//QVGA
//	640, 480,	//VGA
//	800, 600,	//SVGA
//	1024, 768,	//XGA
//	1280, 1024, //SXGA
//	1600, 1200, //UXGA
//};

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
	
	//初始化 OV2640,采用SXGA分辨率(1600*1200)
	for (i = 0; i < sizeof(ov2640_sxga_init_reg_tbl) / 2; i++)
	{
		SCCB_WR_Reg(ov2640_sxga_init_reg_tbl[i][0], ov2640_sxga_init_reg_tbl[i][1]);
	}
	
	OV2640_JPEG_Mode();													  //JPEG模式
	
	OV2640_OutSize_Set(jpeg_img_size_tbl[3][0], jpeg_img_size_tbl[3][1]); //设置输出尺寸——默认是QVGA 320*240尺寸
	return 0;
}

////OV2640切换为JPEG模式
//void OV2640_JPEG_Mode(void)
//{
//	uint16_t i = 0;
//	//设置:YUV422格式
//	for (i = 0; i < (sizeof(ov2640_yuv422_reg_tbl) / 2); i++)
//	{
//		SCCB_WR_Reg(ov2640_yuv422_reg_tbl[i][0], ov2640_yuv422_reg_tbl[i][1]);
//	}

//	//设置:输出JPEG数据
//	for (i = 0; i < (sizeof(ov2640_jpeg_reg_tbl) / 2); i++)
//	{
//		SCCB_WR_Reg(ov2640_jpeg_reg_tbl[i][0], ov2640_jpeg_reg_tbl[i][1]);
//	}
//}

//设置图像输出大小
//OV2640输出图像的大小(分辨率),完全由该函数确定
//width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
//返回值:0,设置成功
//    其他,设置失败
//uint8_t OV2640_OutSize_Set(uint16_t width, uint16_t height)
//{
//	uint16_t outh;
//	uint16_t outw;
//	uint8_t temp;
//	if (width % 4)
//		return 1;
//	if (height % 4)
//		return 2;
//	outw = width / 4;
//	outh = height / 4;
//	SCCB_WR_Reg(0XFF, 0X00);
//	SCCB_WR_Reg(0XE0, 0X04);
//	SCCB_WR_Reg(0X5A, outw & 0XFF); //设置OUTW的低八位
//	SCCB_WR_Reg(0X5B, outh & 0XFF); //设置OUTH的低八位
//	temp = (outw >> 8) & 0X03;
//	temp |= (outh >> 6) & 0X04;
//	SCCB_WR_Reg(0X5C, temp); //设置OUTH/OUTW的高位
//	SCCB_WR_Reg(0XE0, 0X00);
//	return 0;
//}

#else

// 全局变量
uint8_t image_buffer[QVGA_WIDTH * QVGA_HEIGHT*2]; // QVGA RGB565图像缓冲区

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
	HAL_Delay(20);
	OV2640_RST = 0; //复位OV2640
	HAL_Delay(20);
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

	    // 初始化寄存器序列
    static const uint8_t ov2640_init_reg_tbl[][2] = {
    // 切换到 Bank 0
    {0xFF, 0x00},
    
    // 基本配置
    {0x12, 0x14},  // 输出格式: RGB565 (0x14)
    {0x3C, 0x30},  // 缩放控制: 启用缩放
    
    // 水平和垂直尺寸设置 (原始图像区域)
    {0x17, 0x11},  // HSTART[7:0] = 17 (水平起始位置)
    {0x18, 0x3F},  // HSTOP[7:0] = 63 (水平结束位置)
    {0x19, 0x01},  // VSTART[7:0] = 1 (垂直起始位置)
    {0x1A, 0x3F},  // VSTOP[7:0] = 63 (垂直结束位置)
    
    // 输出窗口大小 (160×120)
    {0x32, 0x28},  // HREF[9:2] = 40 (水平参考)
    {0x1E, 0x07},  // HOutSize[7:0] = 7 (水平输出尺寸低 8 位)
    {0x03, 0x05},  // HOutSize[11:8] = 5 (水平输出尺寸高 4 位)
                    // 总 HOutSize = 5*256 + 7 = 1287 (对应 160 像素)
    
    {0x1F, 0x04},  // VOutSize[7:0] = 4 (垂直输出尺寸低 8 位)
    {0x04, 0x09},  // VOutSize[11:8] = 9 (垂直输出尺寸高 4 位)
                    // 总 VOutSize = 9*256 + 4 = 2308 (对应 120 像素)
    
    // RGB565 格式配置
    {0x33, 0x02},  // 像素顺序: RGB
    {0x3A, 0x04},  // RGB 输出控制: 565 格式
    
    // 同步信号配置
    {0x07, 0xC0},  // 同步信号极性: HSYNC/VSYNC 低电平有效
    
    // 亮度/对比度/饱和度
    {0x50, 0x80},  // 亮度: 中间值
    {0x51, 0x80},  // 对比度: 中间值
    {0x52, 0x20},  // 饱和度: 默认值
    {0x53, 0x00},  // 色调: 默认值
    
    // 帧率控制 (可选)
    {0x11, 0x00},  // PLL 配置: 使用内部 24MHz 晶振
    {0x0C, 0x00},  // 帧率控制: 无限制 (根据时钟)
    
    // 完成配置
    {0xFF, 0x00},  // 返回 Bank 0
    };
    OV2640_WriteRegList(ov2640_init_reg_tbl, sizeof(ov2640_init_reg_tbl)/2);

		
		    // 默认设置为RGB565模式
   // OV2640_SetImageFormat(IMAGE_FORMAT_RGB565);
	return 0;
}


// 设置分辨率
void OV2640_SetResolution(uint16_t width, uint16_t height)
{
    SCCB_WR_Reg(0xFF, 0x00);
    SCCB_WR_Reg(0xE0, 0x04);
    SCCB_WR_Reg(0xC0, (width >> 3) & 0xFF);   // HSIZE[10:3]
    SCCB_WR_Reg(0xC1, (height >> 3) & 0xFF);  // VSIZE[10:3]
    
    uint8_t temp = (width & 0x07) << 3;
    temp |= height & 0x07;
    temp |= (width >> 7) & 0x80;
    SCCB_WR_Reg(0x8C, temp);
    
    SCCB_WR_Reg(0xE0, 0x00);
}

// 设置图像格式
void OV2640_SetImageFormat(uint8_t format)
{
    if(format == IMAGE_FORMAT_JPEG)
    {
        static const uint8_t ov2640_jpeg_reg_tbl[][2] = {
            {0xFF, 0x01},
            {0x15, 0x00},
            {0xFF, 0x00},
            {0x44, 0x32},
            {0x45, 0x20},
            {0x46, 0x4F},
            {0x47, 0x50},
            {0x48, 0x51},
            {0x49, 0x26},
            {0x4A, 0x18},
            {0x4B, 0x0C},
            {0x4C, 0x0A},
            {0x4D, 0x0A},
            {0x4E, 0x09},
            {0x4F, 0x0A},
            {0x50, 0x0B},
            {0x51, 0x0F},
            {0x52, 0x0E},
            {0x53, 0x0D},
            {0x54, 0x11},
            {0x55, 0x0E},
            {0x56, 0x0A},
            {0x57, 0x09},
            {0x58, 0x0D},
            {0x59, 0x15},
            {0x5A, 0x13},
            {0x5B, 0x0F},
            {0x5C, 0x0F},
            {0x5D, 0x13},
            {0x5E, 0x11},
            {0x5F, 0x11},
            {0x60, 0x13},
            {0x61, 0x16},
            {0x62, 0x14},
            {0x63, 0x18},
            {0x64, 0x17},
            {0x65, 0x1D},
            {0x66, 0x1C},
            {0x67, 0x1D},
            {0x68, 0x1D},
            {0x69, 0x1D},
            {0x6A, 0x1D},
            {0x6B, 0x1D},
            {0x6C, 0x1D},
            {0x6D, 0x1D},
            {0x6E, 0x1D},
            {0x6F, 0x1D},
            {0x70, 0x1D},
            {0x71, 0x1D},
            {0x72, 0x1D},
            {0x73, 0x1D},
            {0x74, 0x1D},
            {0x75, 0x1D},
            {0x76, 0x1D},
            {0x77, 0x1D},
            {0x78, 0x1D},
            {0x79, 0x1D},
            {0x7A, 0x1D},
            {0x7B, 0x1D},
            {0x7C, 0x1D},
            {0x7D, 0x1D},
            {0x7E, 0x1D},
            {0x7F, 0x1D}
        };
        OV2640_WriteRegList(ov2640_jpeg_reg_tbl, sizeof(ov2640_jpeg_reg_tbl)/2);
    }
    else // RGB565
    {
        static const uint8_t ov2640_rgb565_reg_tbl[][2] = {
            {0xFF, 0x00},
            {0xDA, 0x08},
            {0xD7, 0x03},
            {0xDF, 0x02},
            {0x33, 0xa0},
            {0x3C, 0x00},
            {0xe1, 0x67},
            {0x00, 0x00}
        };
        OV2640_WriteRegList(ov2640_rgb565_reg_tbl, sizeof(ov2640_rgb565_reg_tbl)/2);
    }
}

// 开始捕获
void OV2640_StartCapture(void)
{
    // 配置DCMI DMA传输
    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)image_buffer, QVGA_WIDTH * QVGA_HEIGHT*2);
}
// 停止捕获
void OV2640_StopCapture(void)
{
    HAL_DCMI_Stop(&hdcmi);
}


#endif