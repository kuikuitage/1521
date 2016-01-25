/******************************************************************************/
/******************************************************************************/

#ifndef __TESTFM_DB_H__
#define __TESTFM_DB_H__

BOOL testfm_is_running(void);

testfm_p_registry_t testfm_get_registry(void);

BOOL testfm_find_cmd(char *cmd, testfm_p_suite_t *pSuite, testfm_p_cmd_t *pCmd);

testfm_error_code_t testfm_run_cmd(testfm_p_suite_t pSuite, testfm_p_cmd_t pCmd);

#endif
