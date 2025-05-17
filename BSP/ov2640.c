#include "ov2640.h"

uint8_t ov2640_init(void)
{
		uint16_t reg;
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