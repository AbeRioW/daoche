#include "sr04.h"
#include "LCD.h"
#include "tim.h"
#include "ov2640.h"



float distant;     									//��������


uint8_t distance_legth = 5;  //Ĭ�ϵľ���ֵ



volatile bool capture_started = false;
volatile uint32_t start_time;
volatile uint32_t end_time;

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
	  char long_dis[20];
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
								sprintf(long_dis,"%.2f",distant);
								LCD_ShowString(0,220,16,(uint8_t*)long_dis,0);	
							 	HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_2);                             //ֹͣ����
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



