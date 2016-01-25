#ifndef _UI_EPG_API_H
#define _UI_EPG_API_H

/*!
  dynamic policy services number;better be a odd number
  */
#define DY_SVC_NUM (5)
/*!
  SVC number by the side of the center SVC
  */
#define DY_OFFSET  (DY_SVC_NUM - 1)/2

enum epg_msg
{
  MSG_EPG_READY = MSG_EXTERN_BEGIN + 50,
  MSG_STOP_EPG,
  MSG_PF_READY,
  MSG_EPG_DEL,
  MSG_TDT_FOUND,
};

#define EPG_DESC_MAX_NAME_LENGTH    256

void ui_epg_init(void);

void ui_epg_release(void);

void ui_epg_start(void);

void ui_epg_stop(void);

void ui_set_epg_dy(void);

void ui_set_epg_dy_by_offset(u16 offset);

void ui_set_epg_day_offset(u8 offset);

#endif

