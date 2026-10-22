// main/dbg.h —— 分步调试打点。屏上显示 tag+剩余堆并强制刷新；tag 同时写 RTC 内存，
// 异常复位(panic/看门狗/掉电)后重启，首页显示 "crash@<最后一个tag>"。DBG_ON=0 全部编译掉。
#pragma once
#include "lvgl.h"
#include <stdbool.h>
#define DBG_ON 1
#if DBG_ON
void        dbg_init(void);            // main：storage_init 之后调一次
const char *dbg_report(void);          // 上次异常复位 → "crash@tag rst=N"；正常 → NULL
void        dbg_attach(lv_obj_t *scr); // 页面 enter 开头：在该屏建打点标签
void        dbg_step(const char *tag); // 打点
void        dbg_detach(void);          // 页面 exit
int         dbg_reset_reason(void);    // esp_reset_reason()
#else
#define dbg_init()      ((void)0)
#define dbg_report()    ((const char *)0)
#define dbg_attach(s)   ((void)0)
#define dbg_step(t)     ((void)0)
#define dbg_detach()    ((void)0)
#define dbg_reset_reason() (0)
#endif
