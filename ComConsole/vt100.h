/* 
 * File:   kbd.h
 * Author: root
 *
 * Created on 2009��12��23��, ����9:13
 */

#ifndef _VT100_H
#define	_VT100_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef  __ARM_ARCH_3__
#define RELEASE_ON_ARM
#endif  //__ARM_ARCH_3__

//===============================================ANSI控制码的说明
#define VT100_FLAG0         "\x1b"          //ESC
#define VT100_FLAG1         "["
#define VT100_FLAG          VT100_FLAG0 VT100_FLAG1
#define VT100_HEAD          VT100_FLAG
#define VT100_UP            VT100_HEAD "A"      //ESC[nA 光标上移n�?
#define VT100_DOWN          VT100_HEAD "B"      //ESC[nB 光标下移n�?
#define VT100_RIGHT         VT100_HEAD "C"      //ESC[nC 光标右移n�?
#define VT100_LEFT          VT100_HEAD "D"      //ESC[nD 光标左移n�?
#define VT100_CLR2EOF       VT100_HEAD "J"      //
#define VT100_CLR_EOL       VT100_HEAD "K"      //ESC[K 清除从光标到行尾的内�?
#define VT100_CLEAR         VT100_HEAD "2J"     //ESC[2J 清屏
#define VT100_SAVE_CURSOR   VT100_HEAD "s"      //ESC[s 保存光标位置
#define VT100_LOAD_CURSOR   VT100_HEAD "u"      //ESC[u 恢复光标位置
#define VT100_HIDE_CURSOR   VT100_HEAD "?25l"   //ESC[?25l 隐藏光标
#define VT100_SHOW_CURSOR   VT100_HEAD "?25h"   //ESC[?25h 显示光标
#define VT100_RESET         VT100_HEAD "0m"     //ESC[0m 关闭所有属�?
#define VT100_HIGHT_LIGHT   VT100_HEAD "1m"     //ESC[1m 设置高亮�?
#define VT100_UNDER_LINE    VT100_HEAD "4m"     //ESC[4m 下划�?
#define VT100_FLASH         VT100_HEAD "5m"     //ESC[5m 闪烁
#define VT100_INVERT        VT100_HEAD "7m"     //ESC[7m 反显
#define VT100_INVISIBLE     VT100_HEAD "8m"     //ESC[8m 消隐
#define VT100_GOTO(x,y)     VT100_HEAD x ";" y "H" VT100_CLR_EOL    //ESC[y;xH设置光标位置
#define VT100_COLOR(b,f)    VT100_HEAD b ";" f "m"                  //
//ESC[30m&nbsp-- ESC[37m 设置前景�?//ESC[40m&nbsp-- ESC[47m 设置背景�?
//字颜�?30-----------39
#define VT100_F_BLACK       "30"    //�?
#define VT100_F_RED         "31"    //�?
#define VT100_F_GREEN       "32"    //�?
#define VT100_F_YELLOW      "33"    //�?
#define VT100_F_BLUE        "34"    //蓝色
#define VT100_F_PINK        "35"    //紫色
#define VT100_F_DARK_G      "36"    //深绿
#define VT100_F_WHITE       "37"    //白色

//字背景颜色范�?40----49
#define VT100_B_BLACK       "40"    //�?
#define VT100_B_RED         "41"    //深红
#define VT100_B_GREEN       "42"    //�?
#define VT100_B_YELLOW      "43"    //黄色
#define VT100_B_BLUE        "44"    //蓝色
#define VT100_B_PINK        "45"    //紫色
#define VT100_B_DARK_G      "46"    //深绿
#define VT100_B_WHITE       "47"    //白色

//#define VT100_STYLE0        VT100_RESET
#define VT100_STYLE0        VT100_COLOR(VT100_B_BLACK,VT100_F_GREEN)
//#define VT100_STYLE0        VT100_COLOR(VT100_F_BLUE,VT100_F_YELLOW)
#define VT100_STYLE1        VT100_COLOR(VT100_B_BLACK,VT100_F_WHITE) VT100_HIGHT_LIGHT
#define VT100_STYLE2        VT100_COLOR(VT100_B_DARK_G,VT100_F_WHITE) VT100_HIGHT_LIGHT
#define VT100_STYLE_MENU    VT100_RESET VT100_COLOR(VT100_B_BLACK,VT100_F_GREEN) VT100_HIGHT_LIGHT
#define VT100_STYLE_KEY     VT100_COLOR(VT100_B_BLACK,VT100_F_YELLOW) VT100_HIGHT_LIGHT VT100_UNDER_LINE
#define VT100_STYLE_HOT     VT100_RESET VT100_COLOR(VT100_B_BLACK,VT100_F_RED) VT100_HIGHT_LIGHT


#ifdef	__cplusplus
}
#endif

#endif	/* _VT100_H */

