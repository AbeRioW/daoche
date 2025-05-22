#include "sr04.h"
#include "LCD.h"
#include "tim.h"



float distant;     									//测量距离
uint32_t measure_buf[3] = {0};			//存放定时器计数值的数组
uint8_t measure_cnt = 0;  				  //状态标志位
uint32_t higt_time;                 //超声波模块返回高电平时间


uint8_t distance_legth = 5;  //默认的距离值
bool distance_boundary = false;

//us延时函数
void delay_SR_us(uint32_t us)
{
		uint32_t delay = (HAL_RCC_GetHCLKFreq()/8000000*us);
	  while(delay--);
}

////读取距离
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
////				HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_2);      //启动输入捕获
////				break;
////			case 3:
////				 higt_time = measure_buf[1]-measure_buf[0];      //高电平时间
////			  
////			   distant = (higt_time*0.034)/2;                	//单位是cm
////			
////			   sprintf(long_dis,"%.2f",distant);
////					LCD_ShowString(0,220,16,(uint8_t*)long_dis,0);	
////			  if(distant <=distance_legth) 
////				{
////						distance_boundary = true;
////				}
////			  measure_cnt = 0;
////			  TIM1->CNT = 0;                //情况计时器技术
////				HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_2);
////			
////			  break;
////		}
//}

////回调函数
//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
////		if(TIM1==htim->Instance)    //判断触发的中断的定时器为TIM2
////		{
////				switch(measure_cnt)
////				{
////					case 1:
////							measure_buf[0] = HAL_TIM_ReadCapturedValue(&htim1,TIM_CHANNEL_2);     //获取当前的捕获值
////					    __HAL_TIM_SET_CAPTUREPOLARITY(&htim1,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING);         //设置为下降沿捕获
////							measure_cnt++;
////					break;
////					
////					case 2:
////							measure_buf[1] = HAL_TIM_ReadCapturedValue(&htim1,TIM_CHANNEL_2);     //获取当前的捕获值
////					    HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_2);                             //停止捕获
////					    measure_cnt++;
////					break;
////				}
////		}
//}

volatile bool capture_started = false;
volatile uint32_t start_time;
volatile uint32_t end_time;

/* 触发HC - SR04测量函数 */
void sr04_trigger_measurement(void)
{
    /* 发送触发信号 */
    TRIG_H;
    delay_SR_us(30);
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



