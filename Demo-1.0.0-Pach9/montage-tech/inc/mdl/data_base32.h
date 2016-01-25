/******************************************************************************/
/******************************************************************************/
#ifndef __DATA_BASE32_H_
#define __DATA_BASE32_H_

/*!
  \file data_base32.h

  This file provides the API to get the proc handle of data base 32.
  In this version database, basic item type in database is u32, the 
  max mark bits supported is 16 and the max virtual view supported is 6
  */
/*!
  Max mask bits from bit11 to 14 supported in this version
  */
#define MAX_MARK_NUM  (16)
/*!
  Max virtual view number supported
  */
#define MAX_VIRTUAL_VIEW_NUM  (6)
/*!
  Basic item type is u16
  */
typedef u32 item_type;

/*!
  db_get_db_32_handle
  */
void *db_get_db_32_handle(void);

#endif //__DATA_BASE_32_H_


