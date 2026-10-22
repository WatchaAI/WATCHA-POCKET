// main/boot_anim.c —— 开屏：终端风 ./watcha → 2^10 → 1024 → hello world
#include "lvgl.h"
#include <string.h>
#include "sfx.h"
#include "esp_random.h"

LV_FONT_DECLARE(font_ps16);

#define C_BG    0x0F0F1A
#define C_GREEN 0x5AB055
#define C_MUTED 0x6B6B7B
#define C_TEXT  0xF4F4EA

#define TICK_MS 50
#define CHAR_W  16
#define LINE_H  24
#define X0      8
#define Y0      16

typedef struct { const char *text; uint32_t color; int start; int per_char; } line_t;
static const line_t LINES[] = {
    {"$ ./watcha",     C_GREEN, 10, 2},
    {"> 2^10",         C_MUTED, 45, 2},
    {"= 10000000000",  C_MUTED, 70, 1},
    {"= 1024",         C_GREEN, 100, 2},
    {"$ hello world",  C_TEXT,  130, 2},
};
#define N (int)(sizeof(LINES)/sizeof(LINES[0]))
#define END_TICK 175

static lv_obj_t *s_scr, *s_cursor, *s_lbl[N];
static lv_timer_t *s_timer;
static int s_tick;
static void (*s_done)(void);
static char s_buf[N][24];
static volatile bool s_skip;
static int s_prev[N];

static void finish(void)
{
    if (s_timer) { lv_timer_delete(s_timer); s_timer = NULL; }
    void (*cb)(void) = s_done; s_done = NULL;
    if (cb) cb();
    if (s_scr) { lv_obj_delete(s_scr); s_scr = NULL; }
}

static void tick_cb(lv_timer_t *t)
{
    (void)t;
    if (s_skip) { finish(); return; }
    s_tick++;
    int cx = X0, cy = Y0;
    int kv = 0, khz = 0;   /* 本格要「嗒」的轻重和音高 */
    for (int i = 0; i < N; i++) {
        int n = (s_tick - LINES[i].start) / LINES[i].per_char;
        if (n < 0) continue;
        int len = strlen(LINES[i].text);
        if (n > len) n = len;
        if (n > s_prev[i]) {   /* 新出了字：不是空格就「嗒」 */
            const char *tx = LINES[i].text;
            if (tx[n - 1] != ' ') {
                uint32_t r = esp_random();
                /* 行首、行尾（像敲回车）、词首偏重，其余随机 1/4 重；轻的音量和音高也随机，像真人打字 */
                bool heavy = (n == 1) || (n == len) || (n >= 2 && tx[n - 2] == ' ') || (r % 4 == 0);
                kv  = heavy ? 215 + (int)((r >> 8) % 41) : 90 + (int)((r >> 8) % 70);
                khz = heavy ? 150 + (int)((r >> 16) % 50) : 230 + (int)((r >> 16) % 120);
            }
            s_prev[i] = n;
        }
        memcpy(s_buf[i], LINES[i].text, n); s_buf[i][n] = 0;
        lv_label_set_text(s_lbl[i], s_buf[i]);
        cx = X0 + n * CHAR_W; cy = Y0 + i * LINE_H;
    }
    lv_obj_set_pos(s_cursor, cx, cy);
    if (kv && sfx_status() == 3) sfx_key(kv, khz);   /* 音频没就绪就不排队，免得顶掉开机音 */
    if ((s_tick / 8) % 2 == 0) lv_obj_remove_flag(s_cursor, LV_OBJ_FLAG_HIDDEN);
    else                       lv_obj_add_flag(s_cursor, LV_OBJ_FLAG_HIDDEN);

    if (s_tick >= END_TICK) finish();
}

void boot_anim_skip(void) { s_skip = true; }

void boot_anim_start(void (*done)(void))
{
    s_done = done; s_tick = 0; s_skip = false;
    s_scr = lv_obj_create(NULL);
    lv_obj_remove_flag(s_scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(s_scr, lv_color_hex(C_BG), 0);
    lv_obj_set_style_pad_all(s_scr, 0, 0);
    lv_obj_set_style_border_width(s_scr, 0, 0);
    for (int i = 0; i < N; i++) {
        s_buf[i][0] = 0;
        s_prev[i] = 0;
        s_lbl[i] = lv_label_create(s_scr);
        lv_label_set_text(s_lbl[i], "");
        lv_obj_set_style_text_font(s_lbl[i], &font_ps16, 0);
        lv_obj_set_style_text_color(s_lbl[i], lv_color_hex(LINES[i].color), 0);
        lv_obj_set_pos(s_lbl[i], X0, Y0 + i * LINE_H);
    }
    s_cursor = lv_obj_create(s_scr);
    lv_obj_remove_flag(s_cursor, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(s_cursor, 14, 16);
    lv_obj_set_style_radius(s_cursor, 0, 0);
    lv_obj_set_style_border_width(s_cursor, 0, 0);
    lv_obj_set_style_bg_color(s_cursor, lv_color_hex(C_GREEN), 0);
    lv_obj_set_pos(s_cursor, X0, Y0);
    lv_screen_load(s_scr);
    s_timer = lv_timer_create(tick_cb, TICK_MS, NULL);
    sfx_play(SFX_BOOT);
}
