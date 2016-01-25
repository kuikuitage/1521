/******************************************************************************/
/******************************************************************************/
#ifndef __MBOOT_H__
#define __MBOOT_H__

/*!
  define the Mboot version (format: x.yy mult by 100)
  */
#define MBOOT_VERSION  100

/*!
  define the max length for each commands
  */
#define CMD_MAX_LEN            256
/*!
  define the max argument count for each commands
  */
#define CMD_MAX_ARGS           10
/*!
  define the max number of supported commands
  */
#define MBOOT_MAX_CMD          20


/*!
  define the mboot command execute function
  */
typedef s32 (*mfunc)(u32 argc, u8 **argv);
/*!
  define the mboot commmad structure
  */
typedef struct
{
  /*!
    cmd name
    */
  u8 *p_name;
  /*!
    cmd usage
    */  
  u8 *p_usage;
  /*!
    cmd help info
    */  
  u8 *p_help;
  /*!
    cmd execute function
    */  
  mfunc func;
}mbcmd_t;


///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///      These functions should be implemented by project
///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*!
  Board relative initialation
  
  \param[in] param1 not defined yet
  \param[in] param2 not defined yet

  \return SUCCESS if OK, else fail
  */
BOOL sys_board_init(u32 param1, u32 param2);

/*!
  Board relative initialation
  
  \param[in] param1 not defined yet
  \param[in] param2 not defined yet

  \return SUCCESS if OK, else fail
  */
BOOL sys_env_init(u32 param1, u32 param2);

/*!
  Add the commands supported to command list
  
  \param[in] None
  */
void mboot_cmd_setup(void);

/*!
  Run the command in sequence
  
  \param[in] None
  */
void mboot_auto_run(void);



///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///       APIs provided by mboot                      
///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*!
  Register the command to command list
  
  \param[in] p_cmd pointer to the command

  \return SUCCESS if OK, else fail
  */
s32 mboot_register_cmd(mbcmd_t *p_cmd);

/*!
  Execute specific command
  
  \param[in] p_cmd the command string to execute

  \return SUCCESS if OK, else fail
  */
s32 mboot_run_cmd(u8 *p_cmd);
#endif //end of __MBOOT_H__
