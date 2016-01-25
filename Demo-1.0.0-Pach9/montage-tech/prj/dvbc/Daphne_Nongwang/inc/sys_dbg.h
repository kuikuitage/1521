#ifndef __SYS_DEBUG_H__
#define __SYS_DEBUG_H__
#include "stdio.h"
#include <string.h>
#include "stdarg.h"
#include "sys_define.h"

#define __DEBUG__
/*mode*/
enum
{
	MAIN,/*ca系统的debug信息*/
	DVBCAS_PORTING,
	DVBCAS_EVENT,
	PROMPT,
	SEARCH,
	DISPLAY,
	LANGUAGE_SET,
	TRACK,
	HELP,
	SYS_SET,
	MSG,
	CLASSIFY,
	MAINMENU,
	CARD_INFO,
	PRO_INFO,
	PPV,
	EPG,
	PROG_LIST,
	CA_CARD_INFO,
	VOLUME,
	FAV_LIST,
	UTIL_API,
	PLAY_API,
	SENIOR_SUB_MENU,
	SUB_MENU,
	TP_SET,
	FW_DESKTOP,
	FW_MESSAGE,
	CA_INFO_DESAI,
	CA_PPT_DLG_DEMO,
	CA_PPT_DLG_DESAI,
	COMM_DLG,
	COMM_ROOT,
	CONDITIONAL_ACCEPT,
	DESKTOP,
	DO_SEARCH,
	CTRL_GET_CTRL_ID,
	CA_IPPV_PPT_DLG,
	COMM_CTL,
	CHANNEL_EDIT,
	PROG_BAR,
	DVBAD,
	TRCA,
	TRAD,
	QZCA,
	DSCA4,

	MAX_MOD_ID = 128
};




#define DISABLE     0   /*disable debug out put*/
#define EMERG	    1   /* system is unusable                   */
#define ERR		    2   /* error conditions                     */
#define WARNING	    3   /* warning conditions                   */
#define NOTICE	    4   /* normal but significant condition     */
#define INFO	    5   /* informational                        */

extern unsigned char debug_level[];
#define DEBUG_ENABLE_MODE(module,level)	{debug_level[module] = level;}
#define DEBUG_DISABLE_MODE(module)		{debug_level[module] = DISABLE;}
#ifdef __DEBUG__
#define DEBUG(module,level,fmt, ...)												\
    do {																		\
        if ((level <= debug_level[module]))		\
        OS_PRINTF("[%s>%s]%s():%d: "fmt,#module,#level, __FUNCTION__, __LINE__, ##__VA_ARGS__);		\
    } while (0)

#define DUMP(module,level,len,p,fmt) \
	do {	\
		if ((level <= debug_level[module])){		\
			int i__;\
			OS_PRINTF("[%s>%s]%s():%d: ",#module,#level, __FUNCTION__, __LINE__);		\
			OS_PRINTF("len = %d>>>",len);\
			for(i__=0;i__<len;i__++){\
				OS_PRINTF(fmt",",p[i__]);\
				if (i__&&i__%20 == 0)OS_PRINTF("\n");\
			}\
			OS_PRINTF("\n");\
		}\
	}while (0)

#else
#define DEBUG(mode,level,args...)		((void)0)
#define DUMP(len,p,fmt)		do{}while(0)
#endif



#ifdef __DEBUG__
	#define D_CHECK_BOOL(ret)  do{if(!ret)OS_PRINTF("On file:%s line %d >>> \n%s check bool return failure!\n",__FILE__,__LINE__,#ret);}while(0)
	#define D_CHECK_ZERO(ret)  do{if(ret != 0)OS_PRINTF("On file:%s line %d >>> \n%s check zero return failure!\n",__FILE__,__LINE__,#ret);}while(0)
    #define ASSERT(a) do{if(!(a))OS_PRINTF("Assert failure %s in %s:%d",#a,__FUNCTION__,__LINE__);}while(0)
#else
	#define D_CHECK_BOOL(ret) do{(ret);}while(0)
	#define D_CHECK_ZERO(ret) do{(ret);}while(0)
    #define ASSERT(a)   do{}while(0)
#endif


#define	CHECK_RET(module,ret)     do{ if(ret<0)DEBUG(module,ERR,"ret value err :%d\n",ret);}while(0)

#endif



