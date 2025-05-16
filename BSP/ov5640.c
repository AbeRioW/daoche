#include "ov5640.h"

uint16_t OV5640_ReadID(void)
{
    uint8_t id_h, id_l;
    HAL_I2C_Mem_Read(&hi2c1, OV5640_I2C_ADDR, 0x300A, I2C_MEMADD_SIZE_16BIT, &id_h, 1, 100);
    HAL_I2C_Mem_Read(&hi2c1, OV5640_I2C_ADDR, 0x300B, I2C_MEMADD_SIZE_16BIT, &id_l, 1, 100);
    return (id_h << 8) | id_l;
}

void OV5640_WriteReg(uint16_t reg, uint8_t val)
{
    HAL_I2C_Mem_Write(&hi2c1, OV5640_I2C_ADDR, reg, I2C_MEMADD_SIZE_16BIT, &val, 1, 100);
}

uint8_t OV5640_ReadReg(uint16_t reg)
{
    uint8_t val;
    HAL_I2C_Mem_Read(&hi2c1, OV5640_I2C_ADDR, reg, I2C_MEMADD_SIZE_16BIT, &val, 1, 100);
    return val;
}

void OV5640_Init(void)
{
		//复位OV5640
    OV5640_WriteReg(0x3008, 0x80);
    HAL_Delay(100);
    
	// 设置出书格式为RGB565
    OV5640_WriteReg(0x4300, 0x61); // RGB565
    OV5640_WriteReg(0x501F, 0x01); // 使能RGB输出
    
    // ????? (320x240 QVGA)
    OV5640_WriteReg(0x3808, 0x01); // H_SIZE[11:8]
    OV5640_WriteReg(0x3809, 0x40); // H_SIZE[7:0] (320)
    OV5640_WriteReg(0x380A, 0x00); // V_SIZE[11:8]
    OV5640_WriteReg(0x380B, 0xF0); // V_SIZE[7:0] (240)
    
    //设置时钟
    OV5640_WriteReg(0x3035, 0x21); // PLL控制
    OV5640_WriteReg(0x3036, 0x46); // PLL控制
    OV5640_WriteReg(0x3820, 0x46); // 翻转
    OV5640_WriteReg(0x3821, 0x00); // 镜像
    

    
    //启动摄像头
    OV5640_WriteReg(0x3008, 0x02);
}