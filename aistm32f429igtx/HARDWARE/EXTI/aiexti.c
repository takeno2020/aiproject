#include "aiexti.h"
#include "aidelay.h"
#include "ailed.h"
#include "aikey.h"
#include "aisys.h"

/*
********************************************************************************
* 外部中断0服务程序 - KEY_UP
********************************************************************************
*/
void EXTI0_IRQHandler(void)
{
    ai_delay_ms(10);
    if (AI_WK_UP == 1) {
        AI_DS0 = !AI_DS1;
        AI_DS1 = !AI_DS1;
    }
    EXTI->PR = 0x1 << 0;
}

/*
********************************************************************************
* 外部中断2服务程序 - KEY1
********************************************************************************
*/
void EXTI2_IRQHandler(void)
{
    ai_delay_ms(10);
    if (AI_KEY1 == 0) {
        AI_DS1 = !AI_DS1;
    }
    EXTI->PR = 0x1 << 2;
}

/*
********************************************************************************
* 外部中断3服务程序 - KEY0
********************************************************************************
*/
void EXTI3_IRQHandler(void)
{
    ai_delay_ms(10);
    if (AI_KEY0 == 0) {
        AI_DS0 = !AI_DS0;
    }
    EXTI->PR = 0x1 << 3;
}

/*
********************************************************************************
* 外部中断10~15服务程序 - KEY2
********************************************************************************
*/
void EXTI15_10_IRQHandler(void)
{
    ai_delay_ms(10);
    if (AI_KEY2 == 0) {
        AI_DS0 = !AI_DS0;
        AI_DS1 = !AI_DS1;
    }
    EXTI->PR = 0x1 << 13;
}

/*
********************************************************************************
* function: 外部中断0初始化函数
* 一共有四个按键，分别是：
*     KEY_UP - WK_UP - PA0  : VCC3.3高电平表示按下按键
*     KEY0   - KEY0  - PH3  : 低电平表示按下按键
*     KEY1   - KEY1  - PH2  : 低电平表示按下按键
*     KEY2   - KEY2  - PC13 : 低电平表示按下按键
* 注：
*     所有按键均没有外置上、下拉电阻，需要内部设置上、下电阻
* 初始化四个按键为中断输入
*   测试时直接使用下面的初始化函数进行初始化，main loop中可以使用串口、LED灯内容辅助
* 指示运行状态；
********************************************************************************
*/
void aiextix_init(void)
{
    ai_key_init();
    ai_nvic_exti_cfg(GPIO_A, 0, RTIR);     // 上升沿触发
    ai_nvic_exti_cfg(GPIO_H, 3, FTIR);     // 下降沿触发
    ai_nvic_exti_cfg(GPIO_H, 2, FTIR);
    ai_nvic_exti_cfg(GPIO_C, 13, FTIR);
    
    ai_nvic_init(0, 2, EXTI0_IRQn, 2);     // KEY_UP
    ai_nvic_init(2, 2, EXTI3_IRQn, 2);     // KEY0
    ai_nvic_init(3, 2, EXTI2_IRQn, 2);     // KEY1
    ai_nvic_init(1, 2, EXTI15_10_IRQn, 2); // KEY2
}
