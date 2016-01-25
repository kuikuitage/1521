/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2012).
*                 All rights reserved.
* Owner:       Dave Bie <dave.bie@montage-tech.com>
****************************************************************************/

#ifndef __LOGIC_NUM_PROC_H__
#define __LOGIC_NUM_PROC_H__

/*!
  Define the max logic channel number
  */
#define MAX_LCN_NUM 255

/*!
  Define start logic number
  */
#define DVB_LOGIC_START_NUMBER   1001

/*!
  Data structure for logical channel desc
  */
typedef struct
{
  /*!
    network id
    */
  u16 network_id;
  /*!
    stream id
    */
  u16 ts_id;
  /*!
    service id
    */
  u16 service_id;
  /*!
    logical channel number
    */
  u16 logical_channel_id;
  /*!
    visible flag
    */
  BOOL is_visible_flag;
}logic_channel_tag;

/*!
 * logic number sort struct
 */
typedef struct
{
 /*!
  progame node id.
  */
  u16 node_id;
 /*!
  logic number in stream
  */
  u16 logic_num;
}prog_logic_num_sort_t;


/*!
  structure of logic channel number
  */
typedef struct
{
  u8 log_ch_num;

  logic_channel_tag *p_logic_channel;
}logic_channel;

/*!
  Data structure for register nit function
  */
typedef struct
{
  /*!
		The callback of parse nit function
	*/
	void (*parse)(handle_t handle, dvb_section_t *p_sec);
   /*!
		The callback of request nit function
	*/
	void (*request)(dvb_section_t *p_sec, u32 table_id, u32 para);
}register_nit_fuc;

void do_search_logic_num_process(logic_channel log_ch_data);
void manual_search_logic_num_process(void);
void sort_by_logic_num(u8 view_id);
void attach_nit_logic_num(register_nit_fuc *p_register_nit);
void attach_bat_logic_num(register_nit_fuc *p_register_nit);
#endif

