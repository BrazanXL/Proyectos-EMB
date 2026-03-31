#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_system.h"
#include "esp_log.h"
#include "ui.h"
#include "driver.h"

// Código base para firmware de control de vibración con ESP32
/*
#define TAG "VIB_TABLE"
#define UI_REFRESH_MS 40 // 25 FPS
#define UART_PORT UART_NUM_0
#define BUF_SIZE 256

//==================== ENUMERACIONES ====================

typedef enum
{
    SYS_BOOT = 0,
    SYS_SELFTEST,
    SYS_IDLE,
    SYS_CONFIG,
    SYS_RUNNING,
    SYS_ERROR,
    SYS_SHUTDOWN
} system_state_t;

//==================== CONTEXTO GLOBAL ====================

typedef struct
{
    system_state_t state;
    uint8_t error_code;
    float freq_setpoint;
    float amplitude;
    uint32_t run_time_ms;
    bool serial_connected;
} system_ctx_t;

static system_ctx_t sys = {
    .state = SYS_BOOT,
    .error_code = 0,
    .freq_setpoint = 0,
    .amplitude = 0,
    .run_time_ms = 0,
    .serial_connected = false};

//==================== PROTOTIPOS ====================

void task_ui(void *arg);
void task_control(void *arg);
void task_comm(void *arg);
void task_supervisor(void *arg);

//==================== FUNCIONES BASE ====================

void system_boot_sequence()
{
    ESP_LOGI(TAG, "Booting system...");

    // Aquí luego iría animación TFT tipo splash
    vTaskDelay(pdMS_TO_TICKS(1500));

    sys.state = SYS_SELFTEST;
}

void system_selftest()
{
    ESP_LOGI(TAG, "Running self-test...");

    // Aquí puedes validar:
    // - pantalla
    // - driver vibración
    // - sensores
    // - memoria

    bool ok = true;

    if (ok)
    {
        sys.state = SYS_IDLE;
    }
    else
    {
        sys.error_code = 1;
        sys.state = SYS_ERROR;
    }
}

//==================== TASK: UI ====================

void task_ui(void *arg)
{
    ui_init();

    while (1)
    {
        ui_update();
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

//==================== TASK: CONTROL ====================

void task_control(void *arg)
{
    vibration_init();

    while (1)
    {
        if (sys.state == SYS_RUNNING)
        {
            vibration_set_frequency(sys.freq_setpoint);
            vibration_set_amplitude(sys.amplitude);

            vibration_update();
        }
        else
        {
            vibration_stop();
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

//==================== TASK: COMUNICACION ====================

void task_comm(void *arg)
{
    uint8_t data[BUF_SIZE];

    while (1)
    {
        int len = uart_read_bytes(UART_PORT, data, BUF_SIZE - 1, pdMS_TO_TICKS(100));

        if (len > 0)
        {
            data[len] = 0;

            ESP_LOGI("SERIAL", "CMD: %s", data);

            if (strstr((char *)data, "START"))
            {
                sys.state = SYS_RUNNING;
            }

            else if (strstr((char *)data, "STOP"))
            {
                sys.state = SYS_IDLE;
            }

            else if (strstr((char *)data, "SET:FREQ:"))
            {
                float f = atof((char *)data + 9);
                sys.freq_setpoint = f;

                ESP_LOGI("SERIAL", "Freq set: %.2f", f);
            }

            else if (strstr((char *)data, "SET:AMP:"))
            {
                float a = atof((char *)data + 8);
                sys.amplitude = a;

                ESP_LOGI("SERIAL", "Amp set: %.3f", a);
            }

            else if (strstr((char *)data, "STATUS"))
            {
                char msg[100];

                sprintf(msg,
                        "STATE:%d FREQ:%.2f AMP:%.3f\n",
                        sys.state,
                        sys.freq_setpoint,
                        sys.amplitude);

                uart_write_bytes(UART_PORT, msg, strlen(msg));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

//==================== TASK: SUPERVISOR ====================

void task_supervisor(void *arg)
{
    while (1)
    {
        switch (sys.state)
        {
        case SYS_BOOT:
            system_boot_sequence();
            break;

        case SYS_SELFTEST:
            system_selftest();
            break;

        case SYS_ERROR:
            // lógica de paro seguro
            break;

        default:
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

//==================== APP MAIN ====================

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing firmware base...");

    // Crear tareas
    xTaskCreate(task_ui, "task_ui", 4096, NULL, 2, NULL);
    xTaskCreate(task_control, "task_control", 4096, NULL, 3, NULL);
    xTaskCreate(task_comm, "task_comm", 4096, NULL, 2, NULL);
    xTaskCreate(task_supervisor, "task_supervisor", 4096, NULL, 4, NULL);

    ESP_LOGI(TAG, "System started.");
}
*/

// Código de prueba para validar conexión con driver de vibración
/*
#define PIN_STEP 25   // PUL+
#define PIN_DIR  26   // DIR+
#define PIN_EN   27   // ENA+

static void motor_gpio_init()
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << PIN_STEP) | (1ULL << PIN_DIR) | (1ULL << PIN_EN),
    };
    gpio_config(&io_conf);

    gpio_set_level(PIN_EN, 0); // habilitar (en muchos drivers es LOW)
}

static void step_pulse(int delay_us)
{
    gpio_set_level(PIN_STEP, 1);
    esp_rom_delay_us(delay_us);

    gpio_set_level(PIN_STEP, 0);
    esp_rom_delay_us(delay_us);
}

static void motor_test_task(void *arg)
{
    while(1)
    {
        gpio_set_level(PIN_DIR, 1);

        for(int i = 0; i < 2000; i++)
        {
            step_pulse(250); // velocidad media
        }
        gpio_set_level(PIN_DIR, 0);

        for(int i = 0; i < 2000; i++)
        {
            step_pulse(350);
        }
    }
}

void app_main(void)
{
    motor_gpio_init();
    xTaskCreate(motor_test_task, "motor_test", 2048, NULL, 5, NULL);
}
    
*/

// Código de prueba para validar leds
/*
#define LED_PIN1 22
#define LED_PIN2 21
static void led_init()
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << LED_PIN1) | (1ULL << LED_PIN2),
    };
    gpio_config(&io_conf);
}

typedef enum {
    LED_IDLE,
    LED_RUNNING,
    LED_ERROR,
    LED_CONFIG
} led_state_t;

static volatile led_state_t led_state = LED_IDLE;

static void led_task(void *arg)
{
    while(1)
    {
        switch(led_state)
        {
            case LED_IDLE:
                // Parpadeo lento (1 Hz)
                gpio_set_level(LED_PIN1, 1);
                vTaskDelay(pdMS_TO_TICKS(500));
                gpio_set_level(LED_PIN1, 0);
                vTaskDelay(pdMS_TO_TICKS(500));
            break;

            case LED_RUNNING:
                // Encendido fijo
                gpio_set_level(LED_PIN2, 1);
                vTaskDelay(pdMS_TO_TICKS(100));
            break;

            case LED_ERROR:
                // Parpadeo rápido
                gpio_set_level(LED_PIN1, 1);
                vTaskDelay(pdMS_TO_TICKS(100));
                gpio_set_level(LED_PIN1, 0);
                vTaskDelay(pdMS_TO_TICKS(100));
            break;

            case LED_CONFIG:
                // Doble blink
                for(int i = 0; i < 2; i++)
                {
                    gpio_set_level(LED_PIN2, 1);
                    gpio_set_level(LED_PIN1, 1);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    gpio_set_level(LED_PIN2, 0);
                    gpio_set_level(LED_PIN1, 0);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
                vTaskDelay(pdMS_TO_TICKS(600));
            break;
        }
    }
}

void led_set_state(led_state_t state)
{
    led_state = state;
}

void app_main(void)
{
    led_init();

    xTaskCreate(led_task, "led_task", 1024, NULL, 1, NULL);

    // TEST automático de patrones
    while(1)
    {
        led_set_state(LED_IDLE);
        vTaskDelay(pdMS_TO_TICKS(3000));

        led_set_state(LED_RUNNING);
        vTaskDelay(pdMS_TO_TICKS(3000));

        led_set_state(LED_ERROR);
        vTaskDelay(pdMS_TO_TICKS(3000));

        led_set_state(LED_CONFIG);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
*/

// Código de prueba para validar conexión tft
/*
void app_main(void)
{
    // Crear tareas
    //xTaskCreate(task_ui, "task_ui", 4096, NULL, 2, NULL);
    tft_test();
}
*/