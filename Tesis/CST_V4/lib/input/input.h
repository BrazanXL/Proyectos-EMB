#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    EVT_NONE = 0,
    EVT_ENC_LEFT,
    EVT_ENC_RIGHT,
    EVT_ENC_CLICK,
    EVT_BTN_BACK,
    EVT_BTN_START
} input_event_t;

void input_init(void);
void input_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif
