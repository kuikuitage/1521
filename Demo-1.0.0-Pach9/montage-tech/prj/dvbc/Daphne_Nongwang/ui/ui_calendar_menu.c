/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"

#include "calendar_def.h"
#include "ui_calendar_menu.h"
#include "time.h"
/*global variable for saving current date*/
static u16 iYear = 0;
static u8 iMonth = 0;
static u8 iDay = 0;

/*control id for create calendar menu*/
enum calendar_control_id
{
  IDC_INVALID = 0,
  IDC_TITLE_BG,
  IDC_SOLAR_1,
  IDC_SOLAR_2,
  IDC_SOLAR_3,
  IDC_SOLAR_4,
  IDC_LUNAR,
  IDC_ANIMAL,
  IDC_MBOX_WEEK,
  IDC_MBOX_DAY,
  //IDC_CAL_HOL,
  IDC_INFO_CONT,

  IDC_HELP_1,
  IDC_HELP_2,
  IDC_HELP_3,
};

enum info_control_id
{
  IDC_LDATE = 1,
  IDC_YI,
  IDC_LHOL,
  IDC_JI,
};
#if 0
rsc

static comm_help_data_t calendar_help_data[3] = //help bar data
{
	{
	  2,
	  2,
	  {
	    IDS_WEEK,
	    IDS_DATE,
	  },
	  { 
	    IM_ICON_ARROW_UP_DOWN,
	    IM_ICON_ARROW_LEFT_RIGHT,
	  },
	},
	
	{
	  1,
	  1,
	  {
	    IDS_MONTH,
	  },
	  { 
	    IM_ICON_ARROW_UP_DOWN,
	  },
	},
	
	{
	  1,
	  1,
	  {
	    IDS_YEAR,
	  },
	  { 
	    IM_ICON_ARROW_UP_DOWN,
	  },
	}	
};
#endif
u16 calendar_menu_cont_keymap(u16 key);
RET_CODE calendar_menu_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 calendar_menu_cbox_keymap(u16 key);
RET_CODE calendar_menu_cbox_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 calendar_menu_mbox_keymap(u16 key);
RET_CODE calendar_menu_mbox_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

void set_calendar_data(control_t *cont, u8 redraw)
{
  control_t *p_ctrl;
  u16 week_day;
  u8 days;
  u16 i;
  u16 iLYear;
  u8 iLMonth, iLDay, iLHolDay;
  u8 asc_str[4]; /* 1 - 31 */
  u8 day_pattern[30];
  u16 pbuffer1[128];
  u16 pbuffer2[128];
  u8 count1, count2;

  for (i = 0; i < 30; i++)
  {
    day_pattern[i] = 0;
  }
  count1 = 0;
  count2 = 0;

  week_day = get_weekday(iYear, iMonth, 1);         //某年某月1号是星期几
  days = (u8)get_monthdays(iYear, iMonth);              //这个月有几天
  iLHolDay = get_lunar_date(iYear, iMonth, iDay, &iLYear, &iLMonth, &iLDay);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_SOLAR_1); //set solar year
  cbox_num_set_curn(p_ctrl, iYear);
  if (redraw & YEAR_CHANGED)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }

  p_ctrl = ctrl_get_child_by_id(cont, IDC_SOLAR_3); //set solar month
  cbox_num_set_curn(p_ctrl, iMonth);
  if (redraw & MONTH_CHANGED)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }

  p_ctrl = ctrl_get_child_by_id(cont, IDC_LUNAR); //set lunar date

  pbuffer1[0] = 0;
  get_lunar_str(iYear, iMonth, iDay, pbuffer1, 128);
  text_set_content_by_unistr(p_ctrl, pbuffer1);
  if (redraw & (MONTH_CHANGED | YEAR_CHANGED | DAY_CHANGED))
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }

  p_ctrl = ctrl_get_child_by_id(cont, IDC_ANIMAL); //set animal
  pbuffer1[0] = 0;
  format_animal(iLYear, pbuffer1, 128);
  text_set_content_by_unistr(p_ctrl, pbuffer1);
  if (redraw & (YEAR_CHANGED | DAY_CHANGED))
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }

  p_ctrl = ctrl_get_child_by_id(cont, IDC_MBOX_DAY); //set day mbox

  if (redraw & (MONTH_CHANGED | FIRST_DRAW))
  {
    for (i = 0; i < 42; i++) //clear old mbox content
    {
      if ((i >= week_day) && (i <= week_day + days - 1))
      {
        pbuffer1[0] = 0;
        sprintf((char *)asc_str, "%d", (i - week_day + 1));
        str_asc2uni(asc_str, pbuffer1);
        iLHolDay =
          get_lunar_date(iYear, iMonth, (i - week_day + 1), &iLYear, &iLMonth,
                         &iLDay);
        pbuffer2[0] = 0;
        if (iLHolDay == 0)
        {
          if(iLDay != 1)
          {
            format_lunar_day(iLDay, pbuffer2, 128);
          }
          else
          {
            format_lunar_month(iLMonth, pbuffer2, 128);
          }
        }
        else
        {
          format_solar_term(iLHolDay, pbuffer2, 80);
        }
        uni_strcat(pbuffer1, pbuffer2, 128);
        mbox_set_content_by_unistr(p_ctrl, i, pbuffer1);
      }
      else
      {
        mbox_set_content_by_ascstr(p_ctrl, i, (u8 *)" ");
      }
    }
  }
  if (redraw & FIRST_DRAW)
  {
    mbox_set_focus(p_ctrl, (u16)(iDay + week_day - 1));
  }

  if (redraw & (DAY_CHANGED))
  {
    mbox_set_focus_ext(p_ctrl, (u16)(iDay + week_day - 1));
  }

  if (redraw & MONTH_CHANGED)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
  
  get_pattern(iYear, iMonth, iDay, day_pattern);
  pbuffer1[0] = 0;
  pbuffer2[0] = 0;
  format_almanac_info(day_pattern, 
    pbuffer1, &count1, 128,
    pbuffer2, &count2, 128);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_INFO_CONT);
  p_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_YI);  //yi
  text_set_content_by_unistr(p_ctrl, pbuffer1);

  if (redraw & (MONTH_CHANGED | YEAR_CHANGED | DAY_CHANGED))
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
  p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_JI); //ji
  text_set_content_by_unistr(p_ctrl, pbuffer2);
  if (redraw & (MONTH_CHANGED | YEAR_CHANGED | DAY_CHANGED))
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}

void calendar_change_page(control_t *p_ctrl, BOOL up_down)
{
  u16 old_week_day, new_week_day;
  u16 old_days, new_days;
  u16 old_focus, new_focus;
  u8 redraw = 0;


  old_focus = mbox_get_focus(p_ctrl);
  old_week_day = get_weekday(iYear, iMonth, 1);             //某年某月1号是星期几
  old_days = get_monthdays(iYear, iMonth);

  if (up_down)                                              //page down
  {
    if (iMonth == 12)
    {
      iYear += 1;
      iMonth = 1;
      redraw |= (MONTH_CHANGED | YEAR_CHANGED);
    }
    else
    {
      iMonth += 1;
      redraw |= MONTH_CHANGED;
    }
    new_week_day = get_weekday(iYear, iMonth, 1);
    new_days = get_monthdays(iYear, iMonth);

    new_focus = old_focus % 7;
    while (new_focus < new_week_day)
    {
      new_focus += 7;
    }

    iDay = new_focus - new_week_day + 1;
    redraw |= DAY_CHANGED;
  }
  else //page up
  {
    if (iMonth == 1)
    {
      iYear -= 1;
      iMonth = 12;
      redraw |= (MONTH_CHANGED | YEAR_CHANGED);
    }
    else
    {
      iMonth -= 1;
      redraw |= MONTH_CHANGED;
    }
    new_week_day = get_weekday(iYear, iMonth, 1);
    new_days = get_monthdays(iYear, iMonth);

    new_focus = old_focus % 7 + 35;
    while (new_focus > (new_week_day + new_days - 1))
    {
      new_focus -= 7;
    }

    iDay = new_focus - new_week_day + 1;
    redraw |= DAY_CHANGED;
  }

  set_calendar_data(p_ctrl->p_parent, redraw);
}

static s32 calendar_menu_cbox_msg(control_t *p_ctrl, u16 msg, u32 para1,
                                   u32 para2)
{
  control_t *p_help;
  s32 ret = SUCCESS;
  u16 days;
  u8 redraw = 0;
  days = get_monthdays(iYear, iMonth);

  switch (msg)
  {
    case MSG_FOCUS_RIGHT:
      ctrl_default_proc(p_ctrl, MSG_LOSTFOCUS, 0, 0);
      ctrl_paint_ctrl(p_ctrl, TRUE);
      if (p_ctrl->id == IDC_SOLAR_1)
      {
        p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_SOLAR_3);
        ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_ctrl, TRUE);
	 p_help = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_HELP_3);
	 ctrl_set_attr(p_help, OBJ_ATTR_HIDDEN);
	 p_help = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_HELP_2);
        ctrl_set_attr(p_help, OBJ_ATTR_ACTIVE);
	ctrl_paint_ctrl(p_help, TRUE);
      }
      else
      {
        p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_MBOX_DAY);
        ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_ctrl, TRUE);
        iDay = 1;
        redraw |= DAY_CHANGED;
        set_calendar_data(p_ctrl->p_parent, redraw);
	p_help = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_HELP_2);
	 ctrl_set_attr(p_help, OBJ_ATTR_HIDDEN);
	 p_help = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_HELP_1);
        ctrl_set_attr(p_help, OBJ_ATTR_ACTIVE);
	ctrl_paint_ctrl(p_help, TRUE);
      }
      break;
    case MSG_FOCUS_LEFT:
      ctrl_default_proc(p_ctrl, MSG_LOSTFOCUS, 0, 0);
      ctrl_paint_ctrl(p_ctrl, TRUE);
      if (p_ctrl->id == IDC_SOLAR_3)
      {
        p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_SOLAR_1);
        ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_ctrl, TRUE);
	p_help = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_HELP_2);
	 ctrl_set_attr(p_help, OBJ_ATTR_HIDDEN);
	 p_help = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_HELP_3);
        ctrl_set_attr(p_help, OBJ_ATTR_ACTIVE);
	ctrl_paint_ctrl(p_help, TRUE);
      }
      else
      {
        p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_MBOX_DAY);
        ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_ctrl, TRUE);
        iDay = (u8)days;
        redraw |= DAY_CHANGED;
        set_calendar_data(p_ctrl->p_parent, redraw);
	p_help = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_HELP_3);
	 ctrl_set_attr(p_help, OBJ_ATTR_HIDDEN);
	 p_help = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_HELP_1);
        ctrl_set_attr(p_help, OBJ_ATTR_ACTIVE);
	ctrl_paint_ctrl(p_help, TRUE);
      }
      break;
    case MSG_INCREASE:
    case MSG_DECREASE:
      cbox_class_proc(p_ctrl, msg, 0, 0);
      if (p_ctrl->id == IDC_SOLAR_3)
      {
        iMonth = (u8)cbox_num_get_curn(p_ctrl);
        redraw |= (MONTH_CHANGED | DAY_CHANGED);
        if ((1 == iMonth) && (MSG_INCREASE == msg) && (iYear < YEAR_MAX))
        {
          iYear++;
          redraw |= YEAR_CHANGED;
        }
        else if ((12 == iMonth) && (MSG_DECREASE == msg) && (iYear > YEAR_MIN))
        {
          iYear--;
          redraw |= YEAR_CHANGED;
        }
      }
      else
      {
        iYear = (u16)cbox_num_get_curn(p_ctrl);
        redraw |= (YEAR_CHANGED | MONTH_CHANGED | DAY_CHANGED);
      }
      set_calendar_data(p_ctrl->p_parent, redraw);
      break;
    default:
      ret = cbox_class_proc(p_ctrl, msg, para1, para2);
      break;
  }


  return ret;
}


static s32 calendar_change(control_t *p_ctrl, u16 msg, u32 para1,  u32 para2)
{
  u16 week_day, days;
  u16 focus;
  u8 redraw = 0;
  s32 ret = SUCCESS;
  control_t *p_help;
  focus = mbox_get_focus(p_ctrl);
  week_day = get_weekday(iYear, iMonth, 1);     //某年某月1号是星期几
  days = get_monthdays(iYear, iMonth);          //这个月有几天

  switch (msg)
  {
    case MSG_FOCUS_LEFT:
      if (focus == week_day)
      {
        ctrl_default_proc(p_ctrl, MSG_LOSTFOCUS, 0, 0); //calendar_change_page(p_ctrl, FALSE, FALSE);//上一页
        ctrl_paint_ctrl(p_ctrl, TRUE);
        p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_SOLAR_3);
	p_help = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_HELP_1);
	 ctrl_set_attr(p_help, OBJ_ATTR_HIDDEN);
	 p_help = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_HELP_2);
        ctrl_set_attr(p_help, OBJ_ATTR_ACTIVE);
        ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_ctrl, TRUE);
	ctrl_paint_ctrl(p_help, TRUE);
      }
      else
      {
        iDay -= 1;
        redraw |= DAY_CHANGED;
        set_calendar_data(p_ctrl->p_parent, redraw);
      }
      break;

    case MSG_FOCUS_RIGHT:
      if (focus == (week_day + days - 1))
      {
        ctrl_default_proc(p_ctrl, MSG_LOSTFOCUS, 0, 0); //calendar_change_page(p_ctrl, TRUE, FALSE);//下一页
        ctrl_paint_ctrl(p_ctrl, TRUE);
        p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_SOLAR_1);
	p_help = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_HELP_1);
	 ctrl_set_attr(p_help, OBJ_ATTR_HIDDEN);
	 p_help = ctrl_get_child_by_id(p_ctrl->p_parent,IDC_HELP_3);
        ctrl_set_attr(p_help, OBJ_ATTR_ACTIVE);
        ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_ctrl, TRUE);
	ctrl_paint_ctrl(p_help, TRUE);
      }
      else
      { 
        iDay += 1;
        redraw |= DAY_CHANGED;
        set_calendar_data(p_ctrl->p_parent, redraw);
        
      }
      break;

    case MSG_FOCUS_UP:
      if ((focus - 7) >= week_day) //上一行
      {
        iDay -= 7;
        redraw |= DAY_CHANGED;
        set_calendar_data(p_ctrl->p_parent, redraw);
      }
      else
      {
        calendar_change_page(p_ctrl, FALSE); //上一页
      }
      break;

    case MSG_FOCUS_DOWN:
      if ((focus + 7) < (week_day + days))
      {
        iDay += 7;
        redraw |= DAY_CHANGED;
        set_calendar_data(p_ctrl->p_parent, redraw);
      }
      else
      {
        calendar_change_page(p_ctrl, TRUE); //下一页
      }
      break;
    default:
      ret = mbox_class_proc(p_ctrl, msg, para1, para2);
  }
  return ret;
}

s32 open_calendar_menu(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_cont, *p_ctrl[6], *p_mweek, *p_mday, *p_title_bg,*p_help[3];
  control_t *p_info, *p_item[4];
  u16 x;
  u16 solar_width[6] = { SOLAR_1_CBOX_W, SOLAR_2_TEXT_W, SOLAR_3_CBOX_W,
                         SOLAR_4_TEXT_W, LUNAR_TXT_W,    ANIMAL_TXT_W };
  u16 unistr[4], i;
  char *week[] = { "日", "一", "二", "三", "四", "五", "六" };
  char *year[] = { "年" };
  char *month[] = { "月" };
  u8 redraw = 0;
  full_screen_title_t title_data = {IM_COMMON_BANNER_SETTLE,IDS_CALENDAR, RSI_TITLE_BG,0};
  utc_time_t g_time;
  time_get(&g_time, TRUE);

  iYear = g_time.year;//+ 2000;
  iMonth = g_time.month;
  iDay = g_time.day;

  calendar_init();

  p_cont = ui_comm_root_create_full_screen(ROOT_ID_CALENDAR,
                             RSI_SN_BG,
                             CALENDAR_MENU_CONT_X, CALENDAR_MENU_CONT_Y,
                             CALENDAR_MENU_CONT_W, CALENDAR_MENU_CONT_H,
                             &title_data);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, calendar_menu_cont_keymap);
  ctrl_set_proc(p_cont, calendar_menu_cont_proc);

  p_title_bg = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)IDC_TITLE_BG,
                             SOLAR_TITLE_BG_X , SOLAR_TITLE_BG_Y,
                             SOLAR_TITLE_BG_W, SOLAR_TITLE_BG_H,
                             p_cont, 0);
  ctrl_set_rstyle(p_title_bg, RSI_SECOND_TITLE, RSI_SECOND_TITLE, RSI_SECOND_TITLE);

  x = SOLAR_TXT_X;

  for (i = 0; i < 6; i++)
  {
    switch (i)
    {
      case 0:
      case 2:
        p_ctrl[i] =
          ctrl_create_ctrl((u8 *)CTRL_CBOX, (u8)(IDC_SOLAR_1 + i), x, SOLAR_TXT_Y,
                           solar_width[i], SOLAR_TXT_H, p_cont,
                           0);
        ctrl_set_rstyle(p_ctrl[i], RSI_SECOND_TITLE, RSI_DLG_BTN_HL, RSI_SECOND_TITLE);
        ctrl_set_keymap(p_ctrl[i], calendar_menu_cbox_keymap);
        ctrl_set_proc(p_ctrl[i], calendar_menu_cbox_proc);
        cbox_enable_cycle_mode(p_ctrl[i], TRUE);
        cbox_set_work_mode(p_ctrl[i], CBOX_WORKMODE_NUMBER);
        cbox_num_set_type(p_ctrl[i], CBOX_ITEM_NUMTYPE_DEC);
        cbox_set_align_style(p_ctrl[i], STL_CENTER | STL_VCENTER);
        cbox_set_font_style(p_ctrl[i], FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
        if (0 == i)
        {
          cbox_num_set_range(p_ctrl[i], YEAR_MIN, YEAR_MAX, YEAR_STEP,
                             YEAR_BLEN);
        }
        else if (2 == i)
        {
          cbox_num_set_range(p_ctrl[i], MONTH_MIN, MONTH_MAX, MONTH_STEP,
                             MONTH_BLEN);
        }
        break;
      case 1:
      case 3:
      case 4:
      case 5:
        p_ctrl[i] =
          ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_SOLAR_1 + i), x, SOLAR_TXT_Y,
                           solar_width[i], SOLAR_TXT_H, p_cont,
                           0);
        ctrl_set_rstyle(p_ctrl[i], RSI_TXT, RSI_TXT, RSI_TXT);
        text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
        text_set_font_style(p_ctrl[i], FSI_SECOND_TITLE, FSI_SECOND_TITLE, FSI_SECOND_TITLE);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_UNICODE);
        if (1 == i)
        {
          gb2312_to_unicode((u8 *)year[0], 2, unistr, 4);
          text_set_content_by_unistr(p_ctrl[i], unistr);
        }
        else if (3 == i)
        {
          gb2312_to_unicode((u8 *)month[0], 2, unistr, 4);
          text_set_content_by_unistr(p_ctrl[i], unistr);
        }
        break;
    }
    x += solar_width[i];
  }
  //matrix box for week, "Mon." "Tues." "Wed."....
  p_mweek = ctrl_create_ctrl((u8 *)CTRL_MBOX, IDC_MBOX_WEEK, WEEK_MBOX_X,
                             WEEK_MBOX_Y, WEEK_MBOX_W, WEEK_MBOX_H, p_cont, 0);
  ctrl_set_rstyle(p_mweek, RSI_MBOX_WEEK, RSI_MBOX_WEEK, RSI_MBOX_WEEK);
  mbox_enable_icon_mode(p_mweek, FALSE);
  mbox_set_count(p_mweek, CALENDAR_MBOX_WEEK_TOL,
                 CALENDAR_MBOX_WEEK_COL, CALENDAR_MBOX_WEEK_ROW);
  mbox_set_item_rect(p_mweek, 0, 0, WEEK_MBOX_W, WEEK_MBOX_H, 0, 0);
  mbox_set_item_rstyle(p_mweek, RSI_MBOX_WEEK_ITEM_HL, RSI_MBOX_WEEK_ITEM_SH, RSI_MBOX_WEEK_ITEM_SH);
  mbox_set_string_fstyle(p_mweek, FSI_MBOX_WEEK_TEXT,
                        FSI_MBOX_WEEK_TEXT, FSI_MBOX_WEEK_TEXT);
  mbox_set_string_offset(p_mweek, 0, 0);
  mbox_set_string_align_type(p_mweek, STL_CENTER | STL_VCENTER);
  mbox_set_content_strtype(p_mweek, MBOX_STRTYPE_UNICODE);
  for (i = 0; i < CALENDAR_MBOX_WEEK_COL; i++)
  {
    gb2312_to_unicode((u8 *)week[i], 2, unistr, 4);
    mbox_set_content_by_unistr(p_mweek, i, unistr);
  }
  //matrix box for date,"1" "2" "3"....."29"....
  p_mday = ctrl_create_ctrl((u8 *)CTRL_MBOX, IDC_MBOX_DAY, DAY_MBOX_X,
                            DAY_MBOX_Y, DAY_MBOX_W, DAY_MBOX_H, p_cont, 0);
  ctrl_set_keymap(p_mday, calendar_menu_mbox_keymap);
  ctrl_set_proc(p_mday, calendar_menu_mbox_proc);
  ctrl_set_rstyle(p_mday, RSI_MBOX_DAY, RSI_MBOX_DAY, RSI_MBOX_DAY);
  mbox_enable_icon_mode(p_mday, FALSE);
  mbox_set_count(p_mday, CALENDAR_MBOX_DAY_TOL,
                 CALENDAR_MBOX_DAY_COL, CALENDAR_MBOX_DAY_ROW);
  mbox_set_item_rect(p_mday, 10, 10, DAY_MBOX_W-10, DAY_MBOX_H-10, 4, 4);
  mbox_set_item_rstyle(p_mday, RSI_MBOX_DAY_ITEM_HL,
                      RSI_MBOX_DAY_ITEM_SH, RSI_MBOX_DAY_ITEM_SH);
  mbox_set_string_fstyle(p_mday, FSI_MBOX_DAY_TEXT_F,
                        FSI_MBOX_DAY_TEXT_N, FSI_MBOX_DAY_TEXT_N);
  mbox_set_string_offset(p_mday, 0, 0);
  mbox_set_string_align_type(p_mday, STL_CENTER | STL_VCENTER);
  mbox_set_content_strtype(p_mday, MBOX_STRTYPE_UNICODE);

  p_info = ctrl_create_ctrl((u8 *)CTRL_CONT, IDC_INFO_CONT, INFO_CONT_X,
                            INFO_CONT_Y, INFO_CONT_W, INFO_CONT_H, p_cont, 0);
  ctrl_set_rstyle(p_info, RSI_INFO_CONT_SH, RSI_INFO_CONT_HL, RSI_INFO_CONT_SH);
  for (i = 0; i < 4; i++)
  {
    if (i % 2)
    {
      p_item[i] = ctrl_create_ctrl((u8 *)CTRL_TEXT, (u8)(IDC_LDATE + i),
                                   INFO_YI_X,
                                   (u16)((i / 2) ? INFO_JI_Y : INFO_YI_Y),
                                   INFO_YI_W,
                                   (u16)((i / 2) ? INFO_LHOL_H : INFO_LUNAR_H), 
                                   p_info, 0);
      ctrl_set_rstyle(p_item[i], RSI_FULL_SCR_BG_MID, RSI_FULL_SCR_BG_MID, RSI_FULL_SCR_BG_MID);
      text_set_align_type(p_item[i], STL_LEFT | STL_VCENTER);
      text_set_font_style(p_item[i], FSI_TXT, FSI_TXT, FSI_TXT);
      text_set_content_type(p_item[i], TEXT_STRTYPE_UNICODE);
    }
    else
    {
      p_item[i] = ctrl_create_ctrl((u8 *)CTRL_BMAP, (u8)(IDC_LDATE + i),
                                   INFO_LUNAR_X,
                                   (u16)((i / 2) ? INFO_LHOL_Y : INFO_LUNAR_Y),
                                   INFO_LUNAR_W,
                                   (u16)((i / 2) ? INFO_LHOL_H : INFO_LUNAR_H), 
                                   p_info, 0);
      //ctrl_set_rstyle(p_item[i], RSI_TXT, RSI_TXT, RSI_TXT);
      //bmap_set_content_by_id(p_item[i], (i / 2) ? IM_JI : IM_YI);game_tetris_7
      bmap_set_content_by_id(p_item[i], (i / 2) ? IM_BAD:IM_GOOD);
    }
  }
  /*set the data of calendar menu*/
  redraw |= FIRST_DRAW;

  set_calendar_data(p_cont, redraw); //set calendar data, includes mbox data , info textfield data......

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  for(i = 0;i < 3;i++ )
  {
     p_help[i] = ctrl_create_ctrl((u8 *)CTRL_CONT,(u8)(IDC_HELP_1 + i), 0, 516-40, CALENDAR_MENU_CONT_W,80, p_cont, 0);
     ctrl_set_attr(p_help[i],OBJ_ATTR_HIDDEN);
     ui_comm_help_create(&calendar_help_data[i],p_help[i]);
  }
  ctrl_set_attr(p_help[0],OBJ_ATTR_ACTIVE);
  

  //gdi_set_enable(FALSE);

  ctrl_default_proc(p_mday, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
#endif
  return SUCCESS;
}


BEGIN_KEYMAP(calendar_menu_cont_keymap,ui_comm_root_keymap)
  ON_EVENT(V_KEY_MENU, MSG_TO_MAINMENU)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
END_KEYMAP(calendar_menu_cont_keymap,ui_comm_root_keymap)

BEGIN_MSGPROC(calendar_menu_cont_proc, ui_comm_root_proc)

END_MSGPROC(calendar_menu_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(calendar_menu_cbox_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_INCREASE)
  ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
  ON_EVENT( V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT( V_KEY_LEFT, MSG_FOCUS_LEFT)
END_KEYMAP(calendar_menu_cbox_keymap, NULL)


BEGIN_MSGPROC(calendar_menu_cbox_proc, cbox_class_proc)
 ON_COMMAND(MSG_FOCUS_RIGHT, calendar_menu_cbox_msg)
 ON_COMMAND( MSG_FOCUS_LEFT, calendar_menu_cbox_msg)
 ON_COMMAND( MSG_INCREASE, calendar_menu_cbox_msg)
 ON_COMMAND( MSG_DECREASE, calendar_menu_cbox_msg)
END_MSGPROC(calendar_menu_cbox_proc, cbox_class_proc)


BEGIN_KEYMAP(calendar_menu_mbox_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(calendar_menu_mbox_keymap, NULL)

BEGIN_MSGPROC(calendar_menu_mbox_proc, mbox_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, calendar_change)
  ON_COMMAND(MSG_FOCUS_DOWN, calendar_change)
  ON_COMMAND(MSG_FOCUS_LEFT, calendar_change)
  ON_COMMAND(MSG_FOCUS_RIGHT, calendar_change)
END_MSGPROC(calendar_menu_mbox_proc, mbox_class_proc)

