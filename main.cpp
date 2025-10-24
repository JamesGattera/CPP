//main.cpp                          // Name The File
//#include <stdio.h>                // Stndard In-Out
//#include <stdbool.h>              // Optional Booleans
#include "esp_log.h"                // ESP_LOGI(TAG, "Starting...");
#include "esp_lcd_io_i2c.h"         //justin case
#include "esp_lcd_panel_commands.h" // SHOULD BE WHAT I NEED TO RUN BASICS
#include "esp_lcd_panel_ops.h"      // THIS ONE
#include "freertos/FreeRTOS.h"      // vTaskDelay(pdMS_TO_TICKS(1000));
#include "driver/i2c_master.h"      // Modern ESP-IDF 5+ I2C API
#include "driver/i2c_types.h"       // Expanded I2C :: for later 
#include "hal/lcd_types.h"          // Expanded LCDs :: for later
#include "esp_lcd_panel_ssd1306.h"  // Specific Formatting
#define TAG "MAIN"                  // Name The Process
#define BoardLed    GPIO_NUM_2      // STRAPPING Indicator
#define I2C_PORT    I2C_NUM_0       // Chosen ESP32 I2C Bus
#define SDA_PIN     GPIO_NUM_21     // SHARED Physical Pin
#define SCL_PIN     GPIO_NUM_22     // SHARED Physical Pin
#define I2C_FREQ_HZ 400*1000        // 400khz          
#define SCREEN_ADDR 0x3C            // Integral SSD1306 Screen
#define HOR_RES     128             // Horizontal-X
#define VER_RES     64              // Vertical-X
#define LCD_CMD_BITS 8              // ?? assume(cmd/param) are 8
// global handles
static i2c_master_bus_handle_t      i2c_bus = NULL;
static esp_lcd_panel_io_handle_t    io_handle = NULL;
static esp_lcd_panel_handle_t       panel_handle = NULL;
static uint8_t                      framebuffer[HOR_RES * (VER_RES / 8)] = {0}; //framebuffer defined
// Create I2C Bus
void InitI2C(void){
    ESP_LOGI(TAG, "InitController");
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_PORT,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT, // or I2C_CLK_SRC_APB
        .glitch_ignore_cnt = 7, // filter mistakes
        .flags = { .enable_internal_pullup = true},
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));
    ESP_LOGI(TAG, "I2C master bus created");
}

// Create IO Handle
void InitIO(void){
    ESP_LOGI(TAG, "Init SSD1306 IO Handle...");
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = SCREEN_ADDR,
        .control_phase_bytes = 1,       // Per LCD spec
        .dc_bit_offset = 6,             // Typically 6 for SSD1306, possibly 7 or 0
        .lcd_cmd_bits = LCD_CMD_BITS,   // 8
        .lcd_param_bits = LCD_CMD_BITS, // 8
        .scl_speed_hz = I2C_FREQ_HZ,    // 400khz    
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

    static esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = 64
    };
    // Create SSD1306 Panel
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,   // -1 external reset doesn't exist
        .bits_per_pixel = 1,    // 'Mono'
        .vendor_config = &ssd1306_config,};

    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    esp_lcd_panel_reset(panel_handle);//reset before init
    esp_lcd_panel_init(panel_handle); //init after reset
    esp_lcd_panel_disp_on_off(panel_handle, true);
    ESP_LOGI(TAG, "SSD1306 panel initialized.");
}
void InitController(){
    ESP_LOGI(TAG, "InitController");
    InitI2C();
    InitIO();
    esp_lcd_panel_mirror(panel_handle, 1, 1); // flip Y axis
}
////    Basic Display Functions
void display_clear(void){// full wipe
    ESP_LOGI(TAG, "display_clear");
    memset(framebuffer,0,sizeof(framebuffer));//clear local framebuffer
}
void drawPixel(int x, int y, bool on) {// Set or clear a single pixel
    if (x < 0 || x >= HOR_RES || y < 0 || y >= VER_RES) return;
    int byteIndex = x + (y / 8) * HOR_RES;  // SSD1306 vertical byte layout
    uint8_t bit = 1 << (y % 8);
    if (on) framebuffer[byteIndex] |= bit;
    else     framebuffer[byteIndex] &= ~bit;
}
void display_update(void) {// Push framebuffer to display
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, HOR_RES, VER_RES, framebuffer);
}
void DrawStripes(void){
    ESP_LOGI(TAG, "DrawStripes");
    for (int y = 0; y < VER_RES; y += 8)
    for (int x = 0; x < HOR_RES; x += 8)
    framebuffer[x + (y / 8) * HOR_RES] = 0xFF;// set byte
    esp_lcd_panel_draw_bitmap(panel_handle,0,0,HOR_RES,VER_RES,framebuffer);//screen push
}

//main-main
extern "C" void app_main() {
    ESP_LOGI(TAG, "Starting...");
    InitController();
    esp_lcd_panel_draw_bitmap(panel_handle,0,0,HOR_RES,VER_RES,framebuffer);//screen push
    display_clear();//after init
    //test plotting
    drawPixel(0, 0, 1);//middle
    drawPixel(127, 0, 1);//middle
    drawPixel(0, 63, 1);//middle
    drawPixel(127, 63, 1);//middle
    drawPixel(127/2, 63/2, 1);//middle
    //refresh screen
    display_update();
    //tail code
    ESP_LOGI(TAG, "Done.");
    vTaskDelete(NULL);
}