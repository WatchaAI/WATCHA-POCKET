// main/apps/apps.c —— 口袋上新玩法表：新玩法在这里加一行（顺序 = 列表顺序）
#include "apps.h"

extern const page_t app_decide;   // 决策猹  apps/app_decide.c（示例，写新玩法先读它）

const app_entry_t APPS[] = {
    { "决策猹", "选不出来？交给猹", &app_decide },
};
const int APPS_N = (int)(sizeof APPS / sizeof APPS[0]);
