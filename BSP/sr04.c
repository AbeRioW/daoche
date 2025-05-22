#include "sr04.h"
#include "LCD.h"
#include "tim.h"



float distant;     									//��������
uint32_t measure_buf[3] = {0};			//��Ŷ�ʱ������ֵ������
uint8_t measure_cnt = 0;  				  //״̬��־λ
uint32_t higt_time;                 //������ģ�鷵�ظߵ�ƽʱ��


uint8_t distance_legth = 5;  //Ĭ�ϵľ���ֵ
bool distance_boundary = false;

//us��ʱ����
void delay_SR_us(uint32_t us)
{
		uint32_t delay = (HAL_RCC_GetHCLKFreq()/8000000*us);
	  while(delay--);
}

////��ȡ����
//void sr04_getdata(void)
//{
////	  char long_dis[20]={0};
////		switch(measure_cnt)
////		{
////			case 0:
////				TRIG_H;
////			  delay_SR_us(30);
////			  TRIG_L;
////			
////			  measure_cnt++;
////			  __HAL_TIM_SET_CAPTUREPOLARITY(&htim1,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
////				HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_2);      //�������벶��
////				break;
////			case 3:
////				 higt_time = measure_buf[1]-measure_buf[0];      //�ߵ�ƽʱ��
////			  
////			   distant = (higt_time*0.034)/2;                	//��λ��cm
////			
////			   sprintf(long_dis,"%.2f",distant);
////					LCD_ShowString(0,220,16,(uint8_t*)long_dis,0);	
////			  if(distant <=distance_legth) 
////				{
////						distance_boundary = true;
////				}
////			  measure_cnt = 0;
////			  TIM1->CNT = 0;                //�����ʱ������
////				HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_2);
////			
////			  break;
////		}
//}

////�ص�����
//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
////		if(TIM1==htim->Instance)    //�жϴ������жϵĶ�ʱ��ΪTIM2
////		{
////				switch(measure_cnt)
////				{
////					case 1:
////							measure_buf[0] = HAL_TIM_ReadCapturedValue(&htim1,TIM_CHANNEL_2);     //��ȡ��ǰ�Ĳ���ֵ
////					    __HAL_TIM_SET_CAPTUREPOLARITY(&htim1,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING);         //����Ϊ�½��ز���
////							measure_cnt++;
////					break;
////					
////					case 2:
////							measure_buf[1] = HAL_TIM_ReadCapturedValue(&htim1,TIM_CHANNEL_2);     //��ȡ��ǰ�Ĳ���ֵ
////					    HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_2);                             //ֹͣ����
////					    measure_cnt++;
////					break;
////				}
////		}
//}

volatile bool capture_started = false;
volatile uint32_t start_time;
volatile uint32_t end_time;

/* ����HC - SR04�������� */
void sr04_trigger_measurement(void)
{
    /* ���ʹ����ź� */
    TRIG_H;
    delay_SR_us(30);
    TRIG_L;
			  __HAL_TIM_SET_CAPTUREPOLARITY(&htim1,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
				HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_2);      //�������벶��
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



