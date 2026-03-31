#include "ui.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <LovyanGFX.hpp>
#define MENU_MAX_ITEMS 3

#define TAG_UI "UI"

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7789 _panel;
  lgfx::Bus_SPI _bus;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus.config();
      cfg.spi_host = VSPI_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read  = 16000000;
      cfg.spi_3wire = true;
      cfg.use_lock = true;
      cfg.dma_channel = 1;
      cfg.pin_sclk = 18;
      cfg.pin_mosi = 23;
      cfg.pin_miso = 19;
      cfg.pin_dc   = 2;
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }

    {
      auto cfg = _panel.config();
      cfg.pin_cs = 5;
      cfg.pin_rst = 4;
      cfg.pin_busy = -1;
      cfg.panel_width = 240;
      cfg.panel_height = 320;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = false;
      cfg.invert = true;
      cfg.rgb_order = false;
      _panel.config(cfg);
    }

    setPanel(&_panel);
  }
};

LGFX tft;
// base tft object to be used in ui functions
/*

//==================== TIPOS ====================

typedef enum {
    UI_EVT_NONE = 0,
    UI_EVT_ENC_CW,
    UI_EVT_ENC_CCW,
    UI_EVT_ENC_BTN,
    UI_EVT_BTN_BACK,
    UI_EVT_BTN_START
} ui_event_t;

typedef enum {
    UI_SCREEN_SPLASH = 0,
    UI_SCREEN_HOME,
    UI_SCREEN_CONFIG,
    UI_SCREEN_RUNNING,
    UI_SCREEN_ERROR
} ui_screen_t;

//==================== CONTEXTO ====================

static ui_screen_t current_screen = UI_SCREEN_SPLASH;
static int menu_index = 0;
static bool ui_dirty = true;
static int last_menu_index = -1;

//==================== PROTOTIPOS ====================

static void draw_splash();
static void draw_home();
static void draw_config();
static void draw_running();
static void draw_error();
static void update_home_selector();

static void process_event(ui_event_t evt);

//==================== INIT ====================

void ui_init()
{
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Backlight
    gpio_set_direction(GPIO_NUM_15, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_15, 1);
}

//==================== UPDATE ====================

void ui_update()
{
    if(!ui_dirty && current_screen != UI_SCREEN_HOME)
        return;

    ui_dirty = false;

    switch(current_screen)
    {
        case UI_SCREEN_SPLASH:
            draw_splash();
            break;

        case UI_SCREEN_HOME:
            if(last_menu_index == -1)
            {
                draw_home();              // dibuja base una sola vez
                last_menu_index = menu_index;
            }
            else if(menu_index != last_menu_index)
            {
                update_home_selector();   // solo mueve cursor
                last_menu_index = menu_index;
            }
            break;

        case UI_SCREEN_CONFIG:
            draw_config();
            break;

        case UI_SCREEN_RUNNING:
            draw_running();
            break;

        case UI_SCREEN_ERROR:
            draw_error();
            break;
    }
}

//==================== EVENTOS ====================

void ui_send_event(int evt)
{
    process_event((ui_event_t)evt);
}

static void process_event(ui_event_t evt)
{
    switch(current_screen)
    {
        case UI_SCREEN_HOME:

            if(evt == UI_EVT_ENC_CW)
            {
                if(menu_index < MENU_MAX_ITEMS - 1)
                    menu_index++;
            }

            if(evt == UI_EVT_ENC_CCW)
            {
                if(menu_index > 0)
                    menu_index--;
            }

            if(evt == UI_EVT_ENC_BTN)
            {
                switch(menu_index)
                {
                    case 0:
                        current_screen = UI_SCREEN_CONFIG;
                        ui_dirty = true;   
                        last_menu_index = -1;
                        break;

                    case 1:
                        current_screen = UI_SCREEN_RUNNING;
                        ui_dirty = true;
                        last_menu_index = -1;
                        break;

                    case 2:
                        current_screen = UI_SCREEN_ERROR;
                        ui_dirty = true;
                        last_menu_index = -1;
                        break;
                }
            }

            if(evt == UI_EVT_BTN_START)
            {
                current_screen = UI_SCREEN_RUNNING;
            }

        break;

        case UI_SCREEN_CONFIG:

            if(evt == UI_EVT_BTN_BACK)
            {
                current_screen = UI_SCREEN_HOME;
            }

        break;

        case UI_SCREEN_RUNNING:

            if(evt == UI_EVT_BTN_START) // stop
            {
                current_screen = UI_SCREEN_HOME;
            }

        break;

        case UI_SCREEN_ERROR:

            if(evt == UI_EVT_BTN_BACK)
            {
                current_screen = UI_SCREEN_HOME;
            }

        break;

        default:
        break;
    }
}


//==================== DRAW FUNCTIONS ====================

static void draw_splash()
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.drawString("VIBRATION TABLE", 20, 40);
    tft.drawString("Booting...", 20, 80);
    
    
}

static void update_home_selector()
{
    
    tft.fillRect(20, 60, 15, 80, TFT_BLACK);

    int y = 60 + (menu_index * 30);

    tft.setTextColor(TFT_GREEN);
    tft.drawString(">", 20, y);
}
static void draw_home()
{
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE);
    tft.drawString("HOME", 20, 20);

    tft.drawString("Configuracion", 40, 60);
    tft.drawString("Ejecutar", 40, 90);
    tft.drawString("Errores", 40, 120);

    update_home_selector();   // dibuja cursor inicial
}

static void draw_config()
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_CYAN);
    tft.drawString("CONFIG", 20, 20);

    tft.drawString("> Frequency", 20, 60);
    tft.drawString("  Time", 20, 90);
}

static void draw_running()
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("RUNNING...", 20, 20);

    tft.drawString("Freq: 50 Hz", 20, 60);
    tft.drawString("Remaining: 08:32", 20, 90);
}

static void draw_error()
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED);
    tft.drawString("ERROR", 20, 20);

    tft.drawString("Code: E01", 20, 60);
}
*/

// Código de prueba para validar conexión tft
/*
void tft_test()
{
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Test colores
    tft.fillScreen(TFT_RED);
    vTaskDelay(pdMS_TO_TICKS(500));

    tft.fillScreen(TFT_GREEN);
    vTaskDelay(pdMS_TO_TICKS(500));

    tft.fillScreen(TFT_BLUE);
    vTaskDelay(pdMS_TO_TICKS(500));

    tft.fillScreen(TFT_BLACK);

    // Test texto
    tft.setTextColor(TFT_WHITE);
    tft.drawString("TEST TFT OK", 40, 40);

    tft.setTextColor(TFT_YELLOW);
    tft.drawString("SPI OK", 40, 80);

    tft.setTextColor(TFT_CYAN);
    tft.drawString("ST7789 RUNNING", 40, 120);

    vTaskDelay(pdMS_TO_TICKS(1500));

    // Test líneas
    tft.drawLine(0, 0, 240, 320, TFT_RED);
    tft.drawLine(240, 0, 0, 320, TFT_GREEN);

    vTaskDelay(pdMS_TO_TICKS(1000));

    // Test rectángulos
    tft.fillRect(60, 60, 120, 80, TFT_BLUE);
    tft.drawRect(60, 60, 120, 80, TFT_WHITE);

    vTaskDelay(pdMS_TO_TICKS(1000));
}
*/