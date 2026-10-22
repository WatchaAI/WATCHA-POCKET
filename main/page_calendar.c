// main/page_calendar.c —— 猹日历（占位，路由验收用）
#include "pages.h"

LV_FONT_DECLARE(font_fusion12);
LV_FONT_DECLARE(font_ps8);

static lv_obj_t *s_scr;

static void page_enter(void)
{
    s_scr = page_screen_create();
    page_stub_fill(s_scr, "猹日历", &font_fusion12);
    lv_screen_load(s_scr);
}

static void page_exit(void)
{
    if (s_scr) { lv_obj_delete(s_scr); s_scr = NULL; }
}

static void page_key_cb(bsp_btn_t btn, bsp_btn_ev_t ev)
{
    (void)btn; (void)ev;
}

const page_t page_calendar = { "page_calendar", page_enter, page_exit, page_key_cb };
