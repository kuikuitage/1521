/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include <stdarg.h>
#include "string.h"

#include "mtos_printk.h"
#include "mtos_task.h"

#include "testfm_define.h"
#include "testfm_inc.h"
#include "testfm_db.h"
#include "testfm_op.h"

#ifdef WIN32
#include "drv_dev.h"
#include "uio.h"
#include "windows.h"
#endif
#define ENABLE_CMD_HISTORY

#ifdef ENABLE_CMD_HISTORY
#define MAX_SAVED_CMD 10
#define KEY_PRE    '['
#define KEY_NEXT  ']'
#endif

static char parameter_str[256] = {0,};

#ifdef ENABLE_CMD_HISTORY
static char command_history[MAX_SAVED_CMD][256];
static int current_cmd = 0;
static int cmd_num = 0;
static int cmd_point = 0;
#endif

#define KEY_BACKSPACE 8
#define DEBUG_PRINT OS_PRINTK

#ifdef ENABLE_CMD_HISTORY
int num_range_loop(int begin, int end, int num)
{
  if(num > end)
    return begin;

  if(num < begin)
    return end;

  return num;
}

void save_command(char *cmd)
{
  if(cmd != NULL)
  {
    memcpy(command_history[current_cmd], cmd, 256);
    
    current_cmd++;
    current_cmd = num_range_loop(0, (MAX_SAVED_CMD - 1), current_cmd);
    
    cmd_point = current_cmd;
    cmd_num++;
    if(cmd_num > MAX_SAVED_CMD)
    {
      cmd_num = MAX_SAVED_CMD;
    }
  }
  else
  {
    MT_ASSERT(0);
  }
}

char *get_pre_command(void)
{
  int n;
  if(cmd_point <= current_cmd)
  {
    n = current_cmd - cmd_point;
  }
  else
  {
    n = current_cmd + MAX_SAVED_CMD - cmd_point;
  }

  if(n < cmd_num)
  {
    cmd_point = num_range_loop(0, (MAX_SAVED_CMD - 1), (cmd_point - 1));
    return command_history[cmd_point];
  }
  else
  {
    //do nothing
    return NULL;
  }
}

char *get_next_command(void)
{
  int n;
  if(cmd_point <= current_cmd)
  {
    n = current_cmd - cmd_point;
  }
  else
  {
    n = current_cmd + MAX_SAVED_CMD - cmd_point;
  }

  if(n > 1)
  {
    cmd_point = num_range_loop(0, (MAX_SAVED_CMD - 1), (cmd_point + 1));
    return command_history[cmd_point];
  }
  else
  {
    //do nothing
    return NULL;
  }
}
#endif

#ifndef WIN32
extern void OS_PutChar(u8 c);
extern u8 OS_GetChar(void);
extern u8 OS_GetChar_Sleep(void);

#else
u8 Win_GetChar(void)
{
  u8 res = 0;
  uio_device_t *p_uio = NULL;
  u16 code = 0;

  p_uio = (uio_device_t*)dev_find_identifier(NULL,
                                             DEV_IDT_TYPE,
                                             SYS_DEV_TYPE_UIO);
    
  while(1)
  {
    if(uio_get_code(p_uio, &code) == SUCCESS)
    {
      break;
    }
    Sleep(100);
  }

  return (u8)code;
}

void Win_PutChar(u8 c)
{
  OS_PRINTF("%c", c);
}
#endif

void Console_PutChar(u8 c)
{
#ifdef WIN32
     Win_PutChar(c);
#else
      OS_PutChar(c);
#endif
}

u8 Console_GetChar(void)
{
#ifdef WIN32
  return Win_GetChar();
#else
  return OS_GetChar_Sleep();
#endif
}

static u8 console_get_char(void)
{
  u8 res = 0;

  res = Console_GetChar();

  if(10 != res && 13 != res) 
  //if(13 != res) 
  {
#ifdef ENABLE_CMD_HISTORY
    if((KEY_BACKSPACE != res) && (KEY_PRE != res) && (KEY_NEXT != res))
#else
    if(KEY_BACKSPACE != res)
#endif
    {
      Console_PutChar(res);
    }
  }
  else 
  {
    Console_PutChar(0x0D);
    Console_PutChar(0x0A);
  }
  
  mtos_task_sleep(1);
  
  return res;
}


static void get_command(char *p_buffer)
{
  unsigned char input = 0;
  int pos = 0;
  memset(p_buffer, 0, 256);

  do
  {
    input = console_get_char();
    //it's for secureCRT.
    //if(input == 10)
    //  continue;
    
    // add support for backspace key
    if(input == KEY_BACKSPACE) 
    {
      if(pos >= 1)
      {
        p_buffer[--pos] = '\0';
        Console_PutChar(input);
      }
    }
#ifdef ENABLE_CMD_HISTORY
    else if((input == KEY_PRE) || (input == KEY_NEXT))
    {
      char *tmp = NULL;
      
      if(input == KEY_PRE)
        tmp = get_pre_command();
      else
        tmp = get_next_command();
      if(tmp != NULL)
      {
        memset(p_buffer, 0, 256);
        memcpy(p_buffer, tmp, 256);

        for(; pos > 0; pos--)
          Console_PutChar(KEY_BACKSPACE);
        
        pos = strlen(p_buffer);
        OS_PRINTK("%s", p_buffer);
      }
      else
      {
        memset(p_buffer, 0, 256);
        for(; pos > 0; pos--)
          Console_PutChar(KEY_BACKSPACE);
        pos = 0;
      }
    }
#endif
    else
    {
      if(pos >= CU_MAX_STR_LENGTH)
      {
        OS_PRINTK("Stop. Exceed the input buffer!!!\n");
      }
      else
      {
          p_buffer[pos++] = input;
      }
    }
    TESTFM_ASSERT(pos != CU_MAX_STR_LENGTH);
  } while(input != 10 && input != 13);
  //} while(input != 13);

  p_buffer[pos-1] = '\0';
}

void testfm_console_get_parameter(char *p_parameter, char spliter)
{
#ifndef WIN32
  MT_ASSERT(NULL != p_parameter);
#endif

  int i = 0, len = 0, len1 = 0;

  len = strlen(parameter_str);

  if(len == 0)
  {
//    MT_ASSERT(NULL != p_parameter_name);
//    OS_PRINTF("\nfirst: %s: ", p_parameter_name);
//    get_command(p_parameter);
//    DEBUG_PRINT("3, %s\n", p_parameter);
    p_parameter[0] = 0;
    return;
  }
  
  for(i = 0; i < len; i++)
  {
    if(parameter_str[i] != spliter)
    {
      p_parameter[i] = parameter_str[i];
    }
    else
    {
      //DEBUG_PRINT("1, %s\n", parameter_str);
      len1 = strlen(&parameter_str[i+1]);
      memcpy(parameter_str, &parameter_str[i+1], len1);
      parameter_str[len1] = 0;
      //DEBUG_PRINT("2, %s\n", parameter_str);
      break;
    }
  }
  //DEBUG_PRINT("i = %d, %s, str len = %d\n", i, parameter_str, strlen(parameter_str));
  if(i != 0)
  {
    p_parameter[i] = 0;
    if(i == len)
    {
      memset(parameter_str, 0, 256);
    }
  }
  else
  {
//    MT_ASSERT(NULL != p_parameter_name);
//    OS_PRINTF("\nsecond: %s: ", p_parameter_name);
//    get_command(p_parameter);
    p_parameter[0] = 0;
  }
  
  //DEBUG_PRINT("3, %s\n", p_parameter);
}

void testfm_console_get_parameter_str(char *p_parameter_str)
{
  int len = 0;
  MT_ASSERT(NULL != p_parameter_str);
  len = strlen(parameter_str);
  memcpy(p_parameter_str, parameter_str, len);
  p_parameter_str[len] = 0;
}

void testfm_set_parameter_str(char *p_parameter_str)
{
  int len = 0;
  MT_ASSERT(NULL != p_parameter_str);
  len = strlen(p_parameter_str);
  MT_ASSERT(256 >= len);
  memcpy(parameter_str, p_parameter_str, len);
  parameter_str[len] = 0;
}

static void console_command_mode_run(void)
{
  char szTemp[256];
  char *str = NULL;
  int len = 0, i = 0;


  OS_PRINTF("\nEnter command mode. 'help' to show command list(non specific)\n");
  OS_PRINTF("\nEnter command mode. 'spechelp' to show specific command list\n");
  
  while (1)
  {
    if(get_console_mode() == DOWNLOAD_MODE)
    {
      OS_PRINTF("> ");
    }
    else
    {
      OS_PRINTF("Cmd > ");
    }
    get_command(szTemp);
#ifdef ENABLE_CMD_HISTORY
    save_command(szTemp);
#endif
    len = strlen(szTemp);

    if(len <= 0)
    {
      // do nothing
      continue;
    }

    // trim the space at the begin of the input
    for(i = 0; i < len; i++)
    {
      if(szTemp[i] != ' ')
      {
        str = &szTemp[i];
        break;
      }
    }

    // trim the space at the end of the input
    for(i = len-1; i >= 0; i--)
    {
      if(szTemp[i] != ' ')
      {
        szTemp[i+1] = 0;
        break;
      }
    }
    
    len = strlen(str);

    if(len <= 0)
    {
      continue;
      // do nothing
    }
    else
    {
      int i = 0, j =0;
      int showhelp = 0;
      testfm_p_suite_t pSuite = NULL;
      testfm_p_cmd_t pTest = NULL;
      testfm_error_code_t res = TESTFM_SUCCESS;


      if(!(((str[0] >= 'a') && (str[0] <= 'z')) || ((str[0] >= 'A') && (str[0] <= 'Z'))))
      {
        OS_PRINTF("CMD should start with 'a-z' and 'A-Z'.\n");
        continue;
      }

      for(i = 0; i < len-1; i++)
      {
        if((j == 0) && (str[i] == ' '))
        {
          j = i;
          memset(parameter_str, 0, 256);
          memcpy(parameter_str, &str[i+1], strlen(&str[i+1]));
        }

        if((str[i] == '-') && (str[i+1]  == 'h') && (str[i-1] == ' ') 
             && (((i+2) == len) || (str[i+2] == ' ')))
        {
          showhelp = 1;
          break;
        }
      }
      if(j != 0)
        str[j] = 0;

      if(0 == memcmp(str, "help", 4))
      {
        // list all non specific command
        cmd_help();
        continue;
      }
      if(0 == memcmp(str, "wizardshelp", 11))
      {
    	  // list all specific command
    	  cmd_help_specific(PLATFORM_WIZARD);
    	  continue;
      }
      if(0 == memcmp(str, "magichelp", 9))
           {
         	  // list all specific command
         	  cmd_help_specific(PLATFORM_MAGIC);
         	  continue;
           }
      if(0 == memcmp(str, "warriorshelp", 12))
           {
         	  // list all specific command
         	  cmd_help_specific(PLATFORM_WARRIORS);
         	  continue;
           }
      if(0 == memcmp(str, "anyhelp", 7))
           {
         	  // list all specific command
         	  cmd_help_specific(PLATFORM_ANY);
         	  continue;
           }

      if(0 == memcmp(str, "read", 4))
      {
        cmd_read();
        continue;
      }

      if(0 == memcmp(str, "write", 5))
      {
        //cmd_write();
        continue;
      }

      if(get_console_mode() == DOWNLOAD_MODE)
      {
        if(0 == memcmp(str, "download", 8))
        {
          cmd_download();
          continue;
        }

        if(0 == memcmp(str, "go", 2))
        {
          cmd_go();
          continue;
        }
        
        if(0 == memcmp(str, "reboot", 6))
        {
          cmd_reboot();
          continue;
        }

        OS_PRINTF("'%s' is not recognized as an internal or external command.\n", str);
        continue;
      }
      

      if(0 == memcmp(str, "exit", 4))
      {
        cmd_exit();
        continue;
      }

      if(0 == memcmp(str, "runall", 6))
      {
        cmd_runall();
        continue;
      }

      if(0 == memcmp(str, "statistic", 9))
      {
        cmd_statistic();
        continue;
      }

      if(0 == memcmp(str, "autorun", 7))
      {
        //cmd_autorun();
        continue;
      }

       // add the reboot cmd for white box test
      if(0 == memcmp(str, "reboot", 6))
      {
        cmd_reboot();
        continue;
      }

      if(testfm_find_cmd(str, &pSuite, &pTest) == FALSE)
      {
        OS_PRINTF("'%s' is not recognized as an internal or external command.\n", str);
      }
      else
      {
        if(showhelp == 1)
        {
          pTest->pHelpFunc();
        }
        else
        {
          //OS_PRINTF(", name: %s\n", pTest->pName);
          res = testfm_run_cmd(pSuite, pTest);
          if(res != TESTFM_SUCCESS)
            OS_PRINTF("error code: %d\n", res);
        }
      }
 
    }
  }
}


void testfm_console_run(void)
{
  console_command_mode_run();
}


void testfm_handle_autotest_cmd_data(u8 *p_cmd)
{
  //BOOL ret = TRUE;
  
  char *str = NULL;
  
  int showhelp = 0;
  testfm_p_suite_t pSuite = NULL;
  testfm_p_cmd_t pTest = NULL;
  testfm_error_code_t res = TESTFM_SUCCESS;

  str = (char *)p_cmd;
  
  if(testfm_find_cmd(str, &pSuite, &pTest) == FALSE)
  {
    OS_PRINTF("'%s' is not recognized as an internal or external command.\n", str);
  }
  else
  {
    if(showhelp == 1)
    {
      pTest->pHelpFunc();
    }
    else
    {
      //OS_PRINTF(", name: %s\n", pTest->pName);
      res = testfm_run_cmd(pSuite, pTest);
      if(res != TESTFM_SUCCESS)
      {
        OS_PRINTF("error code: %d\n", res);
        //ret = FALSE;
      }
    }
  }
}
