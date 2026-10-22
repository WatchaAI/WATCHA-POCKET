#include "storage.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char *TAG = "storage";
static bool s_ok = false;

void storage_init(void)
{
    esp_err_t e = nvs_flash_init();   // 默认 "nvs" 分区（0x9000），与 cardid 分区无关
    if (e == ESP_ERR_NVS_NO_FREE_PAGES || e == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "nvs needs erase (%s), erasing nvs partition only", esp_err_to_name(e));
        nvs_flash_erase();
        e = nvs_flash_init();
    }
    s_ok = (e == ESP_OK);
    ESP_LOGI(TAG, "init %s", s_ok ? "ok" : esp_err_to_name(e));
}

uint32_t store_get_u32(const char *key, uint32_t def)
{
    if (!s_ok) return def;
    nvs_handle_t h;
    if (nvs_open("watcha", NVS_READONLY, &h) != ESP_OK) return def;
    uint32_t v = def;
    nvs_get_u32(h, key, &v);
    nvs_close(h);
    return v;
}

bool store_set_u32(const char *key, uint32_t val)
{
    if (!s_ok) return false;
    nvs_handle_t h;
    if (nvs_open("watcha", NVS_READWRITE, &h) != ESP_OK) return false;
    uint32_t old = 0;
    bool same = (nvs_get_u32(h, key, &old) == ESP_OK && old == val);
    esp_err_t e = ESP_OK;
    if (!same) { e = nvs_set_u32(h, key, val); if (e == ESP_OK) e = nvs_commit(h); }
    nvs_close(h);
    return e == ESP_OK;
}

bool store_get_str(const char *key, char *buf, size_t len)
{
    if (!s_ok || !buf || !len) return false;
    nvs_handle_t h;
    if (nvs_open("watcha", NVS_READONLY, &h) != ESP_OK) return false;
    size_t n = len;
    esp_err_t e = nvs_get_str(h, key, buf, &n);
    nvs_close(h);
    return e == ESP_OK;
}

bool store_set_str(const char *key, const char *val)
{
    if (!s_ok) return false;
    nvs_handle_t h;
    if (nvs_open("watcha", NVS_READWRITE, &h) != ESP_OK) return false;
    esp_err_t e = nvs_set_str(h, key, val);
    if (e == ESP_OK) e = nvs_commit(h);
    nvs_close(h);
    return e == ESP_OK;
}
