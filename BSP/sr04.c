#include "sr04.h"
#include "LCD.h"
#include "tim.h"
#include "ov2640.h"
#include "usart.h"



float distant;     									//��������


uint8_t distance_legth = 5;  //Ĭ�ϵľ���ֵ



volatile bool capture_started = false;
volatile uint32_t start_time;
volatile uint32_t end_time;

uint8_t diyu10[15]={0xFD,0x00,0x0C,0x01,0x01,0xB5,0xCD,0xD3,0xDA,0xCA,0xAE,0xC0,0xE5,0xC3,0xD7};
uint8_t diyu20[17]={0xFD,0x00,0x0E,0x01,0x01,0xB5,0xCD,0xD3,0xDA,0xB6,0xFE,0xCA,0xAE,0xC0,0xE5,0xC3,0xD7}; 
uint8_t diyu30[17]={0xFD,0x00,0x0E,0x01,0x01,0xB5,0xCD,0xD3,0xDA,0xC8,0xFD,0xCA,0xAE,0xC0,0xE5,0xC3,0xD7}; 
uint8_t diyu40[17]={0xFD,0x00,0x0E,0x01,0x01,0xB5,0xCD,0xD3,0xDA,0xCB,0xC4,0xCA,0xAE,0xC0,0xE5,0xC3,0xD7}; 

//us��ʱ����
void delay_SR_us(uint32_t us)
{
		uint32_t delay = (HAL_RCC_GetHCLKFreq()/8000000*us);
	  while(delay--);
}

/* ����HC - SR04�������� */
void sr04_trigger_measurement(void)
{
    /* ���ʹ����ź� �ߵ�ƽ*/
    TRIG_H;
	
	  HAL_TIM_Base_Start_IT(&htim3);  //����������ʱ��3

}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim)
{
	 static  bool sound = false;
	 		char show_image[20];
	  char long_dis[40];
    if (htim->Instance == TIM1)
    {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {
            uint32_t capture_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

            if (capture_started)
            {
                end_time = capture_value;
                capture_started = false;

                // �������
                uint32_t duration = end_time - start_time;
                distant = (duration * 0.034) / 2; // ��λ: cm

                // ��������Խ��о��봦��������ʾ����
							sprintf(long_dis,"dis:%.2f",distant);
								LCD_ShowString(0,220,16,(uint8_t*)long_dis,0);	
							
							sprintf(show_image,"dia_band:%d",dis_band);
						LCD_ShowString(0,200,16,(uint8_t*)show_image,0);	
							 	HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_2);                             //ֹͣ����
							
							 if(distant<=dis_band)
							 {
								    HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
								 if(sound==false)
								 {
									switch (dis_band)
									{
										case 10:
											 HAL_UART_Transmit(&huart1,diyu10,15,0xffff);
										  sound= true;
											break;
										case 20:
											 HAL_UART_Transmit(&huart1,diyu20,17,0xffff);
																				  sound= true;
											break;
										case 30:
											 HAL_UART_Transmit(&huart1,diyu30,17,0xffff);
																				  sound= true;
											break;
										case 40:
											 HAL_UART_Transmit(&huart1,diyu40,17,0xffff);
																				  sound= true;
											break;
									}
								 }

							 }
							 else
							 {
									 HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);
								   sound= false;
							 }
            }
            else
            {
                start_time = capture_value;
                capture_started = true;

                // �л�������Ϊ�½���
                __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
            }
        }
    }
}



