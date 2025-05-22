#include "sr04.h"
#include "LCD.h"
#include "tim.h"
#include "ov2640.h"



float distant;     									//测量距离


uint8_t distance_legth = 5;  //默认的距离值



volatile bool capture_started = false;
volatile uint32_t start_time;
volatile uint32_t end_time;

//us延时函数
void delay_SR_us(uint32_t us)
{
		uint32_t delay = (HAL_RCC_GetHCLKFreq()/8000000*us);
	  while(delay--);
}

/* 触发HC - SR04测量函数 */
void sr04_trigger_measurement(void)
{
    /* 发送触发信号 */
    TRIG_H;
    //delay_SR_us(30);
    TRIG_L;
			  __HAL_TIM_SET_CAPTUREPOLARITY(&htim1,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
				HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_2);      //启动输入捕获
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

                // 计算距离
                uint32_t duration = end_time - start_time;
                distant = (duration * 0.034) / 2; // 单位: cm

                // 在这里可以进行距离处理，例如显示距离
								sprintf(long_dis,"%.2f",distant);
								LCD_ShowString(0,220,16,(uint8_t*)long_dis,0);	
							//OV2640_StartCapture();
            }
            else
            {
                start_time = capture_value;
                capture_started = true;

                // 切换捕获极性为下降沿
                __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
            }
        }
    }
}



