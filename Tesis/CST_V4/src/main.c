#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_system.h"
#include "esp_log.h"
#include "ui.h"


#define TAG "VIB_TABLE"
#define UI_REFRESH_MS 40   // 25 FPS

/*==================== ENUMERACIONES ====================*/

typedef enum {
    SYS_BOOT = 0,
    SYS_SELFTEST,
    SYS_IDLE,
    SYS_CONFIG,
    SYS_RUNNING,
    SYS_ERROR,
    SYS_SHUTDOWN
} system_state_t;

/*==================== CONTEXTO GLOBAL ====================*/

typedef struct {
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
    .serial_connected = false
};

/*==================== PROTOTIPOS ====================*/

void task_ui(void *arg);
void task_control(void *arg);
void task_comm(void *arg);
void task_supervisor(void *arg);

/*==================== FUNCIONES BASE ====================*/

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

    if(ok){
        sys.state = SYS_IDLE;
    } else {
        sys.error_code = 1;
        sys.state = SYS_ERROR;
    }
}

/*==================== TASK: UI ====================*/

void task_ui(void *arg)
{
    ui_init();

    while(1)
    {
        ui_update();
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

/*==================== TASK: CONTROL ====================*/

void task_control(void *arg)
{
    while(1)
    {
        if(sys.state == SYS_RUNNING)
        {
            // Aquí controlas PWM / DAC / Driver
            // usando sys.freq_setpoint y sys.amplitude
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/*==================== TASK: COMUNICACION ====================*/

void task_comm(void *arg)
{
    while(1)
    {
        // aquí parseas comandos serial tipo:
        // SET:FREQ:50
        // START
        // STOP

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/*==================== TASK: SUPERVISOR ====================*/

void task_supervisor(void *arg)
{
    while(1)
    {
        switch(sys.state)
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

/*==================== APP MAIN ====================*/

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
