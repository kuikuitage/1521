
#ifndef _UI_BROWSE_API_H
#define _UI_BROWSE_API_H

enum browser_msg
{
  MSG_BROWSER_ENTER = MSG_EXTERN_BEGIN + 120,
  MSG_BROWSER_EXIT,
};

void ui_browser_init(void);
void ui_browser_release(void);

void ui_browser_start(void);
void ui_browser_stop(void);

void ui_browser_enter(void);
void ui_browser_exit(void);

#endif

