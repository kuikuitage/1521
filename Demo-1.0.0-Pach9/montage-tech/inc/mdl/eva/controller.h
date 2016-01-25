/******************************************************************************/
/******************************************************************************/
#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

/*!
  private data length
  */
#define CTRL_PRIVAT_DATA (64)

/*!
  the base controller class
  */
typedef struct tag_controller
{
  /*!
    public base class, must be the first member
    */
  FATHER interface_t _interface;
  /*!
    private data buffer
    */
  u8 private_data[CTRL_PRIVAT_DATA];

  /*!
   get the part handle
    */
  void * (*get_part_handle)(handle_t _this);

  // **************************************************************************
  // * Below function are pure virtual function
  // **************************************************************************

    
  /*!
   Virtual. call it when receive event
    */
  VIRTUAL RET_CODE (*on_process_evt)(handle_t _this, os_msg_t *p_msg);

}controller_t;


/*!
  create controller parameter define
  */
typedef struct tag_ctrl_para
{
  /*!
    part handle
    */
    void *p_part_handle;
}ctrl_para_t;

/*!
  create controller define
  */
controller_t * controller_create(controller_t *p_contrl,ctrl_para_t *p_para);

#endif // End for __CONTROLLER_H_

