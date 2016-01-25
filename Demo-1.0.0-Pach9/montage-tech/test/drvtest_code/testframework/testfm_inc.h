/******************************************************************************/
/******************************************************************************/

#ifndef __TESTFM_INC_H__
#define __TESTFM_INC_H__

#define TESTFM_ASSERT(condition) \
  do \
  {  \
    if((condition) != TRUE) \
    { \
      extern void testfm_restroe_to_cmd(void);\
      mtos_printk("\nASSERT:%s line %d\n", __FILE__, __LINE__);\
      testfm_restroe_to_cmd();\
    }\
  }\
  while (0)

extern void private_setresult(testfm_result_t result, u32 errorcode);
#define TESTFM_SETRESULT(result, errorcode, is_assert) \
  do \
  {  \
    private_setresult(result, errorcode);\
    TESTFM_ASSERT(!is_assert);\
  }\
  while (0)


  #define TESTFM_LOG(...) \
    do\
    { \
      mtos_printk("#LOG: ");\
      mtos_printk(__VA_ARGS__);\
      mtos_printk(" #END\n");\
    }\
    while(0)

testfm_error_code_t testfm_init(void);

void testfm_set_platform(u32 platform, const char *platform_name);

testfm_p_suite_t testfm_add_suite(const char *suiteName,
                                                           testfm_initialize_func_t pInit,
                                                           testfm_cleanup_func_t pClean);
testfm_p_suite_t testfm_add_suite_specific(const char *suiteName,
                                                           testfm_initialize_func_t pInit,
                                                           testfm_cleanup_func_t pClean,
                                                           supported_platform_t plat);

testfm_p_cmd_t testfm_add_cmd(testfm_p_suite_t pSuite,
                                                 const char *cmdName,
                                                 testfm_cmd_func_t pCmdFunc,
                                                 testfm_help_func_t pHelpFunc,
                                                 u32 supported_platform,
                                                 BOOL canAutoRun);

void testfm_console_run(void);

void testfm_save_env(u32 pointer, u32 stack);

void testfm_console_get_parameter(char *p_parameter, char spliter);
void testfm_console_get_parameter_str(char *p_parameter_str);

void testfm_set_parameter_str(char *p_parameter_str);
u32 testfm_get_num(char *str);

#endif
