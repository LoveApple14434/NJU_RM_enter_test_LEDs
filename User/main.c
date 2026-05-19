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


// #include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_it.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/TIMER/btim.h"


int main(void)
{
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);         /* 设置时钟,72M */
    delay_init(72);                             /* 初始化延时函数 */
    led_init();                                 /* 配置STM32操作LED相关的寄存器 */
    key_init();
    // usart_init(115200);
    btim_timx_int_init(18000-1, 4000-1);


    while(1)
    {
        // LED0(0);                                /* LED0 亮 */
        // // LED1(1);                                /* LED1 灭 */
        // delay_ms(500);
        // LED0(1);                                /* LED0 灭 */
        // // LED1(0);                                /* LED1 亮 */
        // delay_ms(500);
        // LED0(0);
        uint8_t keyval = key_scan(1);
        // HAL_UART_Transmit(&g_uart1_handle, &keyval, 1, 1000);
        if(keyval==KEY1_PRES) LED0_TOGGLE();
        // delay_ms(500);
    }
}

