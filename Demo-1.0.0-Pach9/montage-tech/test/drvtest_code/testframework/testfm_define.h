/******************************************************************************/
/******************************************************************************/

#ifndef __TESTFM_DEFINE_H__
#define __TESTFM_DEFINE_H__

/** Maximim length of string. */
#define CU_MAX_STR_LENGTH	256

typedef enum {
  /* basic errors */
  TESTFM_SUCCESS           = 0,  /**< No error condition. */
  TESTFM_NOMEMORY          = 1,  /**< Memory allocation failed. */

  /* Test Registry Level Errors */
  TESTFM_NOREGISTRY        = 10,  /**< Test registry not initialized. */
  TESTFM_REGISTRY_EXISTS   = 11,  /**< Attempt to cu_set_registry() without cu_cleanup_registry(). */

  /* Test Suite Level Errors */
  TESTFM_NOSUITE           = 20,  /**< A required cu_p_suite_t pointer was NULL. */
  TESTFM_NO_SUITENAME      = 21,  /**< Required cu_suite_t name not provided. */
  TESTFM_SINIT_FAILED      = 22,  /**< Suite initialization failed. */
  TESTFM_SCLEAN_FAILED     = 23,  /**< Suite cleanup failed. */
  TESTFM_DUP_SUITE         = 24,  /**< Duplicate suite name not allowed. */

  /* Test Case Level Errors */
  TESTFM_NOCMD            = 30,  /**< A required cu_p_test_t pointer was NULL. */
  TESTFM_NO_CMDNAME       = 31,  /**< Required cu_test_t name not provided. */
  TESTFM_DUP_CMD          = 32,  /**< Duplicate test case name not allowed. */
  TESTFM_CMD_NOT_IN_SUITE = 33,  /**< Test not registered in specified suite. */
  TESTFM_NOHELP                 = 36, /**< A required help function pointer was NULL. */
  TESTFM_NOPLATFORM        = 37, /**< A required help function pointer was NULL. */
  TESTFM_PLATFORMNOTSUPPORT = 38, /**< Platform not support. */
} testfm_error_code_t;

typedef enum
{
  PLATFORM_WIZARD = 0x1<<0,
  PLATFORM_MAGIC = 0x1<<1,
  PLATFORM_WARRIORS = 0x1<<2,
  PLATFORM_JAZZ = 0x1<<4,
  
  PLATFORM_ANY = 0x1<<31,
  
  PLATFORM_MAX
}supported_platform_t;

typedef enum
{
  RET_SUCCESS = 0,
  RET_FAIL,
  RET_DONE
}testfm_result_t;

/*  Type definition for Initialization/Cleaup/TestFunction */
typedef s32 (*testfm_initialize_func_t)(void);  /**< Signature for suite
                                                  initialization function. */
typedef s32 (*testfm_cleanup_func_t)(void);     /**< Signature for suite cleanup
                                                  function. */
typedef void (*testfm_cmd_func_t)(void);        /**< Signature for a testing
                                                  function in a test case. */
typedef void (*testfm_help_func_t)(void);        /**< Signature for a test help
                                                  function in a test case. */
typedef void (*testfm_task_func_t)(void* para); /**<Signature for a testing
							      with tasks to run paralllel*/

typedef struct testfm_cmd_t
{
  char                       *cmdName; /**< command name of the test function. */
  testfm_cmd_func_t  pCmdFunc; /**< Pointer to the test function. */
  
  testfm_help_func_t  pHelpFunc; /**< Pointer to the test help function. */
  u32                        platformList;  /**< supported platform. */
  BOOL                        canAutoRun;
								  	
  //struct testfm_task_func_t* pTasklist;
  struct testfm_cmd_t* pNext; /**< Pointer to the next test in linked list. */
  struct testfm_cmd_t* pPrev; /**< Pointer to the previous test in linked
                                list. */

} testfm_cmd_t;

typedef testfm_cmd_t* testfm_p_cmd_t;

typedef struct testfm_suite_t
{
  char              *name; /**< Suite name. */
  testfm_p_cmd_t          pCmd; /**< Pointer to the 1st test in the suite. */
  testfm_initialize_func_t pInitializeFunc; /**< Pointer to the suite
                                             initialization function. */
  testfm_cleanup_func_t  pCleanupFunc; /**< Pointer to the suite cleanup
                                          function. */

  u32      numberOfCmds; /**< Number of tests in the suite. */
  struct testfm_suite_t*  pNext; /**< Pointer to the next suite in linked
                                  list. */
  struct testfm_suite_t*  pPrev; /**< Pointer to the previous suite in linked
                                  list. */
  supported_platform_t spec_cmd;//plateform support

} testfm_suite_t;

typedef testfm_suite_t* testfm_p_suite_t;

typedef struct testfm_registry_t
{
  u32 numberOfSuites;  /**< Number of suites in the test
                                       registry. */
  u32 numberOfCmds;   /**< Number of cmds in the test
                                       registry. */
  testfm_p_suite_t    pSuite;         /**< Pointer to the 1st suite in the test
                                       registry. */
} testfm_registry_t;
typedef testfm_registry_t* testfm_p_registry_t;

typedef struct testfm_platform_statistic_t
{
  unsigned int platform;
  char *platform_name;
  unsigned int total;
  unsigned int fail;
}testfm_platform_statistic_t;


#endif
