/******************************************************************************/
/******************************************************************************/
#ifndef __AP_SCAN_PRIV_H_
#define __AP_SCAN_PRIV_H_

/*!
  Find Tp result
  */
typedef enum
{
  /*!
    Find TP success
    */
  FIND_SUC = 0,
  /*!
    Find TP fail, waiting continue
    */
  FIND_BUSY,
  /*!
    Find TP Done, no TP more
    */
  FIND_DONE
}find_tp_ret_t;

/*!
  compare tp result.
  */
typedef enum
{
 /*!
   the same tp, give up.
   */
  SAME_TPS = 0,
 /*!
   the different tp, continue.
   */
  DIFFERENT_TPS,
 /*!
  over lapped tp, give up.
   */
  OVERLAPPED_TPS,
}tp_compare_result_t;



/*!
  Implementation policy of application scan
  */
typedef struct
{
  /*!
    Implementation policy for Message proc
    */
  RET_CODE (*msg_proc)(void *p_data, os_msg_t *p_msg);
  /*!
    Implementation policy for Pre-scan function
    */
  find_tp_ret_t (*get_tp_info)(void *p_data, nc_channel_info_t *p_channel);
  /*!
    Implementation policy for tp finished
    */
  RET_CODE (*on_tp_done)(void *p_data, void *p_tp_info);
    /*!
    Get buffer for runing time
    */
  void * (*require_runingtime_buffer)(void *p_data, u32 size);
  /*!
    destroy current policy
    */
  void (*destroy_policy)(void *p_policy);
  /*!
    request extend table enable
    */
  BOOL (*require_extend_table)(void *p_policy, u8 table_id);
  /*!
    get scramble flag origin
    */
  scan_scramble_origin_t (*get_scan_scramble_origin)(void *p_policy);
    /*!
    scan module performance track
    */
  void (*performance)(void *p_data, u32 check, u32 context1, u32 context2);
  /*!
    check repeat tp
    */
  BOOL (*repeat_tp_check)(void *p_data, nc_channel_info_t *p_channel);
  /*!
    blind scan version
    */
  u8 (*get_bouquet_id_list)(void *p_data, u16 *p_list);
    /*!
   get pg sid from sdt flag
    */
  BOOL (*is_no_pg_filter)(void *p_policy);
  
  /*!
    Private data for application scan
    */
  void *p_priv_data;
}scan_policy_t;

#endif  //__AP_SCAN_PRIV_H_
