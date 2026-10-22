// main/apps/app_ui.c —— 见 app_ui.h
#include "app_ui.h"

lv_obj_t *ui_label(lv_obj_t *parent, const char *text, const lv_font_t *font, uint32_t color)
{
    lv_obj_t *l = lv_label_create(parent);
    lv_label_set_text(l, text);
    lv_obj_set_style_text_font(l, font, 0);
    lv_obj_set_style_text_color(l, lv_color_hex(color), 0);
    return l;
}

lv_obj_t *ui_block(lv_obj_t *parent, int x, int y, int w, int h, uint32_t color)
{
    lv_obj_t *o = lv_obj_create(parent);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, w, h);
    lv_obj_set_style_radius(o, 0, 0);
    lv_obj_set_style_border_width(o, 0, 0);
    lv_obj_set_style_pad_all(o, 0, 0);
    lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    return o;
}

void ui_topbar(lv_obj_t *scr, const char *title)
{
    lv_obj_t *t = ui_label(scr, title, &font_fusion12, C_GREEN);
    lv_obj_set_pos(t, 18, 9);
    ui_block(scr, 0, 26, UI_W, 1, C_LINE);
}

void ui_footer(lv_obj_t *scr, const char *hint)
{
    ui_block(scr, 0, 292, UI_W, 1, C_LINE);
    lv_obj_t *h = ui_label(scr, hint, &font_fusion12, C_MUTED);
    lv_obj_align(h, LV_ALIGN_TOP_MID, 0, 298);
}
