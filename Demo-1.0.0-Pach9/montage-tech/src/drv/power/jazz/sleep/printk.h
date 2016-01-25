/******************************************************************************/
/******************************************************************************/

#ifndef __PRINTK_H__
#define __PRINTK_H__



/*!
  comments
  */
typedef char    *ck_va_list;
/*!
  comments
  */
typedef unsigned int  CK_NATIVE_INT;
/*!
  comments
  */
typedef unsigned int  ck_size_t;

/*!
  pad with zero
  */
#define ZEROPAD   0x01  
/*!
  unsigned/signed long
  */
#define SIGN    0x02    
/*!
  show plus
  */
#define PLUS    0x04    
/*!
  space if plus
  */
#define SPACE    0x08   
/*!
  left justified  
  */
#define LEFT    0x10    
/*!
  0x
  */
#define SPECIAL    0x20 
/*!
  use 'ABCDEF' instead of 'abcdef'
  */
#define LARGE    0x40

/*!
  comments
  */
#define CK_ISDIGIT(c)      ((c >= '0') && (c <= '9'))
/*!
  comments
  */
#define CK_DO_DIV(n,base)    ({ long __res;            \
             __res = ((unsigned long) n) % (unsigned) base;  \
             n = ((unsigned long) n) / (unsigned) base;    \
             __res; })
/*!
  Module Internal Routine
  */
int  printk(const char *fmt, ...);

/*!
  Module API Prototype  
*/
void  register_console(int (*Console)(char));
/*!
  comments
  */
int  sprintf(char *s, const char *format, ...);

/*!
  comments
  */
#define  PRINT(x)  do { printk x ; } while (0)

#endif //__PRINTK_H__
