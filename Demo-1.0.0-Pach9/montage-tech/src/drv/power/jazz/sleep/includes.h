/******************************************************************************/
/******************************************************************************/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__

/*!
  Basic types: Know Exactly How Your CPU/Compiler Process Variable
  */
typedef unsigned char    BOOLEAN;
/*!
  comments
  */
typedef unsigned char    INT8U;
/*!
  comments
  */
typedef signed   char    INT8S;
/*!
  comments
  */
typedef unsigned short int  INT16U;
/*!
  comments
  */
typedef signed   short int  INT16S;
/*!
  comments
  */
typedef unsigned long  int  INT32U;
/*!
  comments
  */
typedef signed   long  int  INT32S;
/*!
  comments
  */
typedef float        FP32;
/*!
  comments
  */
typedef double        FP64;

/*!
  Extra types: Compatiable with User Defined Types
  */
#define TU1      INT8U
/*!
  Extra types: Compatiable with User Defined Types
  */
#define TU2      INT16U
/*!
  Extra types: Compatiable with User Defined Types
  */
#define TU4      INT32U
/*!
  Extra types: Compatiable with User Defined Types
  */
#define TUI      unsigned int
/*!
  Extra types: Compatiable with User Defined Types
  */
#define TS1      INT8S
/*!
  Extra types: Compatiable with User Defined Types
  */
#define TS2      INT16S
/*!
  Extra types: Compatiable with User Defined Types
  */
#define TS4      INT32S
/*!
  Extra types: Compatiable with User Defined Types
  */
#define TSI      signed   int
/*!
  Extra types: Compatiable with User Defined Types
  */
#define TF4      FP32
/*!
  Extra types: Compatiable with User Defined Types
  */
#define TD8      FP64

#endif //__INCLUDES_H__

