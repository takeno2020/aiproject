/*
********************************************************************************
* usmart是从正点原子的工程移植过来的,自己做了格式整理,仅用于研究学习,下面是声明:
* 本程序只供学习使用，未经作者许可，不得用于其它任何用途
* ALIENTEK STM32开发板	   
* 正点原子@ALIENTEK
* 技术论坛:www.openedv.com 
* 版本：V3.3
* 版权所有，盗版必究。
* Copyright(C) 正点原子 2011-2021
* All rights reserved
********************************************************************************
*/
#include "usmart.h"
#include "aiusart.h"
#include "aisys.h"


// 系统命令
u8 *sys_cmd_tab[] = {
    "?",
    "help",
    "list",
    "id",
    "hex",
    "dec",
    "runtime",	   
};

void usmart_print_title()
{
#if USMART_USE_HELP
    printf("-----------------------USMART V3.3-----------------------\r\n");
    printf("    USMART是由ALIENTEK开发的一个灵巧的串口调试互交组件,通过\r\n");
    printf("它,你可以通过串口助手调用程序里面的任何函数,并执行.因此,你可\r\n");
    printf("以随意更改函数的输入参数(支持数字(10/16进制,支持负数)、字符串\r\n");
    printf("、函数入口地址等作为参数),单个函数最多支持10个输入参数,并支持\r\n");
    printf("函数返回值显示.支持参数显示进制设置功能,支持进制转换功能.\r\n");
    printf("技术支持:www.openedv.com\r\n");
    printf("USMART有7个系统命令(必须小写):\r\n");
    printf("?:      获取帮助信息\r\n");
    printf("help:   获取帮助信息\r\n");
    printf("list:   可用的函数列表\r\n\n");
    printf("id:     可用函数的ID列表\r\n\n");
    printf("hex:    参数16进制显示,后跟空格+数字即执行进制转换\r\n\n");
    printf("dec:    参数10进制显示,后跟空格+数字即执行进制转换\r\n\n");
    printf("runtime:1,开启函数运行计时;0,关闭函数运行计时;\r\n\n");
    printf("请按照程序编写格式输入函数名及参数并以回车键结束.\r\n");
    printf("--------------------------ALIENTEK------------------------- \r\n");
#else
    printf("指令失效\r\n");
#endif
}

/*
********************************************************************************
*    Function: usmart_sys_cmd_exe
* Description: 处理系统指令
*       Input: str - 命令字符串地址
*      Output: None
*      Return: 0,成功处理;其他,错误代码;
*      Others: None
********************************************************************************
*/
u8 usmart_sys_cmd_exe(u8 *str)
{
    u8 i;
    u8 sfname[MAX_FNAME_LEN];    // 存放本地函数名
    u8 pnum;
    u8 rval;
    u32 res;
    
    // 得到指令及指令长度
    res = usmart_get_cmdname(str, sfname, &i, MAX_FNAME_LEN);
    if (res)
        return USMART_FUNCERR;        // 错误的指令
    
    str += i;
    for (i = 0; i < sizeof(sys_cmd_tab) / 4; i++) {    // 支持的系统指令
        if (usmart_strcmp(sfname, sys_cmd_tab[i]) == 0)
            break;
    }
    
    switch (i) {
    case 0:
    case 1:    // 帮助指令
        printf("\r\n");
        usmart_print_title();
        break;
    case 2:    // 查询指令
        printf("\r\n");
        printf("------------------------函数清单-------------------------\r\n");
        for (i = 0; i < usmart_dev.fnum; i++)
            printf("%s\r\n", usmart_dev.funs[i].name);
        printf("\r\n");
        break;
    case 3:    // 查询ID
        printf("\r\n");
        printf("------------------------函数 ID--------------------------\r\n");
        for (i = 0; i < usmart_dev.fnum; i++) {
            usmart_get_fname((u8 *)usmart_dev.funs[i].name,
                             sfname, &pnum, &rval);          // 得到本地函数名
            printf("%s id is:\r\n0X%08X\r\n", sfname, usmart_dev.funs[i].func);
        }
        printf("\r\n");
        break;
    case 4:    // hex指令
        printf("\r\n");
        usmart_get_aparm(str, sfname, &i);
        if (i == 0) {    // 参数正常
            i = usmart_str2num(sfname, &res);        // 记录该参数
            if (i == 0) {                            // 进制转换功能
                printf("HEX:0X%X\r\n", res);         // 转为16进制
            } else if (i != 4) {
                return USMART_PARMERR;               // 参数错误.
            } else {                                 // 参数显示设定功能
                printf("16进制参数显示!\r\n");
                usmart_dev.sptype=SP_TYPE_HEX;
            }
        }else {
            return USMART_PARMERR;                   // 参数错误.
        }
        printf("\r\n"); 
        break;
    case 5:    // dec指令
        printf("\r\n");
        usmart_get_aparm(str, sfname, &i);
        if (i == 0) {                                // 参数正常
            i = usmart_str2num(sfname, &res);        // 记录该参数
            if (i == 0) {                            // 进制转换功能
                printf("DEC:%lu\r\n", res);          // 转为10进制
            } else if (i != 4) {
                return USMART_PARMERR;               // 参数错误.
            } else {                                 // 参数显示设定功能
                printf("10进制参数显示!\r\n");
                usmart_dev.sptype=SP_TYPE_DEC;
            }
        } else {
            return USMART_PARMERR;                   // 参数错误. 
        }
        printf("\r\n");
        break;
    case 6:    // runtime指令,设置是否显示函数执行时间
        printf("\r\n");
        usmart_get_aparm(str, sfname, &i);
        if(i == 0) {                                 // 参数正常
            i = usmart_str2num(sfname, &res);        // 记录该参数
            if (i == 0) {                            // 读取指定地址数据功能
                if (USMART_ENTIMX_SCAN == 0) {
                    printf("\r\nError! \r\nTo EN RunTime function,\
                           Please set USMART_ENTIMX_SCAN = 1 first!\r\n");//报错
                } else {
                    usmart_dev.runtimeflag=res;
                    if (usmart_dev.runtimeflag)
                        printf("Run Time Calculation ON\r\n");
                    else
                        printf("Run Time Calculation OFF\r\n");
                }
            }else {
                return USMART_PARMERR;               // 未带参数,或者参数错误
            }
        } else {
            return USMART_PARMERR;                   // 参数错误.
        }
        printf("\r\n");
        break;
    default:    // 非法指令
        return USMART_FUNCERR;
    }
    
    return 0;
}

/*
********************************************************************************
*                                移植注意
* 本例是以stm32为例,如果要移植到其他mcu,请做相应修改.
* usmart_reset_runtime:清除函数运行时间,连同定时器的计数寄存器以及标志位一起清零.
* 并设置重装载值为最大,以最大限度的延长计时时间.
* usmart_get_runtime,获取函数运行时间,通过读取CNT值获取,由于usmart是通过中断调用
* 的函数,所以定时器中断不再有效,此时最大限度只能统计2次CNT的值,也就是:
* 清零后+溢出一次,当溢出超过2次,没法处理,所以最大延时,
* 控制在:2*计数器CNT*0.1ms.对STM32来说,是:13.1s左右
* 其他的:TIM4_IRQHandler和Timer4_Init,需要根据MCU特点自行修改.
* 确保计数器计数频率为:10Khz即可.另外,定时器不要开启自动重装载功能!!
********************************************************************************
*/

#if USMART_ENTIMX_SCAN == 1
/*
********************************************************************************
*    Function: usmart_reset_runtime
* Description: 复位runtime
*       Input: 需要根据所移植到的MCU的定时器参数进行修改
*      Output: None
*      Return: void
*      Others: None
********************************************************************************
*/
void usmart_reset_runtime(void)
{
    TIM4->SR &= ~(1 << 0);    // 清除中断标志位
    TIM4->ARR = 0XFFFF;       // 将重装载值设置到最大
    TIM4->CNT = 0;            // 清空定时器的CNT
    usmart_dev.runtime = 0;	
}

/*
********************************************************************************
*    Function: usmart_get_runtime
* Description: 获得runtime时间
*       Input: 需要根据所移植到的MCU的定时器参数进行修改
*      Output: None
*      Return: 执行时间,单位:0.1ms,最大延时时间为定时器CNT值的2倍*0.1ms
*      Others: 需要根据所移植到的MCU的定时器参数进行修改
********************************************************************************
*/
u32 usmart_get_runtime(void)
{
    if (TIM4->SR & 0X0001) {             // 在运行期间,产生了定时器溢出
        usmart_dev.runtime += 0XFFFF;
    }
    usmart_dev.runtime += TIM4->CNT;
    return usmart_dev.runtime;           // 返回计数值
}

/*
********************************************************************************
*    Function: TIM4_IRQHandler
* Description: 定时器4中断服务程序
*       Input: void
*      Output: None
*      Return: void
*      Others: 非USMART函数,放到这里,仅仅方便移植.
********************************************************************************
*/	 
void TIM4_IRQHandler(void)
{
    if (TIM4->SR & 0X0001) {        // 溢出中断
        usmart_dev.scan();          // 执行usmart扫描
        TIM4->CNT = 0;              // 清空定时器的CNT
        TIM4->ARR = 1000;           // 恢复原来的设置
    }
    
    TIM4->SR &= ~(1 << 0);          // 清除中断标志位 	    
}

/*
********************************************************************************
*    Function: Timer4_Init
* Description: 使能定时器4,使能中断.
*       Input: void
*      Output: None
*      Return: void
*      Others: 非USMART函数,放到这里,仅仅方便移植.
********************************************************************************
*/
void Timer4_Init(u16 arr, u16 psc)
{
    RCC->APB1ENR |= 1 << 2;            // TIM4 时钟使能
    TIM4->ARR = arr;                   // 设定计数器自动重装值
    TIM4->PSC = psc;                   // 预分频器7200,得到10Khz的计数时钟
    TIM4->DIER |= 1 << 0;              // 允许更新中断
    TIM4->CR1 |= 0x01;                 // 使能定时器4
    ai_nvic_init(3, 3, TIM4_IRQn, 2);  //抢占3,子优先级3,组2(组2中优先级最低的)								 
}
#endif

/*
********************************************************************************
*    Function: usmart_init
* Description: 初始化串口控制器
*       Input: sysclk - 系统时钟（Mhz）
*      Output: None
*      Return: void
*      Others: None
********************************************************************************
*/
void usmart_init(u8 sysclk)
{
#if USMART_ENTIMX_SCAN == 1
    // 分频,时钟为10K ,100ms中断一次,注意,计数频率必须为10Khz,
    // 以和runtime单位(0.1ms)同步.
    Timer4_Init(1000, (u32)sysclk * 100 - 1);
#endif
    usmart_dev.sptype = 1;            // 十六进制显示参数
}

/*
********************************************************************************
*    Function: usmart_cmd_rec
* Description: 从str中获取函数名,id,及参数信息
*       Input: str - 字符串指针.
*      Output: None
*      Return: 0,识别成功;其他,错误代码.
*      Others: None
********************************************************************************
*/
u8 usmart_cmd_rec(u8 *str) 
{
    u8 sta, i, rval;             // 状态
    u8 rpnum, spnum;
    u8 rfname[MAX_FNAME_LEN];    // 暂存空间,用于存放接收到的函数名
    u8 sfname[MAX_FNAME_LEN];    // 存放本地函数名
    
    // 得到接收到的数据的函数名及参数个数
    sta = usmart_get_fname(str, rfname, &rpnum, &rval);
    if (sta)
        return sta;              // 错误
    
    for (i = 0; i < usmart_dev.fnum; i++) {
        // 得到本地函数名及参数个数
		sta = usmart_get_fname((u8 *)usmart_dev.funs[i].name,
                               sfname, &spnum, &rval);
        if (sta)
            return sta;                             // 本地解析有误
        if (usmart_strcmp(sfname, rfname) == 0) {   // 相等
            if (spnum > rpnum)
                return USMART_PARMERR;    // 参数错误(输入参数比源函数参数少)
            usmart_dev.id = i;            // 记录函数ID.
            break;                        // 跳出.
		}
    }
    
    if (i == usmart_dev.fnum)
        return USMART_NOFUNCFIND;         // 未找到匹配的函数
    sta = usmart_get_fparam(str, &i);     // 得到函数参数个数
    if (sta)
        return sta;                       // 返回错误
    usmart_dev.pnum = i;                  // 参数个数记录
    
    return USMART_OK;
}

/*
********************************************************************************
*    Function: usmart_exe
* Description: usamrt执行函数,该函数用于最终执行从串口收到的有效函数.
*              最多支持10个参数的函数,更多的参数支持也很容易实现.不过用的很少.
*              一般5个左右的参数的函数已经很少见了.
*              该函数会在串口打印执行情况.形式为:
*                  "函数名(参数1，参数2...参数N)=返回值".
*              当所执行的函数没有返回值的时候,所打印的返回值是一个无意义的数据.
*       Input: void
*      Output: None
*      Return: void
*      Others: None
********************************************************************************
*/
void usmart_exe(void)
{
    u8 id, i;
    u32 res;
    u32 temp[MAX_PARM];          // 参数转换,使之支持了字符串
    u8 sfname[MAX_FNAME_LEN];    // 存放本地函数名
    u8 pnum, rval;
    
    id = usmart_dev.id;
    if (id >= usmart_dev.fnum)
        return;                  // 不执行.
    usmart_get_fname((u8 *)usmart_dev.funs[id].name,
                      sfname, &pnum, &rval);    // 得到本地函数名,及参数个数
    printf("\r\n%s(", sfname);                  // 输出正要执行的函数名
    for (i = 0; i < pnum; i++) {                // 输出参数
        if (usmart_dev.parmtype & (1 << i)) {   // 参数是字符串
            printf("%c", '"');
            printf("%s", usmart_dev.parm + usmart_get_parmpos(i));
            printf("%c", '"');
            temp[i] = (u32)&(usmart_dev.parm[usmart_get_parmpos(i)]);
        } else {                                // 参数是数字
			temp[i] = *(u32 *)(usmart_dev.parm + usmart_get_parmpos(i));
            if (usmart_dev.sptype == SP_TYPE_DEC)
                printf("%ld", temp[i]);         // 10进制参数显示
            else
                printf("0X%X", temp[i]);        // 16进制参数显示
        }
        if (i != pnum - 1)
            printf(",");
    }
    printf(")");
    
#if USMART_ENTIMX_SCAN == 1
    usmart_reset_runtime();                     // 计时器清零,开始计时
#endif
    switch (usmart_dev.pnum) {
    case 0:    // 无参数(void类型)
        res = (*(u32(*)())usmart_dev.funs[id].func)();
        break;
    case 1:    // 有1个参数
        res = (*(u32(*)())usmart_dev.funs[id].func)(temp[0]);
        break;
    case 2:    // 有2个参数
        res = (*(u32(*)())usmart_dev.funs[id].func)(temp[0], temp[1]);
        break;
    case 3:    // 有3个参数
        res = (*(u32(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2]);
        break;
    case 4:    // 有4个参数
        res = (*(u32(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2],
                                                    temp[3]);
        break;
    case 5:    // 有5个参数
        res = (*(u32(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2],
                                                    temp[3], temp[4]);
        break;
    case 6:    // 有6个参数
        res = (*(u32(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2],
                                                    temp[3], temp[4], temp[5]);
        break;
    case 7:    // 有7个参数
        res = (*(u32(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2],
                                                    temp[3], temp[4], temp[5],
                                                    temp[6]);
        break;
    case 8:    // 有8个参数
        res = (*(u32(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2],
                                                    temp[3], temp[4], temp[5],
                                                    temp[6], temp[7]);
        break;
    case 9:    // 有9个参数
        res = (*(u32(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2],
                                                    temp[3], temp[4], temp[5],
                                                    temp[6], temp[7], temp[8]);
        break;
    case 10:    // 有10个参数
        res = (*(u32(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2],
                                                    temp[3], temp[4], temp[5],
                                                    temp[6], temp[7], temp[8],
                                                    temp[9]);
        break;
	}
    
#if USMART_ENTIMX_SCAN == 1
    usmart_get_runtime();                // 获取函数执行时间
#endif
    if (rval == 1) {                     // 需要返回值.
        if (usmart_dev.sptype == SP_TYPE_DEC)
            printf("=%lu;\r\n", res);    // 输出执行结果(10进制参数显示)
        else
            printf("=0X%X;\r\n", res);   // 输出执行结果(16进制参数显示)	   
	} else {
        printf(";\r\n");                 // 不需要返回值,直接输出结束
    }
	
    if (usmart_dev.runtimeflag) {        // 需要显示函数执行时间
        printf("Function Run Time:%d.%1dms\r\n",
               usmart_dev.runtime / 10, usmart_dev.runtime % 10);
    }	
}

/*
********************************************************************************
*    Function: usmart_scan
* Description: usmart扫描函数
*       Input: void
*      Output: None
*      Return: void
*      Others: 通过调用该函数,实现usmart的各个控制.
*              该函数需要每隔一定时间被调用一次以及时执行从串口发过来的各个函数.
*              本函数可以在中断里面调用,从而实现自动管理.
*              如果非ALIENTEK用户,则AI_USART_RX_STA和AI_USART_RX_BUF[]
*              需要用户自己实现
********************************************************************************
*/
void usmart_scan(void)
{
    u8 sta, len;
    
    if (AI_USART_RX_STA & 0x8000) {                 // 串口接收完成?
        len = AI_USART_RX_STA & 0x3fff;             // 得到此次接收到的数据长度
        AI_USART_RX_BUF[len] = '\0';                // 在末尾加入结束符. 
        sta = usmart_dev.cmd_rec(AI_USART_RX_BUF);  // 得到函数各个信息
        if (sta == 0) {
            usmart_dev.exe();                       // 执行函数
        } else {
            len = usmart_sys_cmd_exe(AI_USART_RX_BUF);
            if (len != USMART_FUNCERR)
                sta = len;
            if (sta) {
                switch (sta) {
                case USMART_FUNCERR:
                    printf("函数错误!\r\n");
                    break;
                case USMART_PARMERR:
                    printf("参数错误!\r\n");
                    break;
                case USMART_PARMOVER:
                    printf("参数太多!\r\n");
                    break;
                case USMART_NOFUNCFIND:
                    printf("未找到匹配的函数!\r\n");
                    break;
                }
            }
        }
        AI_USART_RX_STA = 0;                         // 状态寄存器清空
    }
}

#if USMART_USE_WRFUNS == 1                           // 如果使能了读写操作
// 读取指定地址的值		 
u32 read_addr(u32 addr)
{
    return *(u32 *)addr;	
}

// 在指定地址写入指定的值		 
void write_addr(u32 addr, u32 val)
{
    *(u32 *)addr = val;
}
#endif
