/******************************************************************************/
/******************************************************************************/
#ifndef __MTOS_PRINTK_H__
#define __MTOS_PRINTK_H__

/*!
    struct for printk_pipe_info
  */
typedef struct printk_pipe_info
{
  /*!
      pipe  message
    */   
  char *p_Datapipe;
  /*!
      read pointer
    */     
  int ReadPt;
  /*!
      write pointer
    */   
  int WritePt;
  /*!
      this pipe depth
    */
  int pipe_depth;

}printk_pipe_info_t;
/*!
    struct for pipe
  */
typedef struct
{
  /*!
      this pipe ap_pipe
    */
  printk_pipe_info_t ap_pipe;
    /*!
      this pipe av_pipe
    */
  printk_pipe_info_t av_pipe;
  
}dual_printk_t;
/*!
  Compatible with print usage
  */
#define OS_PRINTK mtos_printk

/*!
  Register the character output function
  
  \param[in] p_printchar the character output function
  */
void mtos_register_putchar(s32 (*p_printchar)(u8, u8));

/*!
  Register the character input function
  
  \param[in] p_printchar the character input function
  */
void mtos_register_getchar(s32 (*p_getchar)(u8, u8 *, u32));
/*!
  MTOS print function, please refer standard libc for usage
  
  \param[in] p_format the variable input, please refer to libc

  \return 0
  */
int  mtos_printk(const char *p_format, ...);
/*!
  MTOS print function, please refer standard libc for usage

  \param[in] p_format the variable input, please refer to libc

  \return 0
  */
int  mtos_printk_1(const char *p_format, ...);
/*!
  close printk
  */
void mtos_close_printk(void);

/*!
  open printk
  */
void mtos_open_printk(void);

#endif //__MTOS_PRINTK_H__
