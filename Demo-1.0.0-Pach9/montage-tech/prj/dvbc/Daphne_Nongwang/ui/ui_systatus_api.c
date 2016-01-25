/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "ui_time_api.h"
#include "sys_cfg.h"
#include "mem_cfg.h"


//#define SS_DEBUG

#ifdef SS_DEBUG
#define SS_PRINTF    OS_PRINTF
#else
#ifndef WIN32
#define SS_PRINTF(param ...)    do { } while (0)
#else
#define SS_PRINTF
#endif
#endif

static void _check_curn_pg(curn_pg_info_t *p_info, u8 view_id)
{
  u16 rid, pos, cnt;

  // chk by cnt
  cnt = db_dvbs_get_count(view_id);
  SS_PRINTF("\t - total = %d\n", cnt);
  if (cnt == 0)
  {
    p_info->id = INVALIDID, p_info->pos = INVALIDPOS;
    return;
  }

  // chk by id
  pos = db_dvbs_get_view_pos_by_id(view_id, p_info->id);
  if (pos != INVALIDPOS)
  {
    SS_PRINTF("\t - id[%d] is ok, update pos[%d]\n", p_info->id, pos);
    p_info->pos = pos;
    return;
  }
  SS_PRINTF("\t - id[%d] is invalid \n", p_info->id);

  // chk by pos
  rid = db_dvbs_get_id_by_view_pos(view_id, p_info->pos);
  if (rid != INVALIDID)
  {
    SS_PRINTF("\t - pos[%d] is ok, update id[%d]\n", p_info->pos, rid);
    p_info->id = rid;
    return;
  }
  SS_PRINTF("\t - pos[%d] is invalid \n", p_info->pos);

  // try to keep the pos
  pos = p_info->pos;
  if (pos == INVALIDPOS || pos >= cnt)
  {
    pos = cnt - 1; /* the last */
  }

  rid = db_dvbs_get_id_by_view_pos(view_id, pos);
  MT_ASSERT(rid != INVALIDID);

  p_info->id = rid, p_info->pos = pos;
  SS_PRINTF("\t - all is invalid, update id[%d] & pos[%d]\n", 
    p_info->id, p_info->pos);
}


static void _check_group_all(group_set_t *p_group_set)
{
  curn_pg_info_t *p_info;
  u8 pg_vid;

  SS_PRINTF("check group all [tv] ...\n");
  pg_vid = ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL);
  p_info = &p_group_set->group_info[0].curn_tv;
  _check_curn_pg(p_info, pg_vid);

  SS_PRINTF("check group all [radio] ...\n");
  pg_vid = ui_dbase_create_view(DB_DVBS_ALL_RADIO, 0, NULL);
  p_info = &p_group_set->group_info[0].curn_radio;
  _check_curn_pg(p_info, pg_vid);

  p_group_set->group_info[0].context = 0;
}





static void _check_group_fav(group_set_t *p_group_set)
{
  curn_info_t *p_group;
  curn_pg_info_t *p_info;
  u16 i, j;
  u8 pg_vid;

  for (i = 0, j = 0; i < 1; i++)
  {
    pg_vid = ui_dbase_create_view(DB_DVBS_FAV_ALL, i , NULL);
    if(db_dvbs_get_count(pg_vid) != 0)
    {
      p_group = &p_group_set->group_info[j + 1 /*all group*/];
      
      pg_vid = ui_dbase_create_view(DB_DVBS_FAV_TV, i , NULL);
      
      SS_PRINTF("check group fav%d [tv] ...\n", i);
      p_info = &p_group->curn_tv;
      _check_curn_pg(p_info, pg_vid);

      pg_vid = ui_dbase_create_view(DB_DVBS_FAV_RADIO, i , NULL);

      SS_PRINTF("check group fav%d [radio] ...\n", i);
      p_info = &p_group->curn_radio;
      _check_curn_pg(p_info, pg_vid);

      p_group->context = (i);

      j++;
    }
  }
  
  for (;j < MAX_FAV_CNT; j++)
  {
    p_group = &p_group_set->group_info[j + 1 /*all group*/];

    SS_PRINTF("check group fav%d [no prog] ...\n", j);
    p_info = &p_group->curn_tv;
    p_info->id = INVALIDID, p_info->pos = INVALIDPOS;
    p_info = &p_group->curn_radio;
    p_info->id = INVALIDID, p_info->pos = INVALIDPOS;
    p_group->context = 0;
  }
}

static void _check_group_fav_all(group_set_t *p_group_set)
{
  curn_info_t *p_group;
  curn_pg_info_t *p_info;
  u16 i, j;
  u8 pg_vid;
  u16 p_count[MAX_FAV_CNT]={0};

  pg_vid = db_dvbs_create_view_all(DB_DVBS_FAV_ALL,p_count,MAX_FAV_CNT,  NULL);

  for (i = 0, j= 0; i < 1; i++)
  {
    if(p_count[i] != 0)
    {
      p_group = &p_group_set->group_info[j + 1 /*all group*/];

      pg_vid = db_dvbs_create_view(DB_DVBS_FAV_TV, i , NULL);
      
      SS_PRINTF("check group fav%d [tv] ...\n", i);
      p_info = &p_group->curn_tv;
      _check_curn_pg(p_info, pg_vid);

      pg_vid = db_dvbs_create_view(DB_DVBS_FAV_RADIO, i , NULL);

      SS_PRINTF("check group fav%d [radio] ...\n", i);
      p_info = &p_group->curn_radio;
      _check_curn_pg(p_info, pg_vid);

      p_group->context = (i);

      j++;

    }
    else
    {
      p_group = &p_group_set->group_info[j + 1 /*all group*/];
      SS_PRINTF("check group fav%d [no prog] ...\n", j);
      p_info = &p_group->curn_tv;
      p_info->id = INVALIDID, p_info->pos = INVALIDPOS;
      p_info = &p_group->curn_radio;
      p_info->id = INVALIDID, p_info->pos = INVALIDPOS;
      p_group->context = 0;
      j++;
    }
  }

}

static BOOL _get_curn_info(group_set_t *p_group_set, u16 pos, u8 mode,
                           u16 *p_curn_rid,
                           u16 *p_curn_pos)
{
  curn_info_t *p_group_info;
  curn_pg_info_t *p_curn_prog;

  if (mode == CURN_MODE_NONE)
  {
    return FALSE;
  }

  p_group_info = &p_group_set->group_info[pos];
  p_curn_prog =
    mode == CURN_MODE_TV ? &p_group_info->curn_tv : &p_group_info->curn_radio;

  if (p_curn_prog->id == INVALIDID || p_curn_prog->pos == INVALIDPOS)
  {
    *p_curn_rid = INVALIDID, *p_curn_pos = INVALIDPOS;
    return FALSE;
  }

  *p_curn_rid = p_curn_prog->id;
  *p_curn_pos = p_curn_prog->pos;

  return TRUE;
}


static BOOL _set_curn_info(group_set_t *p_group_set, u16 pos, u8 mode,
                           u16 curn_rid,
                           u16 curn_pos)
{
  curn_info_t *p_group_info;
  curn_pg_info_t *p_curn_prog;

  if (mode == CURN_MODE_NONE)
  {
    return FALSE;
  }

  p_group_info = &p_group_set->group_info[pos];
  p_curn_prog =
    mode == CURN_MODE_TV ? &p_group_info->curn_tv : &p_group_info->curn_radio;

  p_curn_prog->id = curn_rid;
  p_curn_prog->pos = curn_pos;

  return TRUE;
}


static BOOL _check_curn_group(group_set_t *p_group_set, u8 mode)
{
  u16 i, rid, pos, curn = p_group_set->curn_group;
  
  //check curn-> total-1
  for (i = curn; i < MAX_GROUP_CNT; i++)
  {
    if (_get_curn_info(p_group_set, i, mode, &rid, &pos))
    {
      p_group_set->curn_group = i;//curn;
      return TRUE;
    }
  }

  //check curn-1 -> 0
  for (i = 0; i < curn; i++)
  {
    if (_get_curn_info(p_group_set, i, mode, &rid, &pos))
    {
      p_group_set->curn_group = i;//curn;
      return TRUE;
    }
  }

  return FALSE;
}


static BOOL _check_curn_info(group_set_t *p_group_set)
{
  u8 mode = p_group_set->curn_mode;
  u32 org_context, new_context;
  u8 org_type, new_type;
  u16 pos;
  
  sys_status_get_group_info(sys_status_get_curn_group(), 
      &org_type, &pos, &org_context);
  
  if (mode == CURN_MODE_NONE)
  {
    mode = CURN_MODE_TV;
  }

  // check curn mode
  if (_check_curn_group(p_group_set, mode))
  {
    goto _SET_CURN_MODE_;
  }

  // switch curn mode
  if (mode == CURN_MODE_TV)
  {
    mode = CURN_MODE_RADIO;
  }
  else
  {
    mode = CURN_MODE_TV;
  }

  // check other mode
  if (_check_curn_group(p_group_set, mode))
  {
    goto _SET_CURN_MODE_;
  }

  // no prog in all mode
  mode = CURN_MODE_NONE;
  p_group_set->curn_group = 0;

 _SET_CURN_MODE_:
  p_group_set->curn_mode = mode;

  sys_status_get_group_info(sys_status_get_curn_group(), 
      &new_type, &pos, &new_context);
      
  return ((org_context == new_context) && (org_type == new_type));
}


BOOL sys_status_check_group_view(BOOL del_flag)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  if(del_flag == TRUE)
    _check_group_all(p_group_set);

  _check_group_fav_all(p_group_set);

  // curn mode & curn group
  return _check_curn_info(p_group_set);
}

BOOL sys_status_check_group(void)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

   // all prog
  _check_group_all(p_group_set);

  // fav group
  _check_group_fav(p_group_set);

  // curn mode & curn group
  return _check_curn_info(p_group_set);
}


u16 sys_status_get_fav_group_num(void)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  u8 mode;
  u16 i, num = 0;
  u16 rid, pos;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  mode = p_group_set->curn_mode;

  for (i = 0; i < MAX_FAV_CNT; i++)
  {
    if (_get_curn_info(p_group_set, i + 1 /*all*/, mode, &rid,
                       &pos))
    {
      num++;
    }
  }

  return num;
}

u16 sys_status_get_fav_group_pos(u16 group)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  u8 mode;
  u16 i, num = 0;
  u16 rid, pos;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  mode = p_group_set->curn_mode;

  MT_ASSERT(group <= MAX_FAV_CNT);
  
  for (i = 0; i < group; i++)
  {
    if (_get_curn_info(p_group_set, i + 1 /*all*/, mode, &rid,
                       &pos))
    {
      num++;
    }
  }

  return num;
}

u16 sys_status_get_all_group_num(void)
{
  u16 num = 0;

  num += sys_status_get_fav_group_num();

  if (num > 0)
  {
    num++; /*all*/
  }

  return num;
}

BOOL sys_status_get_group_all_info(u8 mode,
                                   u16 *p_curn_rid, u16 *p_curn_pos)
{
  u16 rid, pos;
  group_set_t *p_group_set;
  sys_status_t *p_status;
  
  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  if (!_get_curn_info(p_group_set, 0, mode, &rid, &pos))
  {
    return FALSE;
  }

  *p_curn_rid = rid;
  *p_curn_pos = pos;

  return TRUE;
}




BOOL sys_status_get_fav_group_info(u16 fav_pos, u8 mode,
                                   u16 *p_curn_rid, u16 *p_curn_pos)
{
  u16 rid, pos;
  group_set_t *p_group_set;
  sys_status_t *p_status;

  if (fav_pos >= MAX_FAV_CNT)
  {
    return FALSE;
  }

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  if (!_get_curn_info(p_group_set, fav_pos + 1 /*all*/, mode,
                      &rid, &pos))
  {
    *p_curn_rid = INVALIDID, *p_curn_pos = INVALIDPOS;
    return FALSE;
  }

  *p_curn_rid = rid;
  *p_curn_pos = pos;

  return TRUE;
}

BOOL sys_status_set_group_all_info(u8 mode,
                                   u16 curn_rid, u16 curn_pos)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  if (!_set_curn_info(p_group_set, 0, mode, curn_rid,
                      curn_pos))
  {
    return FALSE;
  }

  return TRUE;
}



BOOL sys_status_set_fav_group_info(u16 fav_pos, u8 mode,
                                   u16 curn_rid, u16 curn_pos)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  if (fav_pos >= MAX_FAV_CNT)
  {
    return FALSE;
  }

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  if (!_set_curn_info(p_group_set, fav_pos + 1 /*all*/, mode,
                      curn_rid, curn_pos))
  {
    return FALSE;
  }

  return TRUE;
}

BOOL sys_status_get_group_info(u16 group, u8 *p_group_type, u16 *p_pos_in_set, u32 *p_context)
{
  sys_status_t *p_status;
  group_set_t *p_group_set;
  
  u16 pos;

  if (group >= MAX_GROUP_CNT)
  {
    return FALSE;
  }

  if (group == 0)
  {
    pos = 0;
    *p_group_type = GROUP_T_ALL;
  }
  else
  {
    pos = group - 1;
    *p_group_type = GROUP_T_FAV;
  }

  *p_pos_in_set = pos;


  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  *p_context =  p_group_set->group_info[group].context;
  
  return TRUE;
}


u16 sys_status_get_curn_group(void)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  return p_group_set->curn_group;
}


BOOL sys_status_set_curn_group(u16 group)
{
  u8 type;
  u16 pos;
  u32 context;

  group_set_t *p_group_set;
  sys_status_t *p_status;

  // check
  if (!sys_status_get_group_info(group, &type, &pos, &context))
  {
    return FALSE;
  }

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  p_group_set->curn_group = group;
  return TRUE;
    }


static u16 goto_next_group(group_set_t *p_group_set, BOOL is_reverse)
{
  s16 curn_group, next_group;
  u16 rid, pos;
  
  curn_group = (s16)p_group_set->curn_group;
  next_group = curn_group;

  do
  {
    if (is_reverse)
    {
      next_group--;
      if (next_group < 0)
      {
        next_group = MAX_GROUP_CNT-1;
      }
    }
    else
    {
      next_group++;
      if (next_group == MAX_GROUP_CNT)
      {
        next_group = 0;
      }
    }

    if (next_group == curn_group)
    {
      // no group has pgs, except curn group 
      break;
    }
  }while(!_get_curn_info(p_group_set, next_group, p_group_set->curn_mode, &rid, &pos));

  return (u16)next_group;
}

u16 sys_status_shift_curn_group(s16 offset)
{
  BOOL is_reverse;
  u16 i, count, curn_group;
  
  group_set_t *p_group_set;
  sys_status_t *p_status;

  count = ABS(offset);
  is_reverse = (BOOL)(offset < 0);

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  curn_group = p_group_set->curn_group;

  for (i = 0; i < count; i++)
  {
    curn_group = goto_next_group(p_group_set, is_reverse);
  }

  p_group_set->curn_group = curn_group;

  return curn_group;
}


BOOL sys_status_get_curn_prog_in_group(u16 group, u8 mode, u16 *p_rid,
                                       u16 *p_pos, u32 *p_context)
{
  u8 group_type;
  u16 group_pos;
  u32 context;

  group_set_t *p_group_set;
  sys_status_t *p_status;

  if (mode >= CURN_MODE_CNT)
  {
    return FALSE;
  }

  if (mode == CURN_MODE_NONE)
  {
    *p_rid = INVALIDID, *p_pos = INVALIDPOS;
    return TRUE;
  }

  if (!sys_status_get_group_info(group, &group_type, &group_pos, &context))
  {
    return FALSE;
  }

  *p_context = context;
  
  switch (group_type)
  {
    case GROUP_T_FAV:
      sys_status_get_fav_group_info(group_pos, mode, p_rid, p_pos);
      break;
    default: /* all group */
      p_status = sys_status_get();
      p_group_set = &p_status->group_set;
      _get_curn_info(p_group_set, 0, mode, p_rid, p_pos);
  }

  return TRUE;
}


BOOL sys_status_set_curn_prog_in_group(u16 group, u8 mode, u16 rid, u16 pos)
{
  u8 group_type;
  u16 group_pos;
  u32 context;

  group_set_t *p_group_set;
  sys_status_t *p_status;

  if (mode >= CURN_MODE_CNT)
  {
    return FALSE;
  }

  if (mode == CURN_MODE_NONE)
  {
    return TRUE;
  }

  if (!sys_status_get_group_info(group, &group_type, &group_pos, &context))
  {
    return FALSE;
  }

  switch (group_type)
  {
    case GROUP_T_FAV:
      sys_status_set_fav_group_info(group_pos, mode, rid, pos);
      break;
    default: /* all group */
      p_status = sys_status_get();
      p_group_set = &p_status->group_set;
      _set_curn_info(p_group_set, 0, mode, rid, pos);
  }

  return TRUE;
}


u8 sys_status_get_curn_prog_mode(void)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  return p_group_set->curn_mode;
}


BOOL sys_status_set_curn_prog_mode(u8 mode)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  if (mode >= CURN_MODE_CNT)
  {
    return FALSE;
  }

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  p_group_set->curn_mode = mode;

  return TRUE;
}

u16 sys_status_get_curn_group_curn_prog_id(void)
{
  u16 curn_group;
  u8 curn_mode;
  u16 pg_id;
  u16 pg_pos;
  u32 context;
  
  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();

  if(sys_status_get_curn_prog_in_group(curn_group, curn_mode, &pg_id, &pg_pos, &context))
  {
    return pg_id;
  }
  else
  {
    return INVALIDID;
  }
}

BOOL sys_status_set_curn_group_info(u16 pg_id, u16 pos)
{
  u16 curn_group;
  u8 curn_mode;

  
  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();

  return sys_status_set_curn_prog_in_group(curn_group, curn_mode, pg_id, pos);

}

void sys_status_get_view_info(u16 group, u8 mode, 
  u16 *p_view_type, u32 *p_context)
{
  u8 group_type, view_type;
  u16 pos_in_set;
  u32 context;
  
  if (mode == CURN_MODE_NONE)
  {
    *p_view_type = DB_DVBS_INVALID_VIEW;
    *p_context = 0;
    return;
  }

  sys_status_get_group_info(group, &group_type, &pos_in_set, &context);

  switch(group_type)
  {
    case GROUP_T_ALL:
      view_type = (mode == CURN_MODE_TV)?DB_DVBS_ALL_TV:DB_DVBS_ALL_RADIO;
      break;
    case GROUP_T_FAV:
      view_type = (mode == CURN_MODE_TV)?DB_DVBS_FAV_TV:DB_DVBS_FAV_RADIO;
      break;
    default:
      MT_ASSERT(0);
      return;
  }

  *p_view_type = view_type;
  *p_context = context;
}


void sys_status_get_curn_view_info(u16 *p_view_type, u32 *p_context)
{
  u8 curn_mode;
  u16 curn_group;

  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();

  sys_status_get_view_info(curn_group, curn_mode, p_view_type, p_context);
}


u16 sys_status_get_pos_by_group(u16 group)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  u8 mode;
  u16 i, num = 0;
  u16 rid, rpos;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  mode = p_group_set->curn_mode;


  for (i = 0; i <= group; i++)
  {
    if (_get_curn_info(p_group_set, i, mode, &rid, &rpos))
    {
      num++;
    }
  }

  return (num - 1);

}

u16 sys_status_get_group_by_pos(u16 pos)
{  
  group_set_t *p_group_set;
  sys_status_t *p_status;

  u8 mode;
  u16 i, num = 0;
  u16 rid, rpos;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  mode = p_group_set->curn_mode;


  for (i = 0; i < MAX_GROUP_CNT; i++)
  {
    if (_get_curn_info(p_group_set, i, mode, &rid, &rpos))
    {
      if(num == pos)
      {
        return i;
      }
      num++;
    }
  }

  return INVALIDPOS;
}

void sys_status_get_group_name(u16 group, u16 *p_name, u32 len)
{
  u8 type = 0;
  u16 pos = 0;
  u32 context = 0;
  
  sys_status_get_group_info(group, &type, &pos, &context);
  switch (type)
  {
    case GROUP_T_ALL:
      gui_get_string(IDS_ALL, p_name, (u16)len);
      break;
    case GROUP_T_FAV:
      sys_status_get_fav_name((u8)context, p_name);      
      break;
    default:
      MT_ASSERT(0);
  }
}

scart_v_format_t sys_status_get_scart_out_cfg(u8 focus)
{
  scart_v_format_t mode = SCART_VID_CVBS;

  switch(focus)
  {
    case 0: // CVBS
      mode = SCART_VID_CVBS;
      break;
    case 1: //YUV
      mode = SCART_VID_YUV;
      break;
    case 2: // RGB: DONT change the order.
      mode = SCART_VID_RGB;
      break;
    default:
     // MT_ASSERT(0);
      mode = SCART_VID_CVBS;
    break;
  }

  return mode;
}

video_out_format_t sys_status_get_video_out_cfg(u8 focus)
{
  video_out_format_t mode = VIDEO_OUT_VID_YUV;
  customer_cfg_t cfg = {0};
  OS_PRINTF("########sys_status_get_video_out_cfg   focus %d\n",focus);
  get_customer_config(&cfg);
  if((CUSTOMER_JINGGONG == cfg.customer) ||
             (CUSTOMER_BOYUAN == cfg.customer) ||
             (CUSTOMER_ZHILING== cfg.customer) ||
             (CUSTOMER_ZHILING_KF== cfg.customer) ||
             (CUSTOMER_ZHILING_LQ== cfg.customer) ||
             (CUSTOMER_SHENZHOU_QN== cfg.customer))
 {
    return VIDEO_OUT_VID_CVBS;
  }
  if((CUSTOMER_TDX_PAKISTAN == cfg.customer)||(CUSTOMER_FANTONG_KFAJX == cfg.customer)
  	||(CUSTOMER_FANTONG_BYAJX == cfg.customer)||(CUSTOMER_FANTONG_XSMAJX== cfg.customer)
  	||(CUSTOMER_ZHONGDA == cfg.customer)||(CUSTOMER_AOJIEXUN == cfg.customer))
  {
#ifndef WIN32
    return VIDEO_OUT_VID_CVBS_CVBS;
#endif
  }
  
  switch(focus)
  {
    case 0: //YUV and CVBS
      mode = VIDEO_OUT_VID_CVBS_YUV;
      break;
    case 1: // S_VIDEO and CVBS
      mode = VIDEO_OUT_VID_CVBS_SVDIEO;
      break;
    case 2:  //only RGB 
      mode = VIDEO_OUT_VID_RGB;
      break;
    case 3: // only CVBS 
      mode = VIDEO_OUT_VID_CVBS;
      break;
    case 4: // only YUV
      mode = VIDEO_OUT_VID_YUV;
    break;
    case 5: // only S_VIDE
      mode = VIDEO_OUT_VID_SVDIEO;
    break;
    default:
     // MT_ASSERT(0);
      mode = VIDEO_OUT_VID_CVBS_YUV;
    break;
  }

  return mode;
}
avc_video_aspect_1_t sys_status_get_video_aspect(u8 focus)
{
  avc_video_aspect_1_t mode =AVC_VIDEO_ASPECT_AUTO_1;
  switch(focus)
  {
    case 0: 
      mode = AVC_VIDEO_ASPECT_43_LETTERBOX_1;
      break;
    case 1: 
      mode = AVC_VIDEO_ASPECT_169_LETTERBOX_1;//AVC_VIDEO_ASPECT_169_1;
      break;
    case 2: 
      mode = AVC_VIDEO_ASPECT_AUTO_1;//AVC_VIDEO_ASPECT_DEFAULT_1;
      break;
    default:
      mode = AVC_VIDEO_ASPECT_AUTO_1;//AVC_VIDEO_ASPECT_DEFAULT_1;
    break;
  }
  return mode;
 #if 0
  if(focus < 2)
  {
    return (focus + 1);// except VIDEO_ASPECT_DEFAULT
  }
  else
  {
    return (focus + 2); //except VIDEO_ASPECT_DEFAULT & VIDEO_ASPECT_169
  }
  #endif

}

scart_v_aspect_t sys_status_get_scart_aspect(u8 focus)
{
  if(focus >= 5)
  {
  		focus =0;
  }
  return (sys_status_get_video_aspect(focus) >= AVC_VIDEO_ASPECT_169_1) ? 
         SCART_ASPECT_16_9 : SCART_ASPECT_4_3;
}


rf_sys_t sys_status_get_rf_system(u8 focus)
{
  rf_sys_t sys = RF_SYS_NTSC;

  return (rf_sys_t)(sys + focus);
}

u16 sys_status_get_rf_channel(BOOL is_pal, u8 focus)
{
  if(is_pal)  // PAL
  {
    focus += RF_PAL_MIN;
  }
  else // NTSC
  {
    focus += RF_NTSC_MIN;
  }

  return (u16)focus;
}

void sys_status_reload_environment_setting(void)
{
  BOOL is_enable;
  av_set_t av_set;
  osd_set_t osd_set;
  language_set_t lang_set;
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);

  nim_device_t *p_dev = dev_find_identifier(NULL,
                                            DEV_IDT_TYPE,
                                            SYS_DEV_TYPE_NIM);

  //set lnb power
  sys_status_get_status(BS_LNB_POWER, &is_enable);
  dev_io_ctrl(p_dev, NIM_IOCTRL_SET_LNB_ONOFF, (u32)is_enable);
  ui_time_lnb_check_enable(is_enable);
  
  //set video
  sys_status_get_av_set(&av_set);

  avc_set_video_aspect_mode_1(avc_handle, sys_status_get_video_aspect(av_set.tv_ratio));
  ui_reset_video_aspect_mode(sys_status_get_video_aspect(av_set.tv_ratio));
  
  avc_switch_video_mode_1(avc_handle, av_set.tv_mode);
  ui_reset_tvmode(av_set.tv_mode); /**cgf test***/
  //avc_switch_video_mode_jazz(avc_handle, av_set.tv_mode);

  avc_cfg_video_out_format_jazz(avc_handle, sys_status_get_video_out_cfg(av_set.video_output));
 // avc_cfg_scart_format_jazz(avc_handle, sys_status_get_scart_out_cfg(av_set.video_output));
  //avc_cfg_scart_aspect_1(avc_handle, sys_status_get_scart_aspect(av_set.tv_ratio));

  avc_set_rf_system_1(avc_handle, sys_status_get_rf_system(av_set.rf_system));
  avc_set_rf_channel_1(avc_handle, sys_status_get_rf_channel(av_set.rf_system, av_set.rf_channel));

  //set osd
  sys_status_get_osd_set(&osd_set);
  //gdi_set_global_alpha_montage((100 - osd_set.transparent) * 255 / 100);  
 gdi_set_global_alpha_montage(10*(10 - osd_set.transparent) * 255 / 100);

  avc_set_video_bright(avc_handle, osd_set.bright);
  avc_set_video_contrast(avc_handle, osd_set.contrast);
  avc_set_video_sature(avc_handle, osd_set.chroma);

  //set language
  sys_status_get_lang_set(&lang_set);
  rsc_set_curn_language(gui_get_rsc_handle(),lang_set.osd_text + 1);
  //sys_set_ca_language
  #ifndef WIN32
  ui_ca_set_language(lang_set);
  #endif
}

void sys_status_set_video_out(void)
{
   av_set_t av_set;
   class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
   sys_status_get_av_set(&av_set);
   avc_set_video_aspect_mode_1(avc_handle, sys_status_get_video_aspect(av_set.tv_ratio));
   ui_reset_video_aspect_mode(sys_status_get_video_aspect(av_set.tv_ratio));

   avc_switch_video_mode_1(avc_handle, av_set.tv_mode);
   //avc_cfg_scart_format_jazz(avc_handle, sys_status_get_scart_out_cfg(av_set.video_output));
   avc_cfg_video_out_format_jazz(avc_handle, sys_status_get_video_out_cfg(av_set.video_output));
}

