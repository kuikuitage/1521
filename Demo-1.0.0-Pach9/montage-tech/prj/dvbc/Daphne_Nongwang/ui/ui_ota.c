#include "ui_common.h"
#include "ui_ota.h"
#include "ui_ota_search.h"

#ifdef OTA_SUPPORT   
enum control_id
{
  IDC_INVALID = 0,
  IDC_OTA_SAT,
  IDC_OTA_TP,
  IDC_OTA_FREQ,
  IDC_OTA_SYM,
  IDC_OTA_POL,
  IDC_OTA_PID,
  IDC_OTA_START,
};

static u8 g_ota_sat_view_id;
static u8 g_ota_tp_view_id;

static u16 ota_cont_keymap(u16 key);
static RET_CODE ota_cont_proc(control_t *cont, u16 msg, u32 para1,
                         u32 para2);
static RET_CODE ota_text_proc(control_t *p_text, u16 msg, u32 para1,
                         u32 para2);
static RET_CODE ota_nbox_proc(control_t *p_nbox, u16 msg, u32 para1, 
						 u32 para2);
static RET_CODE ota_cbox_proc(control_t *p_cbox, u16 msg, u32 para1,
                             u32 para2);


static void ota_redraw_sat_info(BOOL is_paint)
{
  control_t *p_ctrl;

  if(is_paint)
  {
    p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_SAT);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_TP);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_FREQ);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_SYM);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_POL);
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}

static void ota_redraw_tp_info(BOOL is_paint)
{
  control_t *p_ctrl;

  if(is_paint)
  {
    p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_TP);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_FREQ);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_SYM);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_POL);
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
}

static void ota_set_transpond(control_t *p_cont)
{
  control_t *p_freq, *p_sym, *p_pol;
  control_t *p_sat, *p_tp;
  dvbs_tp_node_t tp_node;  
  u16 tp_pos;
  u16 tp_id;

  memset(&tp_node, 0, sizeof(dvbs_tp_node_t));  
  
  p_sat = ctrl_get_child_by_id(p_cont, IDC_OTA_SAT);
  p_tp = ctrl_get_child_by_id(p_cont, IDC_OTA_TP);
  p_freq = ctrl_get_child_by_id(p_cont, IDC_OTA_FREQ);
  p_sym = ctrl_get_child_by_id(p_cont, IDC_OTA_SYM);
  p_pol = ctrl_get_child_by_id(p_cont, IDC_OTA_POL);

  tp_pos = ui_comm_select_get_focus(p_tp);
  tp_id = db_dvbs_get_id_by_view_pos(g_ota_tp_view_id, tp_pos);
  db_dvbs_get_tp_by_id(tp_id, &tp_node);

  tp_node.freq = ui_comm_numedit_get_num(p_freq);
  tp_node.sym = ui_comm_numedit_get_num(p_sym);
  tp_node.polarity = ui_comm_select_get_focus(p_pol);

  ui_set_transpond(&tp_node);
  
}

static RET_CODE ota_fill_tp(control_t *ctrl, u16 focus, u16 *p_str,
                        u16 max_length)
{
  dvbs_tp_node_t tp_node;
  u8 asc_buf[32];
  u16 total;
  BOOL is_enable;  
  
  if (g_ota_tp_view_id == DB_DVBS_INVALID_VIEW)
  {
    total = 0;
  }
  else
  {
    total = db_dvbs_get_count(g_ota_tp_view_id);
  }

  if (total > 0)
  {
    ui_dbase_get_tp_by_pos(g_ota_tp_view_id, &tp_node, focus);
    sprintf((char *)asc_buf, "(%d/%d) %d %c %d", (int)(focus + 1), (int)(total),
            (int)(tp_node.freq), tp_node.polarity ? 'V' : 'H', (int)(tp_node.sym));
    is_enable = TRUE;
  }
  else
  {
    memset(&tp_node, 0, sizeof(dvbs_tp_node_t));
    tp_node.freq = 3000, tp_node.sym = 1000;
    strcpy(asc_buf, "NO_TP");
    is_enable = FALSE;
  }
  str_nasc2uni(asc_buf, p_str, max_length);

  return SUCCESS;
}

static RET_CODE ota_fill_sat(control_t *ctrl, u16 focus, u16 *p_str,
                        u16 max_length)
{
  u8 asc_buf[10];
  u16 total, len = 0;
  sat_node_t sat_node;
  BOOL is_enable;

  total = db_dvbs_get_count(g_ota_sat_view_id);
  if (total == 0)
  {
    memset(&sat_node, 0, sizeof(sat_node_t));
    sat_node.lnb_high = sat_node.lnb_low = 5150;
    str_nasc2uni("NO_SAT", sat_node.name, DB_DVBS_MAX_NAME_LENGTH);
    is_enable = FALSE;
  }
  else
  {
    sprintf((char *)asc_buf, "%d/%d ", (int)(focus + 1), (int)(total));
    ui_dbase_get_sat_by_pos(g_ota_sat_view_id, &sat_node, focus);
    is_enable = TRUE;
  }

  if (total > 0)
  {
    str_asc2uni(asc_buf, p_str);
    len = (u16)uni_strlen(p_str);
  }

  /* NAME */
  uni_strncpy(p_str + len, sat_node.name, max_length - len);

  return SUCCESS;
}

static void ota_update_sat_info(u16 focus)
{
  u16 total;
  sat_node_t sat_node;
  control_t *other;

  total = db_dvbs_get_count(g_ota_sat_view_id);
  if (total > 0)
  {
    ui_dbase_get_sat_by_pos(g_ota_sat_view_id, &sat_node, focus);

    /* TP INFO */
    g_ota_tp_view_id = ui_dbase_create_view(DB_DVBS_SAT_TP, sat_node.id, NULL);
    total = db_dvbs_get_count(g_ota_tp_view_id);

    other = ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_TP);
    ui_comm_select_set_param(other, TRUE,
                             CBOX_WORKMODE_DYNAMIC, total, 0, ota_fill_tp);
    ui_comm_select_set_focus(other, 0);
  }
}

static void ota_update_tp_info(u16 focus)
{
  u16 total;
  dvbs_tp_node_t tp_node;
  control_t *other;

  total = db_dvbs_get_count(g_ota_tp_view_id);
  if (total > 0)
  {
    ui_dbase_get_tp_by_pos(g_ota_tp_view_id, &tp_node, focus);

    /* FREQ */
    other= ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_FREQ);
    ui_comm_numedit_set_num(other, tp_node.freq);

    /* SYM */
    other= ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_SYM);
    ui_comm_numedit_set_num(other, tp_node.sym);  

    /* POL */
    other = ui_comm_root_get_ctrl(ROOT_ID_OTA, IDC_OTA_POL);
    ui_comm_select_set_focus(other, (u16)tp_node.polarity);  
  }
}

static void ota_set_antenna_by_sat(u16 focus)
{
  u16 total;
  sat_node_t sat_node;

  total = db_dvbs_get_count(g_ota_sat_view_id);
  if (total == 0)
  {
    memset(&sat_node, 0, sizeof(sat_node_t));
    sat_node.lnb_high = sat_node.lnb_low = 5150;
  }
  else
  {
    ui_dbase_get_sat_by_pos(g_ota_sat_view_id, &sat_node, focus);
  }

  // set antenna
  ui_set_antenna(&sat_node);
}

static void ota_set_transpond_by_tp(u16 focus)
{
  dvbs_tp_node_t tp_node;
  u16 total = 0;

  if (g_ota_tp_view_id != DB_DVBS_INVALID_VIEW)
  {
    total = db_dvbs_get_count(g_ota_tp_view_id);
  }

  if (total > 0)
  {
    ui_dbase_get_tp_by_pos(g_ota_tp_view_id, &tp_node, focus);
  }
  else
  {
    memset(&tp_node, 0, sizeof(dvbs_tp_node_t));
    tp_node.freq = 3000, tp_node.sym = 1000;
  }
  // set transpond
  ui_set_transpond(&tp_node);
}

RET_CODE open_ota(u32 para1, u32 para2)
{
#if 0
rsc

  control_t *p_cont, *p_ctrl[OTA_ITEM_CNT];
  u16 sat_total, tp_total, sat_focus, tp_focus;
  sat_node_t sat;
//  control_t *p_start;
  u8 i;

  u16 stxt [OTA_ITEM_CNT] = { IDS_SATELLITE, IDS_TRANSPONDER,
     IDS_FREQUECY, IDS_SYMBOL, IDS_POLARITY, IDS_DOWNLOAD_PID, IDS_START};
  u16 y;

  sat_focus = 0;
  tp_focus = 0;

  g_ota_sat_view_id = ui_dbase_create_view(DB_DVBS_SELECTED_SAT, 0, NULL);
  sat_total = db_dvbs_get_count(g_ota_sat_view_id);
  if (sat_total == 0)
  {
    return ERR_FAILURE;
  }

  db_dvbs_get_sat_by_id(db_dvbs_get_id_by_view_pos(g_ota_sat_view_id, sat_focus), 
    &sat);

  g_ota_tp_view_id = ui_dbase_create_view(DB_DVBS_SAT_TP, sat.id, NULL);
  tp_total = db_dvbs_get_count(g_ota_tp_view_id);  

  p_cont =
    ui_comm_root_create_with_signbar(ROOT_ID_OTA, RSI_OTA_FRM,
                          OTA_CONT_X, OTA_CONT_Y,
                          OTA_CONT_W, OTA_CONT_H,
                          IDS_UPGRADE_BY_SAT, RSI_TITLE_BG, FALSE);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ota_cont_keymap);
  ctrl_set_proc(p_cont, ota_cont_proc);

  y = OTA_ITEM_Y;
  for (i = 0; i < OTA_ITEM_CNT; i++)
  {
    switch (i)
    {
      case 0://satellite
      case 1://transponder
      case 4://polarity
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_OTA_SAT + i),
          OTA_ITEM_X, y, OTA_ITEM_LW, OTA_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], ota_cbox_proc);
        if(i == 0)
        {
          ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_DYNAMIC, 
            sat_total, 0, ota_fill_sat);
        }
        else if(i == 1)
        {
          ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_DYNAMIC, 
            tp_total, 0, ota_fill_tp);
        }
        else
        {
          ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC, 
            2, CBOX_ITEM_STRTYPE_STRID, NULL);
          ui_comm_select_set_content(p_ctrl[i], 0, IDS_HORI);
          ui_comm_select_set_content(p_ctrl[i], 1, IDS_VERT);
        }
        break;
        
      case 2://freq
      case 3://sym
      case 5://pid
        p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_OTA_SAT + i),
          OTA_ITEM_X, y, OTA_ITEM_LW, OTA_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], ota_nbox_proc);
        if(i == 2)
        {
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC, 
            OTA_FREQ_MIN, OTA_FREQ_MAX, OTA_FREQ_BIT, (OTA_FREQ_BIT - 1));
          ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_MHZ);
        }
        else if(i == 3)
        {
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC, 
            OTA_SYM_MIN, OTA_SYM_MAX, OTA_SYM_BIT, (OTA_FREQ_BIT - 1));
          ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_KSS);
        }
        else
        {
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC, 
            OTA_PID_MIN, OTA_PID_MAX, OTA_PID_BIT, (OTA_PID_BIT - 1));
        }
        break;
        
      case 6:
        p_ctrl[i] = ui_comm_static_create(p_cont, (u8)(IDC_OTA_SAT + i),
          OTA_ITEM_X, y, OTA_ITEM_LW, OTA_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], ota_text_proc);
        break;
      default:
        MT_ASSERT(0);
        break;
    }

    ctrl_set_related_id(p_ctrl[i],
                        0,                                     /* left */
                        (u8)((i - 1 +
                              OTA_ITEM_CNT) %
                             OTA_ITEM_CNT + 1),            /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % OTA_ITEM_CNT + 1)); /* down */

    y += OTA_ITEM_H + OTA_ITEM_V_GAP;
  }
  ui_comm_select_set_focus(p_ctrl[0], sat_focus);
  ui_comm_select_set_focus(p_ctrl[1], tp_focus);
  
  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);


  
  ui_ota_init();
#endif
  return SUCCESS;
}

static RET_CODE on_ota_exit_all(control_t *cont, u16 msg, u32 para1,
                             u32 para2)
{
  ui_ota_release();

  return ERR_NOFEATURE;
}

static RET_CODE on_ota_exit(control_t *cont, u16 msg, u32 para1,
                            u32 para2)
{
  ui_ota_release();

  return ERR_NOFEATURE;
}

static RET_CODE on_ota_text_select(control_t *p_text, u16 msg, u32 para1,
                             	   u32 para2)
{

  control_t *p_pid, *p_cont, *p_sym;
  control_t *p_sat, *p_freq, *p_pol, *p_tp;
	ota_info_t upgrade = {0};
  ss_public_t *ss_public;
  dvbs_tp_node_t tp;
  u16 sat_id, tp_id;
  sat_node_t sat;

	p_cont = ctrl_get_parent(ctrl_get_parent(p_text));
	p_pid = ctrl_get_child_by_id(p_cont, IDC_OTA_PID);
	p_sat = ctrl_get_child_by_id(p_cont, IDC_OTA_SAT);
	p_tp = ctrl_get_child_by_id(p_cont, IDC_OTA_TP);
	p_freq = ctrl_get_child_by_id(p_cont, IDC_OTA_FREQ);
	p_sym = ctrl_get_child_by_id(p_cont, IDC_OTA_SYM);
	p_pol = ctrl_get_child_by_id(p_cont, IDC_OTA_POL);

	sat_id = db_dvbs_get_id_by_view_pos(g_ota_sat_view_id, ui_comm_select_get_focus(p_sat));
	db_dvbs_get_sat_by_id(sat_id, &sat);

  tp_id = db_dvbs_get_id_by_view_pos(g_ota_tp_view_id, ui_comm_select_get_focus(p_tp));
	db_dvbs_get_tp_by_id(tp_id, &tp);

	upgrade.download_data_pid = (u16)ui_comm_numedit_get_num(p_pid);

	ss_public = ss_ctrl_get_public(class_get_handle_by_id(SC_CLASS_ID));      
	upgrade.orig_software_version = ss_public->otai.orig_software_version;

	tp.freq= ui_comm_numedit_get_num(p_freq);

	tp.polarity = (u8)ui_comm_select_get_focus(p_pol);

	tp.sym = ui_comm_numedit_get_num(p_sym);

  trans_diseqc_info(&(upgrade.locks.disepc_rcv), &sat);
  trans_sat_info(&(upgrade.locks.sat_rcv), &sat);
  trans_tp_info(&(upgrade.locks.tp_rcv), &tp);

  return manage_open_menu(ROOT_ID_OTA_SEARCH, 0, (u32)&upgrade);
}

static RET_CODE on_ota_nbox_unselect(control_t *p_nbox, u16 msg, u32 para1, 
u32 para2)
{
  RET_CODE ret = SUCCESS;
  u32 border;

  if (nbox_is_on_edit(p_nbox))
  {
    if (nbox_is_outrange(p_nbox, &border))
    {
      ret = nbox_class_proc(p_nbox, MSG_SELECT, para1, para2);
    }
    else
    {
      ret = ERR_NOFEATURE;
    }
  }
  else
  {
    ret = ERR_NOFEATURE;
  }

  return ret;
}

static RET_CODE on_ota_nbox_change(control_t *p_nbox, u16 msg, u32 para1, 
									 u32 para2)
{
	control_t *p_cont;
	
	p_cont = ctrl_get_parent(ctrl_get_parent(p_nbox));
  
  ota_set_transpond(p_cont);

	return SUCCESS;
}

static RET_CODE on_ota_nbox_outrange(control_t *p_nbox, u16 msg, u32 para1, 
									 u32 para2)
{
	ui_comm_cfmdlg_open(NULL, IDS_MSG_OUT_OF_RANGE, NULL, 0);

	return SUCCESS;
}

static RET_CODE on_ota_nbox_focus_left(control_t *p_nbox, u16 msg, u32 para1, 
									   u32 para2)
{
	RET_CODE ret = SUCCESS;
	u32 num, min, max;
	
	if (nbox_is_on_edit(p_nbox))
	{
		ret = nbox_class_proc(p_nbox, msg, para1, para2);
	}
	else
	{
		num = nbox_get_num(p_nbox);
		nbox_get_range(p_nbox, &min, &max);

		if(num == min)
		{
			num = max;
		}
		else
		{
			num--;
		}
		
		nbox_set_num_by_dec(p_nbox, num);
		ctrl_paint_ctrl(p_nbox, TRUE);
	}
	
	return ret;
}

static RET_CODE on_ota_nbox_focus_right(control_t *p_nbox, u16 msg, u32 para1, 
									    u32 para2)
{
	RET_CODE ret = SUCCESS;
	u32 num, min, max;
	
	if (nbox_is_on_edit(p_nbox))
	{
		ret = nbox_class_proc(p_nbox, msg, para1, para2);
	}
	else
	{
		num = nbox_get_num(p_nbox);
		nbox_get_range(p_nbox, &min, &max);

		if(num == max)
		{
		  num = min;
		}
		else
		{
		  num++;
		}
		
		nbox_set_num_by_dec(p_nbox, num);
		ctrl_paint_ctrl(p_nbox, TRUE);
	}
	
	return ret;
}


static RET_CODE on_ota_nbox_focus_up(control_t *p_nbox, u16 msg, u32 para1, 
									   u32 para2)
{
  BOOL is_out;
  u32 border;
  
  MT_ASSERT(p_nbox != NULL);
  if((p_nbox->priv_attr & NBOX_HL_STATUS_MASK))
  {
    is_out = nbox_is_outrange(p_nbox, &border);
    
    nbox_exit_edit(p_nbox);
    
    if(is_out)
    {
      return SUCCESS;
    }
  }

  return ERR_NOFEATURE;
}


static RET_CODE on_ota_nbox_focus_down(control_t *p_nbox, u16 msg, u32 para1, 
									   u32 para2)
{
  BOOL is_out;
  u32 border;
  
  MT_ASSERT(p_nbox != NULL);
  if((p_nbox->priv_attr & NBOX_HL_STATUS_MASK))
  {
    is_out = nbox_is_outrange(p_nbox, &border);
    
    nbox_exit_edit(p_nbox);
    
    if(is_out)
    {
      return SUCCESS;
    }
  }

  return ERR_NOFEATURE;
}


static RET_CODE on_ota_cbox_changed(control_t *p_cbox, u16 msg, u32 para1,
                             	   u32 para2)
{	
  RET_CODE ret = SUCCESS;
  u16 to = (u16)para2;
  BOOL is_onfocus = ctrl_is_onfocus(p_cbox);

	// before switch
  ret = cbox_class_proc(p_cbox, msg, para1, para2);  

  /* after switch */
  switch(ctrl_get_ctrl_id(ctrl_get_parent(p_cbox)))
  {
    case IDC_OTA_SAT:
      ota_set_antenna_by_sat(to);
      ota_update_sat_info(to);
      ota_redraw_sat_info(is_onfocus);
      break;
    case IDC_OTA_TP:
      ota_set_transpond_by_tp(to);
      ota_update_tp_info(to);
      ota_redraw_tp_info(is_onfocus);
      break;
    case IDC_OTA_POL:
      ota_set_transpond(ctrl_get_parent(ctrl_get_parent(p_cbox)));
      break;
    default:
      break;
  }

  return SUCCESS;
}

BEGIN_KEYMAP(ota_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ota_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ota_cont_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_EXIT_ALL, on_ota_exit_all)
	ON_COMMAND(MSG_EXIT, on_ota_exit)	
END_MSGPROC(ota_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(ota_text_proc, text_class_proc)
	ON_COMMAND(MSG_SELECT, on_ota_text_select)
END_MSGPROC(ota_text_proc, text_class_proc)

BEGIN_MSGPROC(ota_nbox_proc, nbox_class_proc)
	ON_COMMAND(MSG_UNSELECT, on_ota_nbox_unselect)
	ON_COMMAND(MSG_CHANGED, on_ota_nbox_change)
	ON_COMMAND(MSG_OUTRANGE, on_ota_nbox_outrange)
	ON_COMMAND(MSG_FOCUS_LEFT, on_ota_nbox_focus_left)
	ON_COMMAND(MSG_FOCUS_RIGHT, on_ota_nbox_focus_right)
	ON_COMMAND(MSG_FOCUS_UP, on_ota_nbox_focus_up)
	ON_COMMAND(MSG_FOCUS_DOWN, on_ota_nbox_focus_down)
END_MSGPROC(ota_nbox_proc, nbox_class_proc)
                             
BEGIN_MSGPROC(ota_cbox_proc, cbox_class_proc)
	ON_COMMAND(MSG_CHANGED, on_ota_cbox_changed)
END_MSGPROC(ota_cbox_proc, cbox_class_proc)

#endif

