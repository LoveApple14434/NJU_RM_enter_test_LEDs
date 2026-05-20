/**
 ****************************************************************************************************
 * @file        btim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       基本定时器 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211216
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/LED/led.h"
#include "btim.h"
#include "stm32f103xe.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_tim.h"
#include <stdint.h>
#include "./BSP/TIMER/btim.h"
#include "BSP/KEY/key.h"
#include "./SYSTEM/usart/usart.h"
#include "stm32f1xx_hal_uart.h"


TIM_HandleTypeDef g_timx_handle = {0};  /* 定时器句柄 */
TIM_HandleTypeDef timx_handle2 = {0};
TIM_HandleTypeDef timx_handle3 = {0};
extern uint8_t state;
uint8_t brt_cnt,brt_cmp;
uint32_t tim2_cnt;

/**
 * @brief       基本定时器TIMX定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void btim_timx_int_init(uint16_t arr, uint16_t psc)
{
    g_timx_handle.Instance = BTIM_TIMX_INT;                      /* 通用定时器X */
    g_timx_handle.Init.Prescaler = psc;                          /* 设置预分频系数 */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 递增计数模式 */
    g_timx_handle.Init.Period = arr;                             /* 自动装载值 */
    g_timx_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_timx_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&g_timx_handle);
    HAL_TIM_Base_Start_IT(&g_timx_handle);    /* 使能定时器x及其更新中断 */
}

/**
 * @brief       定时器底层驱动，开启时钟，设置中断优先级
                此函数会被HAL_TIM_Base_Init()函数调用
 * @param       htim:定时器句柄
 * @retval      无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIMX_INT)
    {
        BTIM_TIMX_INT_CLK_ENABLE();                     /* 使能TIM时钟 */
        HAL_NVIC_SetPriority(BTIM_TIMX_INT_IRQn, 1, 3); /* 抢占1，子优先级3，组2 */
        HAL_NVIC_EnableIRQ(BTIM_TIMX_INT_IRQn);         /* 开启ITM3中断 */
    } else if (htim->Instance==TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM3_IRQn, 2, 4);
        HAL_NVIC_EnableIRQ(TIM3_IRQn);
    } else if (htim->Instance==TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM2_IRQn, 2, 4);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
    }
}

/**
 * @brief       定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void BTIM_TIMX_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_handle); /* 定时器中断公共处理函数 */
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&timx_handle2); /* 定时器中断公共处理函数 */
}

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&timx_handle3); /* 定时器中断公共处理函数 */
    
}

/**
 * @brief       定时器更新中断回调函数
 * @param       htim:定时器句柄
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIMX_INT)
    {
        state=0;
        tim1_1s_stop();
    } else if (htim->Instance == TIM3) {
        brt_cnt++;
        if (brt_cnt<brt_cmp) {
            LED0(0);
        } else {
            LED0(1);
        }
    } else if (htim->Instance == TIM2) {
        if (tim2_cnt<0xffffffff) {
            ++tim2_cnt;
        }
    }
}

void tim1_1s_stop()
{
    HAL_TIM_Base_Stop_IT(&g_timx_handle);
}

void tim1_1s_start()
{
    HAL_TIM_Base_Start_IT(&g_timx_handle);
}



/**
 * @brief       基本定时器TIMX定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void btim_timx_int2_init(uint16_t arr, uint16_t psc)
{
    timx_handle2.Instance = TIM3;                      /* 使用 TIM3 作为亮度控制定时器 */
    timx_handle2.Init.Prescaler = psc;                          /* 设置预分频系数 */
    timx_handle2.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 递增计数模式 */
    timx_handle2.Init.Period = arr;                             /* 自动装载值 */
    timx_handle2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timx_handle2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&timx_handle2);
    HAL_TIM_Base_Start_IT(&timx_handle2);    /* 使用中断方式启动 TIM3 */
}

uint8_t LED0_brightness_control(uint8_t brightness)
{
    brt_cmp=brightness;
    return 0;
}


void tim3_init(void)
{
    timx_handle3.Instance = TIM2;                      /* 使用 TIM3 作为亮度控制定时器 */
    timx_handle3.Init.Prescaler = 72-1;                          /* 设置预分频系数 */
    timx_handle3.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 递增计数模式 */
    timx_handle3.Init.Period = 1000-1;                             /* 自动装载值 */
    timx_handle3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timx_handle3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&timx_handle3);
}


uint32_t key_press_time_cnt(uint8_t (*key_pres_func)(void), uint8_t press_ret)
{
    HAL_TIM_Base_Start_IT(&timx_handle3);    /* 使用中断方式启动 TIM2 */
    while (1) {
        uint8_t retval = key_pres_func();
        // HAL_UART_Transmit(&g_uart1_handle, &retval, 1, 100);
        if (retval!=press_ret) {
            uint32_t ret = tim2_cnt;
            // HAL_UART_Transmit(&g_uart1_handle, (uint8_t*)&ret, 4, 1000);
            HAL_TIM_Base_Stop_IT(&timx_handle3);
            tim2_cnt=0;
            return ret;
        }
    }
}


