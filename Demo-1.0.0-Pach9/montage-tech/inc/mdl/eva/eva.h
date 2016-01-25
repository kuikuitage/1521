/******************************************************************************/
/******************************************************************************/
#ifndef __EVA_H_
#define __EVA_H_


/*!
  eva error debug out
  */
#define EVA_DBG OS_PRINTF

/*!
  eva error print out
  */
#define EVA_WARN OS_PRINTF

/*!
  eva error print out
  */
#define EVA_ERROR OS_PRINTF("EVA ERROR. func: %s, line: %d\n", __FUNCTION__, __LINE__); OS_PRINTF

/*!
  eva print out level
  */
typedef enum 
{
  /*!
    eva print out from debug level
    */
  EVA_DEBUG_L,
  /*!
    eva print out from warning level
    */
  EVA_WARNING_L,
  /*!
    eva print out from error level
    */
  EVA_ERROR_L,
  /*!
    eva has not any print out
    */
  EVA_NONE_L,
}eva_output_level;


/*!
  create eva system parameter define
  */
typedef struct tag_eva_init_para
{
  /*!
    print ou level. 
    */
  eva_output_level debug_level;
  /*!
    eva system task priority
    */
  u32 eva_sys_task_prio;
}eva_init_para_t;


/*!
  init eva system

  \param[in] p_para /see eva_init_para_t.

  \return return SUCCESS
  */
RET_CODE eva_init(eva_init_para_t *p_para);


/*!
  send a system message. 
  */
RET_CODE eva_send_sys_msg(handle_t _this, interface_t *p_interface,
                          os_msg_t *p_msg);

/*!
  send a error message. 
  */
RET_CODE eva_send_error_msg(handle_t _this, os_msg_t *p_msg);


/*!
  get eva version
  
  \return return version
  */
char *eva_get_version(void);

#endif // End for __EVA_H_

