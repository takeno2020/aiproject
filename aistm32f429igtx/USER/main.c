/*
********************************************************************************
*                          SYS INCLUDES
********************************************************************************
*/
#include <aigpio.h>
#include <aisys.h>
#include <aidelay.h>

/*
********************************************************************************
*                        HARDWARE INCLUDES
********************************************************************************
*/
#include "ailed.h"
#include "aikey.h"
#include "aiusart.h"
#include "aiwm9825g6kh.h"
#include "ailtdc.h"
#include "ailcd.h"
#include "aipcf8574.h"
#include "aiap3216c.h"

/*
********************************************************************************
*                        Private variables、define
********************************************************************************
*/
// const u8 ai_text_buf[] = {"ALIENTEK Apollo STM32F429 IIC TEST2"};

/*
********************************************************************************
*                              MAIN
********************************************************************************
*/
int main(void)
{
    u16 timeout = 0, j;
    u8 key = AI_KEY_ALL_UP;
    u8 beep_stat = 1;
    u16 ir, als, ps;
    
    /* 设置时钟180MHz */
    ai_sys_clock_init(360, 25, 2, 8);
    ai_delay_init(180);
	
    /* 外设初始化 */
	ai_uart_init(90, 115200);
    ai_led_init();
    ai_key_init();
    ai_wm9825g6kh_init();
    ai_lcd_init();
    ai_delay_ms(100);
     
    /* 设置外设的开始运行状态 */
    ai_led_on(AI_LED_DS0);
    ai_led_off(AI_LED_DS1);
    
	ai_brush_color = AI_RED; 
    ai_lcd_show_str(10, 40, 240, 16, 16, (u8 *)"Apollo STM32");
    ai_lcd_show_str(10, 60, 240, 16, 16, (u8 *)"PCF8574 & AP3216 CTEST");
    ai_lcd_show_str(10, 80, 240, 16, 16, (u8 *)"ATOM@ALIENTEK");
    ai_lcd_show_str(10, 100, 240, 16, 16, (u8 *)"2021/01/30");
    ai_lcd_show_str(10, 120, 240, 16, 16, "KEY0:BEEP ON/OFF");
    ai_lcd_show_str(10, 140, 240, 16, 16, "EXIO:DS1 ON/OFF");
    
    while (ai_pcf8574_init() != 0) {
        ai_lcd_show_str(10, 160, 240, 16, 16, "PCF8574 Check Failed!");
        ai_delay_ms(500);
        ai_lcd_show_str(10, 160, 240, 16, 16, "Please Check!        ");
        ai_delay_ms(500);
        AI_DS0 = !AI_DS0;
    }
    ai_lcd_show_str(10, 180, 240, 16, 16, "PCF8574 Ready!");
    ai_brush_color = AI_BLUE;
    
    while (ai_ap3216c_init() != 0) {
        ai_lcd_show_str(10, 200, 240, 16, 16, "AP3216C Check Failed!");
        ai_delay_ms(500);
        ai_lcd_show_str(10, 200, 240, 16, 16, "Please Check!        ");
        ai_delay_ms(500);
        AI_DS1 = !AI_DS1;
    }
    ai_lcd_show_str(10, 200, 240, 16, 16, "AP3216C Ready!");
    ai_lcd_show_str(10, 220, 240, 16, 16, " IR:");
    ai_lcd_show_str(10, 240, 240, 16, 16, " PS:");
    ai_lcd_show_str(10, 260, 240, 16, 16, "ALS:");
    ai_brush_color = AI_GREEN;
    
    /* main loop */
    while (1) {
        key = ai_key_scan(0);
        if (key == AI_KEY0_DOWN) {
            beep_stat = !beep_stat;
            ai_pcf8574_write_bit(AI_BEEP_IO, beep_stat);
        }
        
        // PCF8574的中断低电平有效
        if (AI_PCF8574_INT == 0) {
            // 读取EXIO状态,同时清除PCF8574的中断输出(INT恢复高电平)
            key = ai_pcf8574_read_bit(AI_EX_IO);
            if (key == 0)
                AI_DS1 = !AI_DS1;
        }
        
        if (j == 120) {
            ai_ap3216c_read_data(&ir, &ps, &als);
            ai_lcd_show_num(10 + 32, 220, ir, 5, 16);
            ai_lcd_show_num(10 + 32, 240, ps, 5, 16);
            ai_lcd_show_num(10 + 32, 260, als, 5, 16);
            j = 0;
        }
        
        timeout++;
        j++;
        ai_delay_ms(10);
        if (timeout == 20) {
            AI_DS0 = !AI_DS0;
            timeout = 0;
        }
    }
}
