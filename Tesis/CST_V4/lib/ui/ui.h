#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C" {
#endif

void ui_init();
void ui_update();
void ui_send_event(int evt);

#ifdef __cplusplus
}
#endif

#endif