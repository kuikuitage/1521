/******************************************************************************/
/******************************************************************************/

#ifndef __LOG_AP_EPG_H__
#define __LOG_AP_EPG_H__

/*!
  performance epg check point
  */
typedef enum
{
/*!
  check one section start
*/
  PERF_ONE_PF_REQUEST = 0,
/*!
  check one section end
*/
  PERF_ONE_PF_FOUND,
/*!
  check one table start
*/
  PERF_ONE_TABLE_REQUEST,
/*!
  check one table end
*/
  PERF_ONE_TABLE_FOUND
/*!
   store one section data start
*/
//  PERF_EPG_BEGIN,
/*!
   store one section data end
*/
 // PERF_EPG_END
}perf_epg_check_point_t;
/*!
  init
*/
handle_t log_ap_epg_init(void);

#endif //End for __LOG_AP_EPG_H__