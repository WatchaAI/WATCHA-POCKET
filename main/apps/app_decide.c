// main/apps/app_decide.c —— 决策猹：选不出来？交给猹。
// ▲▼ 换题目，确认开转，选项像老虎机一样越转越慢，停下来就是答案。
#include "app_ui.h"
#include "sfx.h"
#include "cha_pool.h"
#include "esp_random.h"

#define MAX_OPTS 8
typedef struct {
    const char *title;
    const char *opts[MAX_OPTS];
    int n;
} scene_t;

// ---- 题目和选项：改这里就能换成你自己的 ----
static const scene_t SCENES[] = {
    { "午饭吃什么",   { "麻辣烫", "黄焖鸡", "沙拉", "拉面", "饺子", "螺蛳粉", "盖浇饭", "先不吃" }, 8 },
    { "现在干点啥",   { "摸鱼", "写代码", "喝口水", "逛猹馆", "写猹评", "起来走走", "回消息", "下班" }, 8 },
    { "先修哪个 Bug", { "登录页", "首页白屏", "祖传代码", "线上报警", "让 AI 修", "明天再说" }, 6 },
};
#define N_SCENES ((int)(sizeof SCENES / sizeof SCENES[0]))

enum { ST_IDLE, ST_SPIN, ST_RESULT };

static lv_obj_t *s_scr, *s_scene, *s_box, *s_opt, *s_tip;
static lv_timer_t *s_timer;
static int s_scene_i, s_state, s_cur, s_steps_left;
static uint32_t s_period;

static void box_style(uint32_t border, int width, uint32_t text)
{
    lv_obj_set_style_border_color(s_box, lv_color_hex(border), 0);
    lv_obj_set_style_border_width(s_box, width, 0);
    lv_obj_set_style_text_color(s_opt, lv_color_hex(text), 0);
}

static void show_scene(void)
{
    s_state = ST_IDLE;
    lv_label_set_text(s_scene, SCENES[s_scene_i].title);
    lv_label_set_text(s_opt, "?");
    lv_label_set_text(s_tip, "按确认，让猹替你选");
    box_style(C_LINE, 1, C_MUTED);
}

static void spin_tick(lv_timer_t *t)
{
    (void)t;
    const scene_t *sc = &SCENES[s_scene_i];
    s_cur = (s_cur + 1 + (int)(esp_random() % (uint32_t)(sc->n - 1))) % sc->n;   // 每次换一个不同的
    lv_label_set_text(s_opt, sc->opts[s_cur]);
    sfx_play(SFX_NAV);
    if (--s_steps_left <= 0) {
        lv_timer_pause(s_timer);
        s_state = ST_RESULT;
        box_style(C_GOLD, 2, C_GOLD);
        lv_label_set_text(s_tip, "就它了！");
        sfx_play(SFX_FANFARE);
        return;
    }
    s_period = s_period * 115 / 100;          // 越转越慢
    lv_timer_set_period(s_timer, s_period);
}

static void start_spin(void)
{
    s_state = ST_SPIN;
    s_steps_left = 14 + (int)(esp_random() % 6);
    s_period = 50;
    box_style(C_LINE, 1, C_TEXT);
    lv_label_set_text(s_tip, "转呀转...");
    lv_timer_set_period(s_timer, s_period);
    lv_timer_resume(s_timer);
}

static void app_enter(void)
{
    s_scr = page_screen_create();
    ui_topbar(s_scr, "决策猹");

    lv_obj_t *av = lv_image_create(s_scr);
    lv_image_set_src(av, cha_random_72());
    lv_obj_set_pos(av, 84, 34);

    s_scene = ui_label(s_scr, "", &font_fusion12, C_MUTED);
    lv_obj_align(s_scene, LV_ALIGN_TOP_MID, 0, 110);

    s_box = ui_block(s_scr, 30, 140, 180, 60, C_CARD);
    s_opt = ui_label(s_box, "?", &font_fusion12, C_MUTED);
    lv_obj_center(s_opt);

    s_tip = ui_label(s_scr, "", &font_fusion12, C_GREEN);
    lv_obj_align(s_tip, LV_ALIGN_TOP_MID, 0, 222);

    ui_footer(s_scr, "▲▼ 换题 · 确认开转 · 长按返回");

    s_timer = lv_timer_create(spin_tick, 50, NULL);
    lv_timer_pause(s_timer);
    show_scene();
    lv_screen_load(s_scr);
}

static void app_exit(void)
{
    if (s_timer) { lv_timer_delete(s_timer); s_timer = NULL; }
    if (s_scr)   { lv_obj_delete(s_scr);     s_scr = NULL; }
}

static void app_key(bsp_btn_t btn, bsp_btn_ev_t ev)
{
    if (ev != BSP_BTN_CLICK || s_state == ST_SPIN) return;
    if (btn == BSP_BTN_UP)   { s_scene_i = (s_scene_i + N_SCENES - 1) % N_SCENES; sfx_play(SFX_NAV); show_scene(); }
    if (btn == BSP_BTN_DOWN) { s_scene_i = (s_scene_i + 1) % N_SCENES;            sfx_play(SFX_NAV); show_scene(); }
    if (btn == BSP_BTN_OK)   { start_spin(); }
}

const page_t app_decide = { "app_decide", app_enter, app_exit, app_key };
