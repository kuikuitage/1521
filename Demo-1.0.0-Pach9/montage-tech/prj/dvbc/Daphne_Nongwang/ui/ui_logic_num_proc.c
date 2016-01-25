/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2012).
*                 All rights reserved.
* Owner:       Dave Bie <dave.bie@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_logic_num_proc.h"
#ifdef LCN_SWITCH_NIT
#include "nit_logic_num.h"
#endif
#ifdef LCN_SWITCH_BAT
#include "bat_logic_num.h"
#endif
/***************************************************************************
函数名：	do_search_logic_num_process
函数描述：	自动搜台后对LCN 处理
入口参数：	LCN 的参数

返回值：无
******************************************************************************/
 void do_search_logic_num_process(logic_channel log_ch_data)
{
 u8 view_id = 0;
 u16 pg_cnt, pg_id;
 u16 i ,j;
 dvbs_prog_node_t pg_node ={0};
 logic_channel_tag *p_log_ch =log_ch_data.p_logic_channel;
 view_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
 pg_cnt = db_dvbs_get_count(view_id);
 // 保存逻辑号和是否隐藏flag
 for(i=0;i<log_ch_data.log_ch_num;i++)
 {
  for(j=0;j<pg_cnt;j++)
   {
      pg_id = db_dvbs_get_id_by_view_pos(view_id, j);
      db_dvbs_get_pg_by_id(pg_id, &pg_node);
      if(pg_node.orig_net_id == p_log_ch->network_id &&
          pg_node.ts_id== p_log_ch->ts_id &&
          pg_node.s_id== p_log_ch->service_id)
        {
          pg_node.logic_ch_num = p_log_ch->logical_channel_id;
          pg_node.skp_flag =(p_log_ch->is_visible_flag ==0)? 1 : 0;
          db_dvbs_edit_program(&pg_node);
          break;
        }
    }
  p_log_ch++;
 }

//没有逻辑号的节目从1001分配逻辑号
  for(i=0;i<pg_cnt;i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &pg_node);
    if(pg_node.logic_ch_num ==0)
    {
      pg_node.logic_ch_num =DVB_LOGIC_START_NUMBER+i;
    }
    db_dvbs_edit_program(&pg_node);
  }
  db_dvbs_save(view_id);
}

/***************************************************************************
函数名：	manual_search_logic_num_process
函数描述：	手动搜台后对LCN 处理
入口参数：无

返回值：无
******************************************************************************/
void manual_search_logic_num_process(void)
{
 u8 view_id = 0;
 u16 pg_cnt, pg_id;
 u16 i ,max_logic_num=0;
 dvbs_prog_node_t pg_node ={0};
 
 view_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
 pg_cnt = db_dvbs_get_count(view_id);

 for(i=0;i<pg_cnt;i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &pg_node);
    if(pg_node.logic_ch_num > max_logic_num)
    {
       max_logic_num = pg_node.logic_ch_num;
     }
  }
 if(max_logic_num <1001)
 {
  max_logic_num =1000;
 }
 for(i=0;i<pg_cnt;i++)
 {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &pg_node);
    if(pg_node.logic_ch_num == 0)
    {
      pg_node.logic_ch_num = ++max_logic_num;
     }
    db_dvbs_edit_program(&pg_node);
  }
 db_dvbs_save(view_id);
}


/***************************************************************************
函数名：	pg_cmp_logic_num
函数描述：比较逻辑号
入口参数：node1,node2 

返回值：
******************************************************************************/
static int pg_cmp_logic_num(const void *node1, const void *node2)
{
  prog_logic_num_sort_t *pre_node = (prog_logic_num_sort_t *) node1;
  prog_logic_num_sort_t *cur_node = (prog_logic_num_sort_t *) node2;
  
  return (pre_node->logic_num- cur_node->logic_num);
}


/***************************************************************************
函数名：	sort_by_logic_num
函数描述：	根据逻辑号来排序
入口参数：view_id 

返回值：无
******************************************************************************/
void sort_by_logic_num(u8 view_id)
{
  s16 view_cnt  = 0;
  s16 i = 0;
  dvbs_prog_node_t prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  prog_logic_num_sort_t *p_sort_buf = NULL;
  prog_logic_num_sort_t *p_pro_item = NULL;
  u16 element_id = 0;
  u16 *p_node_id_mem = NULL;
  
  view_cnt = db_dvbs_get_count(view_id);

  p_sort_buf = mtos_malloc(view_cnt * sizeof(prog_logic_num_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, view_cnt * sizeof(prog_logic_num_sort_t));

  p_node_id_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_node_id_mem != NULL);
  memset(p_node_id_mem, 0x0, view_cnt * sizeof(u16));
  
  p_pro_item = (prog_logic_num_sort_t *)p_sort_buf;

  for(i = 0; i < view_cnt; i++)
  {
    element_id = db_dvbs_get_id_by_view_pos(view_id, i);
    ret = db_dvbs_get_pg_by_id(element_id, &prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->node_id = element_id;
    p_pro_item->logic_num= prog_node.logic_ch_num;
    p_pro_item++;
  }

  qsort(p_sort_buf, view_cnt, sizeof(prog_logic_num_sort_t), pg_cmp_logic_num);
  for(i = 0; i < view_cnt;i++)
  {
    *(p_node_id_mem + i) = p_sort_buf[i].node_id | 0x8000;
  }

  db_dvbs_dump_item_in_view(view_id, p_node_id_mem, view_cnt);

  mtos_free(p_node_id_mem);
  mtos_free(p_sort_buf);
}

/***************************************************************************
函数名：	attach_nit_logic_num
函数描述：	注册可以解LCN 的NIT 函数
入口参数：函数指针

返回值：无
******************************************************************************/
#ifdef LCN_SWITCH_NIT
void attach_nit_logic_num(register_nit_fuc *p_register_nit)
{
  p_register_nit ->parse = parse_nit_with_lcn;
  p_register_nit ->request  = request_nit_with_lcn;
}
#endif
/***************************************************************************
函数名：	attach_bat_logic_num
函数描述：	注册可以解LCN 的BAT 函数
入口参数：函数指针

返回值：无
******************************************************************************/
#ifdef LCN_SWITCH_BAT
void attach_bat_logic_num(register_nit_fuc *p_register_bat)
{
  p_register_bat ->parse = parse_bat_with_lcn;
  p_register_bat ->request  = request_bat_multi_mode_with_lcn;
}
#endif
