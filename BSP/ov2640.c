#include "ov2640.h"
#include "ov2640cfg.h"

/*	const u8 *EFFECTS_TBL[7] = {
	"Normal", "Negative", "B&W", "Redish", "Greenish", "Bluish", "Antique"}; //7种特效
const u8 *JPEG_SIZE_TBL[9] = {
	"QCIF", "QQVGA", "CIF", "QVGA", "VGA", "SVGA", "XGA", "SXGA", "UXGA"}; //JPEG图片 9种尺寸
*/
// JPEG 尺寸支持列表
const uint16_t jpeg_img_size_tbl[][2] = {
	176, 144,	//QCIF
	160, 120,	//QQVGA
	352, 288,	//CIF
	320, 240,	//QVGA
	640, 480,	//VGA
	800, 600,	//SVGA
	1024, 768,	//XGA
	1280, 1024, //SXGA
	1600, 1200, //UXGA
};

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

//OV2640切换为JPEG模式
void OV2640_JPEG_Mode(void)
{
	uint16_t i = 0;
	//设置:YUV422格式
	for (i = 0; i < (sizeof(ov2640_yuv422_reg_tbl) / 2); i++)
	{
		SCCB_WR_Reg(ov2640_yuv422_reg_tbl[i][0], ov2640_yuv422_reg_tbl[i][1]);
	}

	//设置:输出JPEG数据
	for (i = 0; i < (sizeof(ov2640_jpeg_reg_tbl) / 2); i++)
	{
		SCCB_WR_Reg(ov2640_jpeg_reg_tbl[i][0], ov2640_jpeg_reg_tbl[i][1]);
	}
}

//设置图像输出大小
//OV2640输出图像的大小(分辨率),完全由该函数确定
//width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
//返回值:0,设置成功
//    其他,设置失败
uint8_t OV2640_OutSize_Set(uint16_t width, uint16_t height)
{
	uint16_t outh;
	uint16_t outw;
	uint8_t temp;
	if (width % 4)
		return 1;
	if (height % 4)
		return 2;
	outw = width / 4;
	outh = height / 4;
	SCCB_WR_Reg(0XFF, 0X00);
	SCCB_WR_Reg(0XE0, 0X04);
	SCCB_WR_Reg(0X5A, outw & 0XFF); //设置OUTW的低八位
	SCCB_WR_Reg(0X5B, outh & 0XFF); //设置OUTH的低八位
	temp = (outw >> 8) & 0X03;
	temp |= (outh >> 6) & 0X04;
	SCCB_WR_Reg(0X5C, temp); //设置OUTH/OUTW的高位
	SCCB_WR_Reg(0XE0, 0X00);
	return 0;
}