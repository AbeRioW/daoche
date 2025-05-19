#include "LCD.h"

uint16_t POINT_COLOR = 0x0000,BACK_COLOR = COLOR_RED;  

//写命令
void LCD_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
}


// 写数据
void LCD_WriteData(uint8_t data) {
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
}

// 写16位数据（用于颜色数据）
void LCD_WriteData16(uint16_t data) {
    uint8_t buf[2];
    buf[0] = data >> 8;
    buf[1] = data & 0xFF;
    
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, buf, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
}

// 设置显示窗口
void LCD_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    LCD_WriteCommand(0x2A); // 列地址设置
    LCD_WriteData(x0 >> 8);
    LCD_WriteData(x0 & 0xFF);
    LCD_WriteData(x1 >> 8);
    LCD_WriteData(x1 & 0xFF);
    
    LCD_WriteCommand(0x2B); // 行地址设置
    LCD_WriteData(y0 >> 8);
    LCD_WriteData(y0 & 0xFF);
    LCD_WriteData(y1 >> 8);
    LCD_WriteData(y1 & 0xFF);
    
    LCD_WriteCommand(0x2C); // 写入RAM
}

// 填充颜色
void LCD_Fill(uint16_t color) {
    uint32_t i;
    LCD_SetWindow(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
    
    for(i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
        LCD_WriteData16(color);
    }
}

// ILI9341初始化
void LCD_Init(void) {
    // 硬件复位
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    
    // 初始化序列
    LCD_WriteCommand(0xCF);
    LCD_WriteData(0x00);
    LCD_WriteData(0xC1);
    LCD_WriteData(0x30);
    
    LCD_WriteCommand(0xED);
    LCD_WriteData(0x64);
    LCD_WriteData(0x03);
    LCD_WriteData(0x12);
    LCD_WriteData(0x81);
    
    LCD_WriteCommand(0xE8);
    LCD_WriteData(0x85);
    LCD_WriteData(0x00);
    LCD_WriteData(0x78);
    
    LCD_WriteCommand(0xCB);
    LCD_WriteData(0x39);
    LCD_WriteData(0x2C);
    LCD_WriteData(0x00);
    LCD_WriteData(0x34);
    LCD_WriteData(0x02);
    
    LCD_WriteCommand(0xF7);
    LCD_WriteData(0x20);
    
    LCD_WriteCommand(0xEA);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    
    LCD_WriteCommand(0xC0);    //Power control
    LCD_WriteData(0x23);       //VRH[5:0]
    
    LCD_WriteCommand(0xC1);    //Power control
    LCD_WriteData(0x10);       //SAP[2:0];BT[3:0]
    
    LCD_WriteCommand(0xC5);    //VCM control
    LCD_WriteData(0x3e);       //Contrast
    LCD_WriteData(0x28);
    
    LCD_WriteCommand(0xC7);    //VCM control2
    LCD_WriteData(0x86);       //--
    
    LCD_WriteCommand(0x36);    // Memory Access Control
    LCD_WriteData(0x48);       // 设置显示方向
    
    LCD_WriteCommand(0x3A);
    LCD_WriteData(0x55);       // 16位像素格式
    
    LCD_WriteCommand(0xB1);
    LCD_WriteData(0x00);
    LCD_WriteData(0x18);
    
    LCD_WriteCommand(0xB6);    // Display Function Control
    LCD_WriteData(0x08);
    LCD_WriteData(0x82);
    LCD_WriteData(0x27);
    
    LCD_WriteCommand(0xF2);    // 3Gamma Function Disable
    LCD_WriteData(0x00);
    
    LCD_WriteCommand(0x26);    //Gamma curve selected
    LCD_WriteData(0x01);
    
    LCD_WriteCommand(0xE0);    //Set Gamma
    LCD_WriteData(0x0F);
    LCD_WriteData(0x31);
    LCD_WriteData(0x2B);
    LCD_WriteData(0x0C);
    LCD_WriteData(0x0E);
    LCD_WriteData(0x08);
    LCD_WriteData(0x4E);
    LCD_WriteData(0xF1);
    LCD_WriteData(0x37);
    LCD_WriteData(0x07);
    LCD_WriteData(0x10);
    LCD_WriteData(0x03);
    LCD_WriteData(0x0E);
    LCD_WriteData(0x09);
    LCD_WriteData(0x00);
    
    LCD_WriteCommand(0xE1);    //Set Gamma
    LCD_WriteData(0x00);
    LCD_WriteData(0x0E);
    LCD_WriteData(0x14);
    LCD_WriteData(0x03);
    LCD_WriteData(0x11);
    LCD_WriteData(0x07);
    LCD_WriteData(0x31);
    LCD_WriteData(0xC1);
    LCD_WriteData(0x48);
    LCD_WriteData(0x08);
    LCD_WriteData(0x0F);
    LCD_WriteData(0x0C);
    LCD_WriteData(0x31);
    LCD_WriteData(0x36);
    LCD_WriteData(0x0F);
    
    LCD_WriteCommand(0x11);    //Exit Sleep
    HAL_Delay(120);
    
    LCD_WriteCommand(0x29);    //Display on
    LCD_WriteCommand(0x2C);    //开始显示
}

// 绘制图像
void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *img)
{
    // 设置显示窗口
    LCD_SetWindow(x, y, x + w - 1, y + h - 1);
    
    // 发送图像数据
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET); // 数据模式
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET); // 选中LCD
    
    uint32_t size = w * h*2; // RGB565每个像素2字节
    HAL_SPI_Transmit(&hspi3,img, size, 1000);
    
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET); // 取消选中LCD
}

//#define SIZE 64
//uint8_t checkerboard[SIZE * SIZE * 2];

//void createCheckerboard() {
//    for(int y = 0; y < SIZE; y++) {
//        for(int x = 0; x < SIZE; x++) {
//            uint16_t color = ((x / 8 + y / 8) % 2) ? 0xFFFF : 0x0000; // 黑白交替
//            int index = (y * SIZE + x) * 2;
//            checkerboard[index] = color >> 8;
//            checkerboard[index+1] = color & 0xFF;
//        }
//    }
//		
//		ILI9341_DrawImage(50,50,SIZE,SIZE,checkerboard);
//}

// 绘制像素点
void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if(x >= 240 || y >= 320) return;
    
    LCD_SetWindow(x, y, x, y);
    LCD_WriteData16(color);
}

// 绘制字符串
void LCD_ShowChar(uint16_t x,uint16_t y,uint16_t fc, uint16_t bc, uint8_t num,uint8_t size,uint8_t mode)
{  
    uint8_t temp;
    uint8_t pos,t;
	uint16_t colortemp=POINT_COLOR;      
		   
	num=num-' ';//得到偏移后的值
	LCD_SetWindow(x,y,x+size/2-1,y+size-1);//设置单个文字显示窗口
	if(!mode) //非叠加方式
	{		
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)LCD_WriteData16(fc); 
				else LCD_WriteData16(bc); 
				temp>>=1; 
				
		    }
			
		}	
	}else//叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {   
						POINT_COLOR=fc;              
		        if(temp&0x01)ILI9341_DrawPixel(x+t,y+pos,POINT_COLOR);//画一个点    
		        temp>>=1; 
		    }
		}
	}
	POINT_COLOR=colortemp;	
	LCD_SetWindow(0,0,320-1,240-1);//恢复窗口为全屏    	   	 	  
}

void LCD_ShowString(uint16_t x,uint16_t y,uint8_t size,uint8_t *p,uint8_t mode)
{         
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {   
		if(x>(320-1)||y>(240-1)) 
		return;     
        LCD_ShowChar(x,y,POINT_COLOR,BACK_COLOR,*p,size,mode);
        x+=size/2;
        p++;
    }  
} 