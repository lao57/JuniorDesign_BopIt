#pragma once
#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int i2c_port;          // 0 or 1
    int gpio_sda;          // SDA pin
    int gpio_scl;          // SCL pin
    int freq_hz;           // e.g., 400000
    uint8_t addr;          // 7-bit I2C address (e.g., 0x27 or 0x3F)
    uint8_t cols;          // LCD columns (e.g., 16 or 20)
    uint8_t rows;          // LCD rows (e.g., 2 or 4)
    bool backlight_on;     // backlight initial state
} lcd_cfg_t;

/** Initialize I2C and the LCD with provided config. */
esp_err_t lcd_init(const lcd_cfg_t *cfg);

/** Initialize using Kconfig defaults (recommended for your project). */
esp_err_t lcd_init_default(void);

/** Clear display (home cursor). */
esp_err_t lcd_clear(void);

/** Turn backlight on/off (PCF8574 backpack). */
esp_err_t lcd_backlight(bool on);

/** Set cursor (0-based col, row). */
esp_err_t lcd_set_cursor(uint8_t col, uint8_t row);

/** Print a C string at current cursor. (No newline handling; stops at '\0'.) */
esp_err_t lcd_print(const char *str);

/** Convenience: clear + print entire string from (0,0). */
esp_err_t display_on_lcd(const char *str);

#ifdef __cplusplus
}
#endif
