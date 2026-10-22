// main/dbg.c —— 见 dbg.h
#include "dbg.h"
#if DBG_ON
#include "esp_system.h"
#include "esp_attr.h"
#include <stdio.h>
#include <string.h>

LV_FONT_DECLARE(font_ps8);
#define MAGIC 0xDB6DB6u
static RTC_DATA_ATTR char     s_last[40];
static RTC_DATA_ATTR uint32_t s_magic;
static char      s_report[72];
static lv_obj_t *s_lbl;

void dbg_init(void)
{
    esp_reset_reason_t r = esp_reset_reason();
    bool ok = (r == ESP_RST_POWERON || r == ESP_RST_SW);
    bool bad = !ok;
    s_report[0] = 0;
    if (bad && s_magic == MAGIC && s_last[0])
        snprintf(s_report, sizeof s_report, "crash@%s rst=%d", s_last, (int)r);
    s_magic = MAGIC;
    s_last[0] = 0;
}

const char *dbg_report(void) { return s_report[0] ? s_report : NULL; }
int dbg_reset_reason(void) { return (int)esp_reset_reason(); }

void dbg_attach(lv_obj_t *scr)
{
    s_lbl = lv_label_create(scr);
    lv_label_set_text(s_lbl, "");
    lv_obj_set_style_text_font(s_lbl, &font_ps8, 0);
    lv_obj_set_style_text_color(s_lbl, lv_color_hex(0xE6B93C), 0);
    lv_obj_set_pos(s_lbl, 56, 9);
}

void dbg_step(const char *tag)
{
    strncpy(s_last, tag, sizeof s_last - 1);
    s_last[sizeof s_last - 1] = 0;
    if (!s_lbl) return;
    char b[64];
    snprintf(b, sizeof b, "%s %lu", tag, (unsigned long)esp_get_free_heap_size());
    lv_label_set_text(s_lbl, b);
}

void dbg_detach(void) { s_lbl = NULL; }
#endif
