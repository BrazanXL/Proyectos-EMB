#include "driver.h"
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "soc/gpio_struct.h"
#include "soc/gpio_reg.h"

#define STEP_PIN 21
#define DIR_PIN  19

#define PASOS_REV 200
#define MICROSTEP 16
#define NTOT (PASOS_REV * MICROSTEP)

#define PITCH 0.0018


static gptimer_handle_t step_timer = NULL;
static volatile bool stepState = false;
static volatile uint32_t stepDelay = 1000;

static float t = 0;
static const float Ts = 0.001;
static float freq = 0;
static float amp = 0;

static vibration_config_t vib = {0};

/*================ TIMER ISR ================*/

static bool IRAM_ATTR stepISR(gptimer_handle_t timer,
                              const gptimer_alarm_event_data_t *edata,
                              void *user_ctx)
{
    stepState = !stepState;

    if(stepState)
        GPIO.out_w1ts = (1 << STEP_PIN);
    else
        GPIO.out_w1tc = (1 << STEP_PIN);

    return true;
}

/*================ INIT ================*/

void vibration_init()
{
    gpio_set_direction(STEP_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(DIR_PIN, GPIO_MODE_OUTPUT);

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000
    };

    gptimer_new_timer(&timer_config, &step_timer);

    gptimer_event_callbacks_t cbs = {
        .on_alarm = stepISR
    };

    gptimer_register_event_callbacks(step_timer, &cbs, NULL);

    gptimer_enable(step_timer);

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = stepDelay,
        .flags.auto_reload_on_alarm = true
    };

    gptimer_set_alarm_action(step_timer, &alarm_config);

    gptimer_start(step_timer);
}

/*================ START ================*/

void vibration_start()
{
    gptimer_start(step_timer);
}

/*================ STOP ================*/

void vibration_stop()
{
    gptimer_stop(step_timer);
}

/*================ UPDATE ================*/

void vibration_update()

{
    if(!vib.running) return;

    float v =
        2 * M_PI * vib.frequency * vib.amplitude *
        cos(2 * M_PI * vib.frequency * t);

    float f_step = (NTOT / PITCH) * v;

    if(f_step < 0)
    {
        gpio_set_level(DIR_PIN, 0);
        f_step = -f_step;
    }
    else
    {
        gpio_set_level(DIR_PIN, 1);
    }

    if(f_step > 1)
    {
        uint32_t newDelay = 1000000 / (2 * f_step);

        if(newDelay != stepDelay)
        {
            stepDelay = newDelay;
        }
    }

    t += Ts;
}

void vibration_set_frequency(float f)
{
    freq = f;
}

void vibration_set_amplitude(float a)
{
    amp = a;
}