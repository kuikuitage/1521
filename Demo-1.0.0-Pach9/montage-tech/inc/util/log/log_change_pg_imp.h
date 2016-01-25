/******************************************************************************/
/******************************************************************************/

#ifndef __LOG_CHANGE_PG_IMP_H__
#define __LOG_CHANGE_PG_IMP_H__

/*!
  change pg performace check point
  */
typedef enum
{
/*!
  check last pg stop for magic
*/
  TAB_PB_STOP,
 /*!
  check last pg stop for Jazz
*/
  TAB_PB_STOP_SYNC,
 /*!
  check same tp or not
*/ 
  TAB_PB_START,
 /*!
  check current pg do play
*/   
  TAB_PB_DO_PLAY
  
}change_pg_perf_timepoint_t;

 /*!
   init
 */
handle_t log_ap_change_pg_init(void);

#endif // End for __LOG_AP_SCAN_H__

