/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "string.h"

#include "mtos_mem.h"
#include "mtos_printk.h"

#include "testfm_define.h"
#include "testfm_inc.h"
#include "testfm_db.h"

static testfm_p_registry_t g_pTestRegistry = NULL; 
static BOOL g_is_test_running = FALSE;
static testfm_platform_statistic_t g_test_statistic;
static testfm_p_cmd_t g_current_cmd = NULL;

u16 testfm_str_len(const char* strName)
{
  u16 count = 0;
  while('\0' != strName[count])
  {
    count++;
  }
  return count;
}

void testfm_set_platform(u32 platform, const char *platform_name)
{
  int len = testfm_str_len(platform_name)+1;
  g_test_statistic.platform = platform;
  g_test_statistic.platform_name = (char *)mtos_malloc(len);
  memcpy(g_test_statistic.platform_name, platform_name, len);

  g_test_statistic.total = 0;
  g_test_statistic.fail = 0;
}

BOOL testfm_is_running(void)
{
  return g_is_test_running;
}

static void clearup_suite(testfm_p_suite_t p_suite)
{
  testfm_p_cmd_t p_cmd = NULL;

  TESTFM_ASSERT(NULL != p_suite);

  do
  {
    p_cmd = p_suite->pCmd;
    p_suite->pCmd = p_cmd->pNext;

    mtos_free(p_cmd->cmdName);
    mtos_free(p_cmd);
  }
  while(NULL != p_suite->pCmd);
}

static void cleanup_registry(void)
{
  testfm_p_suite_t p_suite = NULL;
  
  TESTFM_ASSERT(NULL != g_pTestRegistry);

  do
  {
    p_suite = g_pTestRegistry->pSuite;
    g_pTestRegistry->pSuite = p_suite->pNext;
    clearup_suite(p_suite);

    mtos_free(p_suite->name);
    mtos_free(p_suite);
  }
  while(NULL != g_pTestRegistry->pSuite);

  mtos_free(g_pTestRegistry);

  g_pTestRegistry = NULL;
}

testfm_error_code_t testfm_init(void)
{
  testfm_error_code_t result = TESTFM_SUCCESS;

  TESTFM_ASSERT(FALSE == testfm_is_running());

  if (NULL != g_pTestRegistry) {
    cleanup_registry();
  }

  g_pTestRegistry = (testfm_p_registry_t)mtos_malloc(sizeof(testfm_registry_t));
  if (NULL == g_pTestRegistry) 
  {
    result = TESTFM_NOMEMORY;
  }
  else
  {
    g_pTestRegistry->pSuite = NULL;
    g_pTestRegistry->numberOfSuites = 0;
    g_pTestRegistry->numberOfCmds = 0;
  }

  return result;
}

testfm_p_registry_t testfm_get_registry()
{
  return g_pTestRegistry;
}


/* suite */
static BOOL suite_exists(const char* szSuiteName)
{
  testfm_p_suite_t p_suite = NULL;

  TESTFM_ASSERT(NULL != g_pTestRegistry);
  TESTFM_ASSERT(NULL != szSuiteName);

  p_suite = g_pTestRegistry->pSuite;
  while (NULL != p_suite)
  {
    if ((NULL != p_suite->name) && (0 == strcmp(szSuiteName, p_suite->name)))
    {
      return TRUE;
    }
    p_suite = p_suite->pNext;
  }

  return FALSE;
}

static testfm_p_suite_t create_suite(const char* strName,
                                                         testfm_initialize_func_t pInit,
                                                         testfm_cleanup_func_t pClean)
{
  testfm_p_suite_t p_suite = (testfm_p_suite_t)mtos_malloc(sizeof(testfm_suite_t));
  int len = testfm_str_len(strName)+1;

  TESTFM_ASSERT(NULL != strName);

  if (NULL != p_suite)
  {
    p_suite->name = (char *)mtos_malloc(len);
    if (NULL != p_suite->name)
    {
	  memcpy(p_suite->name, strName, len);

      p_suite->pInitializeFunc = pInit;
      p_suite->pCleanupFunc = pClean;
      p_suite->pCmd = NULL;
      p_suite->pNext = NULL;
      p_suite->pPrev = NULL;
      p_suite->numberOfCmds = 0;

      p_suite->spec_cmd = PLATFORM_MAX;

    }
    else
    {
      mtos_free(p_suite);
      p_suite = NULL;
    }
  }

  return p_suite;
}
static testfm_p_suite_t create_suite_specific(const char* strName,
                                                         testfm_initialize_func_t pInit,
                                                         testfm_cleanup_func_t pClean, supported_platform_t plat)
{
  testfm_p_suite_t p_suite = (testfm_p_suite_t)mtos_malloc(sizeof(testfm_suite_t));
  int len = testfm_str_len(strName)+1;

  TESTFM_ASSERT(NULL != strName);

  if (NULL != p_suite) 
  {
    p_suite->name = (char *)mtos_malloc(len);
    if (NULL != p_suite->name) 
    {
	  memcpy(p_suite->name, strName, len);
    
      p_suite->pInitializeFunc = pInit;
      p_suite->pCleanupFunc = pClean;
      p_suite->pCmd = NULL;
      p_suite->pNext = NULL;
      p_suite->pPrev = NULL;
      p_suite->numberOfCmds = 0;
      p_suite->spec_cmd = plat;
    }
    else 
    {
      mtos_free(p_suite);
      p_suite = NULL;
    }
  }

  return p_suite;
}
static void insert_suite(testfm_p_suite_t p_suite)
{
  testfm_p_suite_t pCurSuite = NULL;

  TESTFM_ASSERT(NULL != g_pTestRegistry);
  TESTFM_ASSERT(NULL != p_suite);

  pCurSuite = g_pTestRegistry->pSuite;

  TESTFM_ASSERT(pCurSuite != p_suite);

  p_suite->pNext = NULL;

  /* if this is the 1st suite to be added... */
  if (NULL == pCurSuite) 
  {
    g_pTestRegistry->pSuite = p_suite;
    p_suite->pPrev = NULL;
  }
  /* otherwise, add it to the end of the linked list... */
  else
  {
    while (NULL != pCurSuite->pNext)
    {
      pCurSuite = pCurSuite->pNext;
      TESTFM_ASSERT(pCurSuite != p_suite);
    }

    pCurSuite->pNext = p_suite;
    p_suite->pPrev = pCurSuite;
  }
  g_pTestRegistry->numberOfSuites++;
}



testfm_p_suite_t testfm_add_suite(const char *suiteName,
                                                           testfm_initialize_func_t pInit,
                                                          testfm_cleanup_func_t pClean)
{
  testfm_p_suite_t p_suite = NULL;

  TESTFM_ASSERT(FALSE == testfm_is_running());
  TESTFM_ASSERT(NULL != g_pTestRegistry);
  TESTFM_ASSERT(NULL != suiteName);

  //OS_PRINTF("testfm_add_suite: %s\n", suiteName);
  TESTFM_ASSERT(TRUE != suite_exists(suiteName));

  p_suite = create_suite(suiteName, pInit, pClean);

  TESTFM_ASSERT(NULL != p_suite);

  insert_suite(p_suite);

  return p_suite;
}

testfm_p_suite_t testfm_add_suite_specific(const char *suiteName,
                                                           testfm_initialize_func_t pInit,
                                                          testfm_cleanup_func_t pClean,
                                                          supported_platform_t plat)
{
  testfm_p_suite_t p_suite = NULL;

  TESTFM_ASSERT(FALSE == testfm_is_running());
  TESTFM_ASSERT(NULL != g_pTestRegistry);
  TESTFM_ASSERT(NULL != suiteName);

  TESTFM_ASSERT(TRUE != suite_exists(suiteName));

  p_suite = create_suite_specific(suiteName, pInit, pClean, plat);

  TESTFM_ASSERT(NULL != p_suite);

  insert_suite(p_suite);

  return p_suite;
}

/* command */
static BOOL cmd_exists(const char* cmdName)
{
  testfm_p_suite_t p_suite = NULL;
  testfm_p_cmd_t p_cmd = NULL;

  TESTFM_ASSERT(NULL != g_pTestRegistry);
  TESTFM_ASSERT(NULL != cmdName);

  p_suite = g_pTestRegistry->pSuite;
  while (NULL != p_suite)
  {
    p_cmd = p_suite->pCmd;
    while(NULL != p_cmd)
    {
      if((NULL != p_cmd->cmdName) && (0 == strcmp(cmdName, p_cmd->cmdName)))
      {
        return TRUE;
      }
      p_cmd = p_cmd->pNext;
    }
    p_suite = p_suite->pNext;
  }

  return FALSE;
}

static testfm_p_cmd_t create_cmd(const char *cmdName,
                                  testfm_cmd_func_t pCmdFunc,
                                  testfm_help_func_t pHelpFunc,
                                  u32 supported_platform,
                                  BOOL canAutoRun)
{
  testfm_p_cmd_t p_cmd = (testfm_p_cmd_t)mtos_malloc(sizeof(testfm_cmd_t));
  int len = testfm_str_len(cmdName)+1;

  TESTFM_ASSERT(NULL != cmdName);

  if (NULL != p_cmd) 
  {
    p_cmd->cmdName = (char *)mtos_malloc(len);
    if (NULL != p_cmd->cmdName) 
    {
	  memcpy(p_cmd->cmdName, cmdName, len);
      p_cmd->pCmdFunc = pCmdFunc;
      p_cmd->pHelpFunc = pHelpFunc;
      p_cmd->platformList = supported_platform;
      p_cmd->canAutoRun = canAutoRun;

      p_cmd->pNext = NULL;
      p_cmd->pPrev = NULL;
    }
    else 
    {
      mtos_free(p_cmd);
      p_cmd = NULL;
    }
  }

  return p_cmd;
}

static void insert_cmd(testfm_p_suite_t p_suite, testfm_p_cmd_t p_cmd)
{
  testfm_p_cmd_t pCurCmd = NULL;
  
  TESTFM_ASSERT(NULL != p_suite);
  TESTFM_ASSERT(NULL != p_cmd);
  TESTFM_ASSERT(NULL == p_cmd->pNext);
  TESTFM_ASSERT(NULL == p_cmd->pPrev);

  pCurCmd = p_suite->pCmd;

  TESTFM_ASSERT(pCurCmd != p_cmd);

  /* if this is the 1st cmd to be added... */
  if (NULL == pCurCmd) 
  {
    p_suite->pCmd = p_cmd;
    p_cmd->pPrev = NULL;
  }
  /* otherwise, add it to the end of the linked list... */
  else
  {
    while (NULL != pCurCmd->pNext)
    {
      pCurCmd = pCurCmd->pNext;
      TESTFM_ASSERT(pCurCmd != p_cmd);
    }

    pCurCmd->pNext = p_cmd;
    p_cmd->pPrev = pCurCmd;
  }

  p_suite->numberOfCmds++;
}

testfm_p_cmd_t testfm_add_cmd(testfm_p_suite_t p_suite, /* test suite pointer */
                                                 const char *cmdName,            /* command name we will call in shell */
                                                 testfm_cmd_func_t pCmdFunc, /* command function pointer */
                                                 testfm_help_func_t pHelpFunc, /* command help function pointer */
                                                 u32 supported_platform,       /* supported platform list */
                                                 BOOL canAutoRun)                 /* can auto run or not */
{
  testfm_p_cmd_t p_cmd = NULL;
  
  TESTFM_ASSERT(FALSE == testfm_is_running());

  TESTFM_ASSERT(NULL != g_pTestRegistry);
  TESTFM_ASSERT(NULL != p_suite);
  TESTFM_ASSERT(NULL != cmdName);
  TESTFM_ASSERT(NULL != pCmdFunc);
  TESTFM_ASSERT(NULL != pHelpFunc);
  TESTFM_ASSERT(0 != supported_platform);

  TESTFM_ASSERT(TRUE != cmd_exists(cmdName));

  p_cmd = create_cmd(cmdName,
                                  pCmdFunc,
                                  pHelpFunc,
                                  supported_platform,
                                  canAutoRun);

  TESTFM_ASSERT(NULL != p_cmd);

  insert_cmd(p_suite, p_cmd);
  g_pTestRegistry->numberOfCmds++;

  return p_cmd;  
}

testfm_error_code_t run_single_cmd(testfm_p_cmd_t pCmd)
{
  testfm_error_code_t ret = TESTFM_SUCCESS;
  MT_ASSERT(NULL != pCmd);

  if (NULL != pCmd->pCmdFunc)
  {
    (*pCmd->pCmdFunc)();       
  }

  return ret;
}


BOOL testfm_find_cmd(char *cmd, testfm_p_suite_t *pSuite, testfm_p_cmd_t *pCmd)
{
  testfm_p_suite_t pSuiteCur = NULL;
  testfm_p_cmd_t pCmdCur = NULL;

  MT_ASSERT(NULL != cmd);
  MT_ASSERT(NULL != g_pTestRegistry);

  pSuiteCur = g_pTestRegistry->pSuite;
  while(pSuiteCur != NULL)
  {
    pCmdCur = pSuiteCur->pCmd;
    
    while(pCmdCur != NULL)
    {
      if(0 == strcmp(pCmdCur->cmdName, cmd))
        break;
      
      pCmdCur = pCmdCur->pNext;
    }
    
    if(pCmdCur != NULL)
      break;
    
    pSuiteCur = pSuiteCur->pNext;
  }
  
  if((pSuiteCur != NULL) && (pCmdCur != NULL))
  {
    *pSuite = pSuiteCur;
    *pCmd = pCmdCur;
    return TRUE;
  }
  
  return FALSE;
}

testfm_error_code_t testfm_run_cmd(testfm_p_suite_t pSuite, testfm_p_cmd_t pCmd)
{
  testfm_error_code_t result = TESTFM_SUCCESS;
  testfm_p_suite_t p_suite = NULL;
  testfm_p_cmd_t p_cmd = NULL;

  if(!(g_test_statistic.platform & pCmd->platformList))
  {
    result = TESTFM_PLATFORMNOTSUPPORT;
  }
  else
  {
    if (NULL == pSuite) {
      result = TESTFM_NOSUITE;
    }
    else if (NULL == pCmd) {
      result = TESTFM_NOCMD;
    }
    else if ((NULL == pCmd->cmdName) || (FALSE == testfm_find_cmd(pCmd->cmdName, &p_suite, &p_cmd)))
    {
      result = TESTFM_NO_CMDNAME;
    }
    else 
    {
      /* test run is starting - set flag */
      g_is_test_running = TRUE;


      if ((NULL != pSuite->pInitializeFunc) && (0 != (*pSuite->pInitializeFunc)()))
      {
        result = TESTFM_SINIT_FAILED;
        g_is_test_running = FALSE;
      }
      /* reach here if no suite initialization, or if it succeeded */
      else
      {
        g_current_cmd = pCmd;
        result = run_single_cmd(pCmd);
        g_current_cmd = NULL;

        if ((NULL != pSuite->pCleanupFunc) && (0 != (*pSuite->pCleanupFunc)())) {
          result = TESTFM_SCLEAN_FAILED;
        }

        g_is_test_running = FALSE;

      }
    }
  }

  return result;
}

void private_setresult(testfm_result_t result, u32 errorcode)
{
  if(g_current_cmd != NULL)
    OS_PRINTF("CMD: %s\n", g_current_cmd->cmdName);
  
  if(result == (testfm_result_t)TESTFM_SUCCESS)
  {
    OS_PRINTF("RET: success\n");
  }
  else
  {
    OS_PRINTF("RET: fail\n");
    OS_PRINTF("ENO: %x\n", errorcode);
  }
}
