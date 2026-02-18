#include "input.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ui.h"

// ====== PINOUT ======
#define PIN_ENC_A      34
#define PIN_ENC_B      35
#define PIN_ENC_BTN    32
#define PIN_BTN_BACK   25
#define PIN_BTN_START  26

// ====== DEBOUNCE ======
#define DEBOUNCE_MS 30

// ====== ENCODER STATE MACHINE (Gray code) ======
static const int8_t enc_table[16] = {
    0, -1, +1, 0,
    +1, 0, 0, -1,
    -1, 0, 0, +1,
    0, +1, -1, 0
};

static uint8_t enc_state = 0;
static int32_t enc_accum = 0;

// ====== BOTONES ======
typedef struct {
    gpio_num_t pin;
    bool last;
    uint32_t last_ms;
} btn_t;

static btn_t btn_back  = { (gpio_num_t)PIN_BTN_BACK, 1, 0 };
static btn_t btn_start = { (gpio_num_t)PIN_BTN_START, 1, 0 };
static btn_t btn_enc   = { (gpio_num_t)PIN_ENC_BTN, 1, 0 };

// ====== UTIL ======
static inline uint32_t millis()
{
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

// ====== INIT ======
void input_init(void)
{
    // Encoder A/B (solo entrada, sin pullups internos en 34/35)
    gpio_config_t io = {};
    io.mode = GPIO_MODE_INPUT;
    io.pin_bit_mask = (1ULL << PIN_ENC_A) | (1ULL << PIN_ENC_B);
    io.pull_up_en = GPIO_PULLUP_DISABLE;
    io.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io);

    // Botones con pull-up interno
    gpio_config_t btn = {};
    btn.mode = GPIO_MODE_INPUT;
    btn.pin_bit_mask =
        (1ULL << PIN_ENC_BTN) |
        (1ULL << PIN_BTN_BACK) |
        (1ULL << PIN_BTN_START);
    btn.pull_up_en = GPIO_PULLUP_ENABLE;
    btn.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&btn);
}

// ====== LECTURA ENCODER ======
static void read_encoder()
{
    uint8_t a = gpio_get_level((gpio_num_t)PIN_ENC_A);
    uint8_t b = gpio_get_level((gpio_num_t)PIN_ENC_B);

    enc_state = ((enc_state << 2) | (a << 1) | b) & 0x0F;

    int8_t delta = enc_table[enc_state];
    if (delta)
    {
        enc_accum += delta;

        // cada 4 pasos = 1 detent (encoder mecánico)
        if (enc_accum >= 4)
        {
            ui_send_event(EVT_ENC_RIGHT);
            enc_accum = 0;
        }
        else if (enc_accum <= -4)
        {
            ui_send_event(EVT_ENC_LEFT);
            enc_accum = 0;
        }
    }
}

// ====== LECTURA BOTONES ======
static void read_button(btn_t *b, input_event_t evt)
{
    bool level = gpio_get_level(b->pin);
    uint32_t now = millis();

    if (level != b->last)
    {
        if ((now - b->last_ms) > DEBOUNCE_MS)
        {
            b->last = level;
            b->last_ms = now;

            if (level == 0) // activo en bajo
            {
                ui_send_event(evt);
            }
        }
    }
}

// ====== TASK ======
void input_task(void *arg)
{
    input_init();

    while (1)
    {
        read_encoder();

        read_button(&btn_enc,   EVT_ENC_CLICK);
        read_button(&btn_back,  EVT_BTN_BACK);
        read_button(&btn_start, EVT_BTN_START);

        vTaskDelay(pdMS_TO_TICKS(5)); // 200 Hz
    }
}
