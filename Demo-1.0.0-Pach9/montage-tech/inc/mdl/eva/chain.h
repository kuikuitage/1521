/******************************************************************************/
/******************************************************************************/
#ifndef __CHAIN_H_
#define __CHAIN_H_


/*!
  private data length
  */
#define CHAIN_PRIVAT_DATA (640)

/*!
  state
  */
typedef enum
{
  /*!
    error state
    */
  CHAIN_UNCREATED,
  /*!
    created
    */
  CHAIN_CREATED,
  /*!
    opened
    */
  CHAIN_OPENED,
  /*!
    running
    */
  CHAIN_RUNNING,
  /*!
    paused
    */
  CHAIN_PAUSED,
  /*!
    stoped
    */
  CHAIN_STOPPED,
  /*!
    closed
    */
  CHAIN_CLOSED,
}chain_state_t;

/*!
  the base chain class
  */
typedef struct tag_chain
{
  /*!
    public base class, must be the first member
    */
  FATHER interface_t _interface;
  
  /*!
    private data buffer
    */
  u8 private_data[CHAIN_PRIVAT_DATA];
  
  /*!
    get chain's state
    */
  chain_state_t (*get_state)(handle_t _this);

  /*!
    get chain's prior
    */
  PRIVATE u32 (*get_priority)(handle_t _this);

  /*!
    dispatch some thing
    */
  PRIVATE RET_CODE (*dispatch)(handle_t _this, void *p_mail);
  
  /*!
    add filter to chain
    */
  RET_CODE (*add_filter)(handle_t _this, ifilter_t *p_filter, char *p_name);
  
  /*!
    remove filter from chain
    */
  RET_CODE (*remove_filter)(handle_t _this, ifilter_t *p_filter);
 
  /*!
    connect pin
    */
  RET_CODE (*connect)(handle_t _this, ipin_t *p_output,
                  ipin_t *p_input, media_format_t *p_format);
  /*!
    This method intelligent connects all of the filters of this chain.
    */
  RET_CODE (*build)(handle_t _this);
  /*!
    purge. 
    */
  void (*purge)(handle_t _this);

  /*!
    open chain.
    */
  RET_CODE (*open)(handle_t _this);

  /*!
    start chain.
    */
  RET_CODE (*start)(handle_t _this);

  /*!
    pause chain.
    */
  RET_CODE (*pause)(handle_t _this);

  /*!
    resume chain.
    */
  RET_CODE (*resume)(handle_t _this);

  /*!
    stop chain.
    */
  RET_CODE (*stop)(handle_t _this);

  /*!
    close chain.
    */
  RET_CODE (*close)(handle_t _this);

  // **************************************************************************
  // * Below function are pure virtual function
  // **************************************************************************
  
  /*!
    Notifies the chain that it is opend now. Virtual. 
    */
  VIRTUAL RET_CODE (*on_open)(handle_t _this);

  /*!
    Notifies the chain that it is runing now. Virtual. 
    */
  VIRTUAL RET_CODE (*on_start)(handle_t _this);

  /*!
    Notifies the chain that it is paused now. Virtual. 
    */
  VIRTUAL RET_CODE (*on_pause)(handle_t _this);

  /*!
    Notifies the chain that it is resume now. Virtual. 
    */
  VIRTUAL RET_CODE (*on_resume)(handle_t _this);

  /*!
    Notifies the chain that it is stop now. Virtual. 
    */
  VIRTUAL RET_CODE (*on_stop)(handle_t _this);

  /*!
    Notifies the chain that it is closed now. Virtual. 
    */
  VIRTUAL RET_CODE (*on_close)(handle_t _this);
}chain_t;

/*!
  create base chain parameter define
  */
typedef struct tag_chain_para
{
  /*!
    Pointer to the controller that use it. 
    */
  interface_t *p_owner;
  /*!
    name 
    */
  char *p_name;

  /*!
    data stream task priority on this chain.
    */
  u32 priority;
  
  /*!
    task size
    */
  u32 stk_size;
}chain_para_t;

/*!
  create a chain instance

  \param[in] p_para init parameter, it can't be NULL

  \return return the instance of chain
  */
chain_t * chain_create(chain_para_t *p_para);

#endif // End for __CHAIN_H_

