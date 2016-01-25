
#ifndef _UART_WIN32_H_
#define _UART_WIN32_H_

#include "sys_types.h"

/*!
  The COM parameters 
  */
typedef struct  //SComm Parameter
{
  /*!
    COM index
    */
  u8 bPort;
  /*!
    baud rate
    */
  u8 bBaudRate;
  /*!
    the COM byte size
    */
  u8 bByteSize;
  /*!
    parity
    */
  u8 bParity;
  /*!
    parity check enable
    */
  u8 fParity;
  /*!
    stop bits
    */
  u8 bStopBits;
}COM_PARAM_ST;

/*!
  initialize the COM
  
  \param[in] id the COM index 
  */
void uartwin_init(u32 id);
/*!
  Send out data sequence
  
  \param[in] p_data the pointer to sending data
  \param[in] len the length to send

  \return 0 if success
  */
s32 uartwin_write(u8* p_data, u32 len);
/*!
  Send out data sequence
  
  \param[in] ch pointer to received byte
  \param[in] timeout the time out value

  \return 0 if success
  */
s32 uartwin_read_byte(u8 *ch,u32 timeout);
/*!
  flush COM
  */
void uartwin_flush();

#endif

