/******************************************************************************/
/******************************************************************************/

#ifndef __TESTFM_OP_H__
#define __TESTFM_OP_H__

typedef enum
{
  TEST_MODE,
  DOWNLOAD_MODE
}console_mode_t;

void set_console_mode(console_mode_t mode);
console_mode_t get_console_mode(void);

void cmd_help(void);
void cmd_help_specific(supported_platform_t plat);
void cmd_read(void);
void cmd_download(void);
void cmd_go(void);
void cmd_reboot(void);

void cmd_exit(void);
void cmd_statistic(void);

void cmd_runall(void);

#endif
