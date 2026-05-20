/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       跑马灯 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 MiniSTM32 V4开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */



#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/TIMER/btim.h"

uint8_t state = 0;

void led_breath(void);

uint8_t temp_fun(void) {return key_scan(1);}

int main(void)
{
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);         /* 设置时钟,72M */
    delay_init(72);                             /* 初始化延时函数 */
    led_init();                                 /* 配置STM32操作LED相关的寄存器 */
    key_init();
    usart_init(115200);
    btim_timx_int_init(18000-1, 4000-1);    // 72M/(18k*4k)=1Hz
    tim1_1s_stop();

    btim_timx_int2_init(72-1, 10);
    LED0_brightness_control(0);

    tim3_init();
    
    uint8_t brightness_level = 0;
    while(1)
    {
        uint8_t keyval = key_scan(1);
        // HAL_UART_Transmit(&g_uart1_handle, &keyval, 1, 1000);
        if (keyval==KEY1_PRES) {
            state=1;
            tim1_1s_start();
            uint8_t state_mem = 1;
            while (state) {
                keyval=key_scan(1);
                if(keyval!=KEY1_PRES&&(state==1||state==3)) {
                    ++state;
                    state_mem=state;
                } else if (keyval==KEY1_PRES&&state==2) {
                    ++state;
                    state_mem=state;
                }
                if (state==4) {
                    led_breath();
                    brightness_level=0;
                    LED0_brightness_control(0);
                }
            }
            HAL_UART_Transmit(&g_uart1_handle, (uint8_t*)&state_mem, 1, 1000);
            if (state_mem==2 || state_mem==3) {
                brightness_level+=1;
                brightness_level%=4;
                LED0_brightness_control(brightness_level*40);
            }
            else if (state_mem==1) {
                // count press time
                uint32_t press_time = key_press_time_cnt(temp_fun, KEY1_PRES);
                press_time+=1000;
                uint8_t flag=0;
                while (flag==0) {
                    for (uint32_t i = 0; i < press_time; i+=10) {
                        if (i<=press_time/2) {
                            LED0_brightness_control(240);
                        } else {
                            LED0_brightness_control(0);
                        }
                        keyval=key_scan(1);
                        if (keyval==KEY1_PRES) {
                            flag=1;
                        } else if (flag) {
                            LED0_brightness_control(0);
                            break;
                        }
                        delay_ms(10);   
                        // HAL_UART_Transmit(&g_uart1_handle, (uint8_t*)&i, 1, 1000);
                    }
                }
            }
        }   
        delay_ms(50);
    }
}

void led_breath(){
    uint8_t keyval = key_scan(1);
    uint8_t flag = 0;
    while (1) {
        for (int i=32; i<512-32; ++i) {
        if (i<=255) {
            LED0_brightness_control(i);
        } else {
            LED0_brightness_control(511-i);
        }
        keyval=key_scan(1);
        if (keyval==KEY1_PRES) {
            flag=1;
        } else if (flag) {
            return;
        }
        delay_ms(11);   // 5000ms / 448 rounds =11.161ms
        }
    }
}