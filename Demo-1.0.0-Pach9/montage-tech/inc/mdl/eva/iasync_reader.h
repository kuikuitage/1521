/******************************************************************************/
/******************************************************************************/
#ifndef __IASYNC_READER_H_
#define __IASYNC_READER_H_

/*
  The iasync_reader interface performs an asynchronous data request 
  on a filter.
  
  This interrace is exposed by output pins that perform asynchronous 
  read operations. The interface is used by the input pin on the 
  downstream filter. Applications do not use this interface. 

  Filter developers: Implement this interface if your output pin delivers 
  data in the form of a byte stream and supports the pull model.
*/

/*!
  private data length
  */
#define IASYNC_READER_PRIVAT_DATA (64)

/*!
  async reader interface define
  */
typedef struct tag_iasync_reader
{
  /*!
    private data buffer
    */
  u8 private_data[IASYNC_READER_PRIVAT_DATA];

  /*!
    The get unprocess sample.
    \param[in] _this this point.
    \param[in] p_sample unprocess sample. 
      
    \return return the result
  */
  PRIVATE RET_CODE (*get_unprocess_sample)(handle_t _this,void *p_sample);

  /*!
    The Request method queues an asynchronous request for data.
    \param[in] _this this point.
    \param[in] p_format media format provided by the caller. 
    \param[in] timeout waiting timed out. 
    \param[in] position offset to stream header. 
    \param[in] context Specifies an arbitrary value that is returned 
      when the request completes
      
    \return return the request done
  */
  RET_CODE (*request)(handle_t _this, media_format_t *p_format, u32 read_len,
                          u32 timeout, u32 position, u32 context);

  /*!
    The WaitForNext method waits for the next pending read request to complete.
    \param[in] _this this point.
    \param[in] p_sample media sample provided by the caller. 
    \param[in] time_out Specifies a time-out in milliseconds.
               use the value 0 to wait indefinitely. 
    \param[in] context Specifies an arbitrary value that is returned 
      when the request completes

    \return return the SUCCESS if read done
  */
  //RET_CODE (*wait_next)(handle_t _this, media_sample_t *p_sample, u32 time_out, u32 context);
}iasync_reader_t;

#endif // End for __IASYNC_READER_H_

