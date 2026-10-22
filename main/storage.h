// main/storage.h —— NVS 读写封装（namespace "watcha"）。注意：别在按键回调里频繁写，闪存有寿命。
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
void     storage_init(void);                          // main 里调一次
uint32_t store_get_u32(const char *key, uint32_t def);
bool     store_set_u32(const char *key, uint32_t val); // 值没变则不写
bool     store_get_str(const char *key, char *buf, size_t len);
bool     store_set_str(const char *key, const char *val);
