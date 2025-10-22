#include "i2c_lcd.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_rom_sys.h"   // ets_delay_us
#include <string.h>
#include <projdefs.h>
#include <portmacro.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "i2c_lcd";

// ======= PCF8574 bit mapping (common boards) =======
// P7 P6 P5 P4 P3   P2   P1  P0
// D7 D6 D5 D4 BL   EN   RW  RS
#define LCD_P_CF_BL   0x08
#define LCD_P_CF_EN   0x04
#define LCD_P_CF_RW   0x02
#define LCD_P_CF_RS   0x01

// HD44780 commands
#define LCD_CMD_CLEAR         0x01
#define LCD_CMD_HOME          0x02
#define LCD_CMD_ENTRYMODE     0x04
#define LCD_CMD_DISPLAYCTRL   0x08
#define LCD_CMD_FUNCSET       0x20
#define LCD_CMD_SET_DDRAM     0x80

// Flags
#define LCD_ENTRY_INC         0x02
#define LCD_ENTRY_NOSHIFT     0x00
#define LCD_DISPLAY_ON        0x04
#define LCD_CURSOR_OFF        0x00
#define LCD_BLINK_OFF         0x00
#define LCD_FUNC_4BIT         0x00
#define LCD_FUNC_2LINE        0x08
#define LCD_FUNC_5x8DOTS      0x00

// DDRAM line offsets (16x2 / 20x4 typical)
static uint8_t s_line_addr[] = {0x00, 0x40, 0x14, 0x54};

typedef struct {
    lcd_cfg_t cfg;
    bool i2c_installed;
    uint8_t ctrl_base; // backlight bit cached
} lcd_ctx_t;

static lcd_ctx_t s_lcd = {0};

// Forward decls
static esp_err_t lcd_i2c_write(uint8_t data);
static esp_err_t lcd_write4(uint8_t nibble, bool rs);
static esp_err_t lcd_send(uint8_t val, bool rs);
static void      lcd_pulse_enable(uint8_t data);

static esp_err_t lcd_i2c_write(uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t err = i2c_master_start(cmd);
    if (err == ESP_OK) err = i2c_master_write_byte(cmd, (s_lcd.cfg.addr << 1) | I2C_MASTER_WRITE, true);
    if (err == ESP_OK) err = i2c_master_write_byte(cmd, data, true);
    if (err == ESP_OK) err = i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(s_lcd.cfg.i2c_port, cmd, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(cmd);
    return ret;
}

static void lcd_pulse_enable(uint8_t data) {
    // EN high -> low pulse; HD44780 needs >= 450 ns
    lcd_i2c_write(data | LCD_P_CF_EN);
    ets_delay_us(1); // ~>1 us
    lcd_i2c_write((data & ~LCD_P_CF_EN));
    ets_delay_us(50); // allow command to settle
}

static esp_err_t lcd_write4(uint8_t nibble, bool rs) {
    uint8_t data = (nibble & 0xF0);
    // shift nibble into upper P7..P4; ensure backlight and RS/RW bits
    data |= s_lcd.ctrl_base;              // includes BL
    if (rs) data |= LCD_P_CF_RS;          // data vs command
    data &= ~LCD_P_CF_RW;                 // write

    // Pulse
    lcd_pulse_enable(data);
    return ESP_OK;
}

static esp_err_t lcd_send(uint8_t val, bool rs) {
    // high nibble, then low nibble
    ESP_RETURN_ON_ERROR(lcd_write4(val & 0xF0, rs), TAG, "write4 hi");
    ESP_RETURN_ON_ERROR(lcd_write4((val << 4) & 0xF0, rs), TAG, "write4 lo");
    return ESP_OK;
}

static esp_err_t lcd_command(uint8_t cmd) {
    return lcd_send(cmd, false);
}

static esp_err_t lcd_data(uint8_t data) {
    return lcd_send(data, true);
}

esp_err_t lcd_backlight(bool on) {
    if (on) s_lcd.ctrl_base |= LCD_P_CF_BL;
    else    s_lcd.ctrl_base &= ~LCD_P_CF_BL;
    // Touch bus to apply last state; a no-op byte (just BL state)
    return lcd_i2c_write(s_lcd.ctrl_base);
}

esp_err_t lcd_clear(void) {
    ESP_RETURN_ON_ERROR(lcd_command(LCD_CMD_CLEAR), TAG, "clear");
    vTaskDelay(pdMS_TO_TICKS(2)); // clear needs ~1.5–2 ms
    return ESP_OK;
}

esp_err_t lcd_set_cursor(uint8_t col, uint8_t row) {
    if (row >= s_lcd.cfg.rows) row = s_lcd.cfg.rows - 1;
    uint8_t addr = (col < s_lcd.cfg.cols ? col : 0) + s_line_addr[row];
    return lcd_command(LCD_CMD_SET_DDRAM | addr);
}

esp_err_t lcd_print(const char *str) {
    if (!str) return ESP_ERR_INVALID_ARG;
    while (*str) {
        ESP_RETURN_ON_ERROR(lcd_data((uint8_t)*str++), TAG, "data");
    }
    return ESP_OK;
}

static esp_err_t lcd_hw_init_sequence(void) {
    // Per HD44780 4-bit init sequence
    vTaskDelay(pdMS_TO_TICKS(50)); // power-on wait

    // Force 8-bit mode 3 times (sending only high nibble)
    for (int i = 0; i < 3; ++i) {
        lcd_write4(0x30, false);
        vTaskDelay(pdMS_TO_TICKS(i == 0 ? 5 : 1));
    }
    // Set 4-bit
    lcd_write4(0x20, false);
    vTaskDelay(pdMS_TO_TICKS(1));

    // Function set: 4-bit, 2-line, 5x8
    ESP_RETURN_ON_ERROR(lcd_command(LCD_CMD_FUNCSET | LCD_FUNC_4BIT | LCD_FUNC_2LINE | LCD_FUNC_5x8DOTS), TAG, "func");
    // Display off
    ESP_RETURN_ON_ERROR(lcd_command(LCD_CMD_DISPLAYCTRL), TAG, "disp off");
    // Clear
    ESP_RETURN_ON_ERROR(lcd_clear(), TAG, "clr");
    // Entry mode
    ESP_RETURN_ON_ERROR(lcd_command(LCD_CMD_ENTRYMODE | LCD_ENTRY_INC | LCD_ENTRY_NOSHIFT), TAG, "entry");
    // Display on, no cursor, no blink
    ESP_RETURN_ON_ERROR(lcd_command(LCD_CMD_DISPLAYCTRL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF), TAG, "disp on");

    return ESP_OK;
}

esp_err_t lcd_init(const lcd_cfg_t *cfg) {
    ESP_RETURN_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, TAG, "cfg null");

    s_lcd.cfg = *cfg;
    s_lcd.ctrl_base = cfg->backlight_on ? LCD_P_CF_BL : 0;

    // Install I2C if not already
    if (!s_lcd.i2c_installed) {
        i2c_config_t conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = cfg->gpio_sda,
            .scl_io_num = cfg->gpio_scl,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = cfg->freq_hz,
            .clk_flags = 0
        };
        ESP_RETURN_ON_ERROR(i2c_param_config(cfg->i2c_port, &conf), TAG, "param");
        ESP_RETURN_ON_ERROR(i2c_driver_install(cfg->i2c_port, conf.mode, 0, 0, 0), TAG, "driver");
        s_lcd.i2c_installed = true;
    }

    // Apply backlight bit immediately
    ESP_RETURN_ON_ERROR(lcd_i2c_write(s_lcd.ctrl_base), TAG, "bl set");

    // Run HD44780 init
    return lcd_hw_init_sequence();
}

/* Default values. NOT WORKING USE EXPLICIT CALL IN MAIN */
// esp_err_t lcd_init_default(void) {
//     lcd_cfg_t c = {
//         .i2c_port = CONFIG_I2C_LCD_I2C_PORT,
//         .gpio_sda = CONFIG_I2C_LCD_SDA_GPIO,
//         .gpio_scl = CONFIG_I2C_LCD_SCL_GPIO,
//         .freq_hz  = CONFIG_I2C_LCD_CLOCK_HZ,
//         .addr     = (uint8_t)CONFIG_I2C_LCD_ADDR_HEX,
//         .cols     = (uint8_t)CONFIG_I2C_LCD_COLS,
//         .rows     = (uint8_t)CONFIG_I2C_LCD_ROWS,
//         .backlight_on = true,
//     };
//     return lcd_init(&c);
// }

esp_err_t display_on_lcd(const char *str) {
    ESP_RETURN_ON_ERROR(lcd_clear(), TAG, "clr");
    ESP_RETURN_ON_ERROR(lcd_set_cursor(0, 0), TAG, "cur");
    return lcd_print(str);
}
