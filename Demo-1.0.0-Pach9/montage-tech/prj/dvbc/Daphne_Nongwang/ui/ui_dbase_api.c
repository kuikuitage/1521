/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "data_base16v2.h"
#ifdef LCN_SWITCH
#include "ui_logic_num_proc.h"
#endif

static char * g_ca_system_desc[] = 
{
  "",
  "Standardized systems",
  "Canal Plus",
  "CCETT",
  "Deutsche Telecom",
  "Eurodec",
  "France Telecom",
  "Irdeto",
  "Jerrold/GI",
  "Matra Communication",
  "News Datacom",
  "Nokia",
  "Norwegian Telekom",
  "NTL",
  "Philips",
  "Scientific Atlanta",
  "Sony",
  "Tandberg Television",
  "Thomson",
  "TV/Com",
  "HPT - Croatian Post and Telecommunications",
  "HRT - Croatian Radio and Television",
  "IBM",
  "Nera",
  "BetaTechnik",
};

#define CA_SYSTIME_ID_MAX_CNT ((sizeof(g_ca_system_desc))/(sizeof(char *)))

/*!
 * program sort struct
 */
typedef struct
{
 /*!
  progame node id.
  */
  u16 node_id;
 /*!
  service number in stream 
  */
  u16 s_id;
}prog_sort_t;

typedef struct
{
 /*!
  progame node id.
  */
  u16 node_id;
 /*!
  video pidin stream 
  */
  u32 video_pid;
  /*!
  tp freq in stream 
  */
  u32 tp_freq;
}prog_tp_freq_sort_t;

typedef struct
{
 /*!
  progame node id.
  */
  u16 node_id;
  /*
  service number in stream 
  */
  u16 s_id;
 /*!
  transport id in stream 
  */
  u16 ts_id;
}prog_ts_service_sort_t;

static u8 g_pg_view_id = 0;

static u8 *p_tp_view_buf = NULL;
static u8 *p_pg_view_buf = NULL;

static u16 old_view_type;
static u32 old_group;
static u32 old_group_context;
static u8 old_mode;

void ui_cache_view(void)
{ 
  sys_status_get_curn_view_info(&old_view_type, &old_group_context);
  old_group = sys_status_get_curn_group();
  old_mode = sys_status_get_curn_prog_mode();
}

void ui_restore_view(void)
{ 
  if(old_view_type<DB_DVBS_INVALID_VIEW)
  {
    ui_dbase_set_pg_view_id(ui_dbase_create_view(old_view_type, old_group_context, NULL));
  }
  else
  {
    ui_dbase_set_pg_view_id(ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL));
  }
  sys_status_set_curn_group((u16)old_group);
  sys_status_set_curn_prog_mode(old_mode);
  sys_status_save();
}

void ui_dbase_init(void)
{

  p_tp_view_buf = mtos_malloc(DB_DVBS_MAX_TP* sizeof(item_type_t));
  MT_ASSERT(p_tp_view_buf != NULL);

  p_pg_view_buf = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(item_type_t));
  MT_ASSERT(p_pg_view_buf != NULL);  
}

u8 ui_dbase_create_view(dvbs_view_t name, u32 context, u8 *p_ex_buf)
{
  u8 *p_view_buf = NULL;
  
  if(p_ex_buf != NULL)
  {
    p_view_buf = p_ex_buf;
  }
  else
  {
    switch (name)
    {
      case DB_DVBS_ALL_RADIO:
      case DB_DVBS_ALL_RADIO_IGNORE_SKIP_FLAG:
      case DB_DVBS_ALL_TV:
      case DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG:
      case DB_DVBS_FAV_RADIO:
      case DB_DVBS_FAV_TV:
      case DB_DVBS_ALL_PG:
      case DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG:
      case DB_DVBS_TV_RADIO:
      case DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG:
      case DB_DVBS_FAV_ALL:
      case DB_DVBS_TP_PG:
      case DB_DVBS_TP_PG_IGNORE_SKIP_FLAG:
      case DB_DVBS_MOSAIC:
      case DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG:
        p_view_buf = p_pg_view_buf;
        break;
      case DB_DVBS_ALL_TP:
        p_view_buf = p_tp_view_buf;
        break;
      default:
        MT_ASSERT(0);
        break;
    }    
  }
  
  return db_dvbs_create_view(name, context, p_view_buf);
}

dvbs_view_t ui_dbase_get_prog_view (dvbs_prog_node_t *p_node)
{
	return (dvbs_view_t)(p_node->video_pid ? DB_DVBS_ALL_TV : DB_DVBS_ALL_RADIO);
}

u16 ui_dbase_get_tp_by_pos (u8 view_id, dvbs_tp_node_t *p_node, u16 pos)
{
	u16 tp_id;

	tp_id = db_dvbs_get_id_by_view_pos (view_id, pos);
	db_dvbs_get_tp_by_id (tp_id, p_node);

	return tp_id;
}

u32 ui_dbase_get_cur_freq(void)
{
      dvbs_prog_node_t pg;
      dvbs_tp_node_t tp;
      db_dvbs_ret_t ret;
      u16 pg_id;
      u32 freq = 0;
      
      pg_id = sys_status_get_curn_group_curn_prog_id();
      if(INVALIDID != pg_id)
      {
        ret = db_dvbs_get_pg_by_id(pg_id, &pg);
        MT_ASSERT(DB_DVBS_OK == ret);
        ret = db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
        MT_ASSERT(DB_DVBS_OK == ret);
        freq = tp.freq;
      }

    return freq;
}

void ui_dbase_get_full_prog_name(dvbs_prog_node_t *p_node, u16 *str, u16 max_length)
{
  u32 len;
  
  str_asc2uni(p_node->is_scrambled ? (u8 *)"$ " : (u8 *)"", str);
  len = uni_strlen(str);
  if (len < max_length)
  {
    uni_strncpy(str + len, p_node->name, max_length - len);
  }
}

BOOL ui_dbase_pg_is_scambled(u16 rid)
{
  dvbs_prog_node_t pg;

  if (rid == INVALIDID)
    return FALSE;

	if (db_dvbs_get_pg_by_id(rid, &pg) != DB_DVBS_OK)
    MT_ASSERT(0);

  return (BOOL)(pg.is_scrambled);
}

BOOL ui_dbase_pg_is_full(void)
{
  //u8 view = (u8)ui_sys_get_data(SS_CUR_VIEW);
  BOOL is_full = is_pg_full();

  ui_dbase_reset_pg_view();
  return is_full;
}

BOOL ui_dbase_pg_ask_for_belong(u16 pg_id, u16 *tp_pos)
{
  dvbs_prog_node_t pg;
  dvbs_tp_node_t tp;
  u8 view_id;
  
  *tp_pos = INVALIDPOS;
  
  if (pg_id == INVALIDID)
    return FALSE;

  if (db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
    return FALSE;

  if (db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp) != DB_DVBS_OK)
    return FALSE;

  view_id = ui_dbase_create_view(DB_DVBS_ALL_TP, 0, NULL);
  *tp_pos = db_dvbs_get_view_pos_by_id(view_id, (u16)pg.tp_id);

  return TRUE;
}

u32 ui_dbase_get_pg_curn_audio_pid(dvbs_prog_node_t *p_node)
{
  return p_node->audio[p_node->audio_channel].p_id;
}


void ui_dbase_set_pg_curn_audio_pid(dvbs_prog_node_t *p_node, u32 pid)
{
  p_node->audio[p_node->audio_channel].p_id = (u16)pid;
}

void ui_dbase_set_pg_view_id(u8 view_id)
{
  g_pg_view_id = view_id;
}

u8 ui_dbase_get_pg_view_id(void)
{
  return g_pg_view_id;
}

void ui_dbase_reset_pg_view(void)
{
  //u16 view = ui_sys_get_data(SS_CUR_VIEW);
  u16 view;
  u32 context;
  sys_status_get_curn_view_info(&view, &context);
  
  if (view != DB_DVBS_INVALID_VIEW)
  {
    /* create view for new view */
    ui_dbase_set_pg_view_id(ui_dbase_create_view(view, context, NULL));
  }
}

void ui_dbase_delete_all_pg(void)
{
  u8 view_id;
  u16 i, cnt;
  u16 param = DB_DVBS_PARAM_ACTIVE_FLAG;
  
  //view_id = ui_dbase_create_view(DB_DVBS_ALL_PG, 0, NULL);
  view_id = ui_dbase_create_view(DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG, 0, NULL);
  cnt = db_dvbs_get_count(view_id);

  for (i = 0; i < cnt; i++)
  {
      db_dvbs_change_mark_status(view_id, i, DB_DVBS_DEL_FLAG, param);
  }
  db_dvbs_save(view_id);

  book_delete_node_all();
}

BOOL ui_dbase_pg_is_fav(u8 view_id, u16 pos)
{
  u8 dig;
	
  for (dig = 0; dig < 1; dig++)
  {
  	if( db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, dig) == TRUE)
    //if(db_dvbs_get_mark_value(view_id, pos, dig))
    {
      return TRUE;
    }
  }

  return FALSE;
}

BOOL ui_dbase_check_tvradio(BOOL is_tv)
{
  u8 old_mode;
  u16 old_group;
  u16 view_type;
  u32 group_context;
  BOOL ret = FALSE;

  old_mode = sys_status_get_curn_prog_mode();
  old_group= sys_status_get_curn_group();

  sys_status_get_view_info(old_group, old_mode, &view_type, &group_context);

  if(db_dvbs_get_count(ui_dbase_create_view(
    is_tv?DB_DVBS_ALL_TV:DB_DVBS_ALL_RADIO, 0, NULL)))
  {
    ret = TRUE;
  }

  //reset current view
  if(DB_DVBS_INVALID_VIEW != view_type)
  {
    ui_dbase_set_pg_view_id(ui_dbase_create_view(view_type, group_context, NULL));
  }
  
  return ret;
}

static u32 freq_val[] = { 5150,   5750,  5950,  9750, 10000, 10050,
                          10450, 10600, 10700, 10750, 11250, 11300 };

#define FREQ_VAL_CNT    (sizeof(freq_val) / sizeof(u32))

void ui_dbase_pos2freq(u16 pos, u32 *p_lnbtype, u32 *p_freq1, u32 *p_freq2,
  u8 *p_is_unicable, u8 *p_unicable_type)
{
  *p_is_unicable = 0;
  *p_unicable_type = 0;
  *p_freq1 = 0;
  *p_freq2 = 0;
  *p_lnbtype = 0;
  
  if (pos < LNB_FREQ_CNT)
  {
    if (pos < FREQ_VAL_CNT)
    {
      *p_lnbtype = 0; // standard
      *p_freq1 = *p_freq2 = freq_val[pos];
    }
    else if (pos < FREQ_VAL_CNT + 2)
    {
      *p_lnbtype = 1; // user
      if (pos == FREQ_VAL_CNT)
      {
        *p_freq1 = 5150;
        *p_freq2 = 5750;
      }
      else
      {
        *p_freq1 = 5750;
        *p_freq2 = 5150;
      }
    }
    else if(pos < (FREQ_VAL_CNT + 3))
    {
      *p_lnbtype = 2; // universal
      *p_freq1 = 9750;
      *p_freq2 = 10600;
    }
    else
    {
      *p_lnbtype = 2; //unicable_universal
      *p_is_unicable = TRUE;
      *p_unicable_type = (pos - FREQ_VAL_CNT -3);
      *p_freq1 = 9750;
      *p_freq2 = 10600;
    }
  }
  else
  {
    UI_PRINTF("pos2feq: ERROR, invaild pos (%d)\n", pos);
    MT_ASSERT(0);
    *p_lnbtype = *p_freq1 = *p_freq2 = 0;
  }
}


static u16 get_freq_index(u32 freq)
{
  u16 i;

  for (i = 0; i < FREQ_VAL_CNT; i++)
  {
    if (freq_val[i] == freq)
    {
      return i;
    }
  }

  UI_PRINTF("get_freq_index: ERROR, freq(%d) is NOT matched\n", freq);
  return 0;
}


u16 ui_dbase_freq2pos(u32 lnbtype, u32 freq1, u32 freq2, u8 user_band, u8 unicable_type)
{
  u16 pos;

  switch (lnbtype)
  {
    case 0: // standard
//      MT_ASSERT(freq1 == freq2);
      pos = get_freq_index(freq1);
      break;
    case 1: // user
//      MT_ASSERT(freq1 != freq2);
      pos = freq1 < freq2 ? FREQ_VAL_CNT : FREQ_VAL_CNT + 1;
      break;
    case 2: // universal
      if(user_band == 0)
      {
        pos = FREQ_VAL_CNT + 2;
      }
      else
      {
        pos = FREQ_VAL_CNT + 3 + unicable_type;
      }
      break;
    default:
      MT_ASSERT(0);
      pos = 0;
  }

  return pos;
}



BOOL ui_dbase_reset_last_prog(BOOL focus_first)
{
  u8 mode, view_id;
  BOOL ret = FALSE;

  sys_status_check_group();

  mode = sys_status_get_curn_prog_mode();

  if (mode != CURN_MODE_NONE)
  {
    // set curn group to group_all
    sys_status_set_curn_group(0);
    view_id = ui_dbase_create_view(
      (mode == CURN_MODE_TV) ? DB_DVBS_ALL_TV : DB_DVBS_ALL_RADIO, 0, NULL);
    ui_dbase_set_pg_view_id(view_id);

    //if it's first scan, focus on first pg.
    if(focus_first)
    {
      sys_status_set_curn_group_info(db_dvbs_get_id_by_view_pos(view_id, 0), 0);
    }

    ret = TRUE;
  }
  // save
  sys_status_save();

  return ret;
}

char * ui_dbase_get_ca_system_desc(dvbs_prog_node_t *p_pg)
{
  u32 index = 0;
  MT_ASSERT(p_pg != NULL);

  if (p_pg->ca_system_id == 0 
    || !p_pg->is_scrambled)
  {
    index = 0; // reserved
  }
  else
  {
    index = SYS_GET_BYTE1(p_pg->ca_system_id) + 1/* skip reserved */;
    if (index > CA_SYSTIME_ID_MAX_CNT)
    {
      OS_PRINTF("ERROR! ca_system_id = 0x%x, force change to 0\n", p_pg->ca_system_id);
      index = 0;
    }
  }

  if(index >= sizeof(g_ca_system_desc)/sizeof(char*))
  {
    index = 0; // reserved
  }
  return g_ca_system_desc[index];
}

/*
BOOL pg_cmp_sev_id(prog_sort_t* prev_node, prog_sort_t* cur_node)
{
  return prev_node->s_id > cur_node->s_id;
}
*/

int pg_cmp_sev_id(const void *node1, const void *node2)
{
  prog_sort_t *pre_node = (prog_sort_t *) node1;
  prog_sort_t *cur_node = (prog_sort_t *) node2;
  
  return (pre_node->s_id - cur_node->s_id);
}

int pg_cmp_video_pid(const void *node1, const void *node2)
{
  prog_tp_freq_sort_t *pre_node = (prog_tp_freq_sort_t *) node1;
  prog_tp_freq_sort_t *cur_node = (prog_tp_freq_sort_t *) node2;
  
  return (pre_node->video_pid- cur_node->video_pid);
}

int pg_cmp_tp_freq(const void *node1, const void *node2)
{
  prog_tp_freq_sort_t *pre_node = (prog_tp_freq_sort_t *) node1;
  prog_tp_freq_sort_t *cur_node = (prog_tp_freq_sort_t *) node2;
  
  return (pre_node->tp_freq - cur_node->tp_freq);
}

int pg_cmp_ts_id(const void *node1, const void *node2)
{
  prog_ts_service_sort_t *pre_node = (prog_ts_service_sort_t *) node1;
  prog_ts_service_sort_t *cur_node = (prog_ts_service_sort_t *) node2;
  
  return (pre_node->ts_id- cur_node->ts_id);
}

int pg_cmp_service_id(const void *node1, const void *node2)
{
  prog_ts_service_sort_t *pre_node = (prog_ts_service_sort_t *) node1;
  prog_ts_service_sort_t *cur_node = (prog_ts_service_sort_t *) node2;
  
  return (pre_node->s_id - cur_node->s_id);
}

void shellsort(prog_sort_t *p_sort, u32 count)
{
  u32 i, j, incre;
  prog_sort_t temp_prog = {0};
//  db_dvbs_qsort_func sort_func = NULL;
  u16 size = sizeof(prog_sort_t);
  
  for(incre = count / 2; incre > 0; incre /= 2)
  {
    for(i = incre; i < count; i++)
    {
      memcpy(&temp_prog, &p_sort[i], size);

      for(j = i; j >= incre; j -= incre)
      {
        //prior > current, change
        if(pg_cmp_sev_id(&p_sort[j - incre], &p_sort[i]) > 0)
        {
          memcpy(&p_sort[j], &p_sort[j - incre], size);
        }
        else
          break;
      }

      memcpy(&p_sort[j], &temp_prog, size);
    }
  }
}

void ui_dbase_pg_sort_by_sid(view_id)
{
  s16 view_cnt  = 0;
  s16 i = 0;
  dvbs_prog_node_t prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  prog_sort_t *p_sort_buf = NULL;
  prog_sort_t *p_pro_item = NULL;
  u16 element_id = 0;
  u16 *p_node_id_mem = NULL;
  
  view_cnt = db_dvbs_get_count(view_id);

  p_sort_buf = mtos_malloc(/*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, /*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_sort_t));

  p_node_id_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_node_id_mem != NULL);
  memset(p_node_id_mem, 0x0, view_cnt * sizeof(u16));
  
  p_pro_item = (prog_sort_t *)p_sort_buf;

  for(i = 0; i < view_cnt; i++)
  {
    element_id = db_dvbs_get_id_by_view_pos(view_id, i);
    ret = db_dvbs_get_pg_by_id(element_id, &prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->node_id = element_id;
    p_pro_item->s_id = prog_node.s_id;
    p_pro_item++;
  }

//  shellsort(p_sort_buf, view_cnt);
  qsort(p_sort_buf, view_cnt, sizeof(prog_sort_t), pg_cmp_sev_id);
  for(i = 0; i < view_cnt;i++)
  {
    *(p_node_id_mem + i) = p_sort_buf[i].node_id | 0x8000;
  }

  db_dvbs_dump_item_in_view(view_id, p_node_id_mem, view_cnt);

  mtos_free(p_node_id_mem);
  mtos_free(p_sort_buf);
}

void ui_dbase_pg_sort_by_freq_video_pid(view_id)
{
  s16 view_cnt  = 0;
  s16 i = 0;
  dvbs_prog_node_t prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  prog_tp_freq_sort_t *p_sort_buf = NULL;
  prog_tp_freq_sort_t *p_pro_item = NULL;
  u16 element_id = 0;
  u16 *p_node_id_mem = NULL;
  dvbs_tp_node_t tp;
  s16 same_tp_num = 1, cnt = 0;;
  view_cnt = db_dvbs_get_count(view_id);

  p_sort_buf = mtos_malloc(/*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_tp_freq_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, /*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_tp_freq_sort_t));

  p_node_id_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_node_id_mem != NULL);
  memset(p_node_id_mem, 0x0, view_cnt * sizeof(u16));
  
  p_pro_item = (prog_tp_freq_sort_t *)p_sort_buf;

  for(i = 0; i < view_cnt; i++)
  {
    element_id = db_dvbs_get_id_by_view_pos(view_id, i);
    ret = db_dvbs_get_pg_by_id(element_id, &prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->video_pid = prog_node.video_pid;

    ret = db_dvbs_get_tp_by_id((u16)prog_node.tp_id, &tp);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->node_id = element_id;
    p_pro_item->tp_freq= tp.freq;
    p_pro_item++;
  }

//  shellsort(p_sort_buf, view_cnt);
  qsort(p_sort_buf, view_cnt, sizeof(prog_tp_freq_sort_t), pg_cmp_tp_freq);
  for(i = 0; i < view_cnt - 1; i++)
  {
    if(p_sort_buf[i].tp_freq == p_sort_buf[i + 1].tp_freq)
    {
      same_tp_num ++;
    }
    else
    {
      qsort(&p_sort_buf[cnt], same_tp_num, sizeof(prog_tp_freq_sort_t), pg_cmp_video_pid);
      cnt = cnt + same_tp_num;
      same_tp_num = 1;
    }
    if(i == view_cnt - 2)
    {
      qsort(&p_sort_buf[cnt], same_tp_num, sizeof(prog_tp_freq_sort_t), pg_cmp_video_pid);
      cnt = cnt + same_tp_num;
      same_tp_num = 1;
    }
  }
  for(i = 0; i < view_cnt;i++)
  {
    *(p_node_id_mem + i) = p_sort_buf[i].node_id | 0x8000;
  }

  db_dvbs_dump_item_in_view(view_id, p_node_id_mem, view_cnt);

  mtos_free(p_node_id_mem);
  mtos_free(p_sort_buf);
}

void ui_dbase_pg_sort_by_ts_service_pid(view_id)
{
  s16 view_cnt  = 0;
  s16 i = 0;
  dvbs_prog_node_t prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  prog_ts_service_sort_t *p_sort_buf = NULL;
  prog_ts_service_sort_t *p_pro_item = NULL;
  u16 element_id = 0;
  u16 *p_node_id_mem = NULL;
  s16 same_ts_num = 1, cnt = 0;;
  view_cnt = db_dvbs_get_count(view_id);

  p_sort_buf = mtos_malloc(/*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_ts_service_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, /*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_ts_service_sort_t));

  p_node_id_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_node_id_mem != NULL);
  memset(p_node_id_mem, 0x0, view_cnt * sizeof(u16));
  
  p_pro_item = (prog_ts_service_sort_t *)p_sort_buf;

  OS_PRINTF("\n**********************************\n");
  OS_PRINTF("enter ts id and service id sequence\n");
  OS_PRINTF("\n**********************************\n");

  for(i = 0; i < view_cnt; i++)
  {
    element_id = db_dvbs_get_id_by_view_pos(view_id, i);
    ret = db_dvbs_get_pg_by_id(element_id, &prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->s_id = prog_node.s_id;
    p_pro_item->node_id = element_id;
    p_pro_item->ts_id= prog_node.ts_id;
    OS_PRINTF("p_pro_item s_id %d,node_id %d,ts_id 0x%x\n",p_pro_item->s_id,p_pro_item->node_id,p_pro_item->ts_id);
    p_pro_item++;
  }

  //  shellsort(p_sort_buf, view_cnt);
  qsort(p_sort_buf, view_cnt, sizeof(prog_ts_service_sort_t), pg_cmp_ts_id);
  for(i = 0; i < view_cnt - 1; i++)
  {
    if(p_sort_buf[i].ts_id== p_sort_buf[i + 1].ts_id)
    {
      same_ts_num ++;
    }
    else
    {
      qsort(&p_sort_buf[cnt], same_ts_num, sizeof(prog_ts_service_sort_t), pg_cmp_service_id);
      cnt = cnt + same_ts_num;
      same_ts_num = 1;
    }
    if(i == view_cnt - 2)
    {
      qsort(&p_sort_buf[cnt], same_ts_num, sizeof(prog_ts_service_sort_t), pg_cmp_service_id);
      cnt = cnt + same_ts_num;
      same_ts_num = 1;
    }
  }

  OS_PRINTF("view_cnt = %d\n",view_cnt);
  for(i = 0; i < view_cnt;i++)
  {
    OS_PRINTF("p_sort_buf[%d].node_id = %d\n",i,p_sort_buf[i].node_id);
    *(p_node_id_mem + i) = p_sort_buf[i].node_id | 0x8000;
  }

  db_dvbs_dump_item_in_view(view_id, p_node_id_mem, view_cnt);

  mtos_free(p_node_id_mem);
  mtos_free(p_sort_buf);

}

void ui_dbase_pg_sort(dvbs_view_t name)
{
  u8 view_id;
  customer_cfg_t cfg = {0};
  
  get_customer_config(&cfg);

  MT_ASSERT(name<DB_DVBS_INVALID_VIEW);

  view_id = db_dvbs_create_view(name, 0, NULL);
  if(db_dvbs_get_count(view_id) > 0)
  {
   #ifdef LCN_SWITCH
    sort_by_logic_num(view_id);
   #else
    if(cfg.customer == CUSTOMER_YINGJI)
    {
      ui_dbase_pg_sort_by_freq_video_pid(view_id);
    }
    else if(cfg.customer == CUSTOMER_CHANGJIANG_QY)
    {
	ui_dbase_pg_sort_by_ts_service_pid(view_id);
    }
    else
    {
      ui_dbase_pg_sort_by_sid(view_id);
    }
   #endif
    db_dvbs_save(view_id);
  }
  
}
