// main/apps/apps.h —— 口袋上新的玩法表
//
// 加一个新玩法只要两步：
//   1. 在 main/apps/ 下新建 app_xxx.c，导出一个 const page_t app_xxx（enter / exit / key 三个函数）
//   2. 在 main/apps/apps.c 的 APPS[] 里加一行 { "名字", "一句话介绍", &app_xxx }
// main/apps/ 下的 .c 文件会被自动编译，不用改 CMakeLists.txt。
#pragma once
#include "pages.h"

typedef struct {
    const char   *name;   // 列表里显示的名字，建议「××猹」，不超过 6 个字
    const char   *desc;   // 选中时底部显示的一句话介绍，不超过 16 个字
    const page_t *page;   // 玩法页面
} app_entry_t;

extern const app_entry_t APPS[];
extern const int APPS_N;
