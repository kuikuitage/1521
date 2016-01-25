/*------------------------------------------------------------------------------------------*
filename    :      Tr_Cas.c
Description :      The TOPREAL CAS API
**-----------------------------------------------------------------------------------------*/
/* Copyright:   (C) BEIJING Topreal TECHNOLOGIES CO.LTD All Rights Reserved. */

/*
There are two kinds of function interface.
MC_XX..(): It resides in MCELL library, and called by user's application.
TRDRV_XX..(): It should be accomplished by user, and called by MCELL.
*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "lib_bitops.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "drv_dev.h"
#include "drv_svc.h"
#include "pti.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "cas_adapter.h"

#include "toprealdef.h"

/**
   for Topreal CAS,we should manager mail/announce msg in our dirver layer, so
   define structs as follows
*/

#define TR_MAX_TASK_NUM (16)
#define FLASH_MSG_STORE_MAX_NUM (20)			/* max msg num store on nvram */
#define FLASH_OFFSET_STORE_MSG_INDEX  (1024)   /* reserve 1k for topreal nvram use */
#define FLASH_OFFSET_MAIL	(1024 + 2048)				/* reserve 1k for topreal nvram use, reserve 2k for short message wIndex store */
#define FLASH_OFFSET_ANNOUNCE (FLASH_OFFSET_MAIL \
		+FLASH_MSG_STORE_MAX_NUM*sizeof(cas_mail_store_t) \
		+sizeof(cas_mail_headers_store_t))
#define NOTIFICATION_MAX_NUM (8192)
#define MSG_QUEUE_MAX_NUM (10)					/* msg queue max num */
#define STORE_MSG_TASK_STACK_SIZE	(24 * KBYTES)
#define STORE_MSG_TASK_PRIO (55)
#define MSG_IDENTIFY_MAX_NUM (200)
#define INVALID_INDEX (0xff)
typedef enum{
TR_CAS_INIT_OPERATE_RET = 0,
TR_CAS_INFO_OPERATE_RET,
TR_EPURSE_INFO_OPERATE_RET,
TR_IPPV_INFO_OPERATE_RET,
TR_ENTITLE_INFO_OPERATE_RET,
TR_CONTROL_MSG_OPERATE_RET,
TR_PURCHASE_OPERATE_RET,
TR_MASTER_SON_OPERATE_RET,
TR_COMMON_OPERATE_RET
}TR_CAS_FUNC;

/*!
  The store mail/announce header format
  */
typedef struct
{
    u16 body_len;
   	u8 priority;
	u8 mail_id;
	u32 has_read;
  	u8 subject[CAS_MAIL_SUBJECT_MAX_LEN];
	u8 creat_date[CAS_MAIL_DATE_MAX_LEN];
}cas_mail_header_store_t;

/*!
The operators info struct
*/
typedef struct
{
  	cas_mail_header_store_t  p_mail_head[FLASH_MSG_STORE_MAX_NUM];
  	u16 max_num;
  	u16 reserve;
}cas_mail_headers_store_t,cas_announce_headers_store_t;

/**
  The msg need to store in nvram
*/
typedef struct
{
	cas_mail_body_t cas_mail_body;				/* all mail/announce headers */
	u16 wIndex; 								/* used to indate mail whether has received */
	u16 reserve;
}cas_mail_store_t;

/**
  The mail/announce body link struct
*/
typedef struct cas_mail_body_node
{
	cas_mail_store_t cas_mail_store;		    /* mail/annouce body need to store in nvram */
	struct cas_mail_body_node *p_next;			/* mail/annouce body node pointer */
}cas_mail_body_node_t,cas_announce_body_node_t;

/**
  The mail/announce format
*/
typedef struct
{
	cas_mail_headers_store_t cas_mail_headers;		/* mail/announce headers */
	cas_mail_body_node_t *p_cas_mail_body_link;	/* mail/announce body link list */
}cas_mail_msg_t,cas_announce_msg_t;

/**
  Store msg to mem result
*/
typedef enum
{
	RET_SUCCESS,
    RET_FLUSH_FAILED,
	RET_HAS_RECEIVED
}cas_store_msg_ret_e;

typedef struct _s_msg_queue_node
{
	CAS_MSG_STRUCT cas_msg_struct;
	u8 used_flag;		/* this node has used ? 1:used, 0:free */
	struct _s_msg_queue_node *next;
}s_msg_queue_node_t, *queue_node_ptr;

typedef struct _s_msg_queue_link
{
	queue_node_ptr head;
	queue_node_ptr tail;
}s_msg_queue_link_t;

/*-------------------------------------------------------------------------------------*/


/***************************************************************************************/
/*---- The below functions are provided by MCELL and called by user's application. ----*/
/***************************************************************************************/
/*!
  error code returned by CoreInit
  */
#define TRCAS_CREATE_TASK_ERROR (1)  /* Create Task failed */
#define TRCAS_INIT_SMC_ERROR (2)              /* Init SMC driver failed */
#define TRCAS_INIT_ECM_ERROR (3)              /* Init ECM driver falied */
#define TRCAS_INIT_EMM_ERROR (4)             /*  Init EMM driver failed */
#define TRCAS_INIT_NVRAM_ERROR (5)          /* Init NVRAM driver failed */

#define TRCAS_PARAMETER_ERROR (2)          /*    2 - Input parameter error */
#define TRCAS_PIN_CODE_ERROR (3)                   /*    3 - Input incorrect Pin code */
#define TRCAS_NVRAM_OPERATE_ERROR (4)        /*    4 - Nvram error from smart card */
#define TRCAS_MEMORY_ALLOC_ERROR (5)         /*    5 - No enough money */
#define TRCAS_MEMORY_SPACE_ERROR (6)          /*    6 - No space for saving product */

#define TRCAS_ETITLE_PARAM_ERROR (1)    /* 1: Input parameters error; */
#define TRCAS_NO_ENTITLE (2)                           /* 2: No entitle info; */
#define TRCAS_ENTITLE_LIMIT (3)                      /* 3: Valid entitle, but limited; */
#define TRCAS_ENTITLE_CLOSED (4)                  /* 4: Entitle closed; */
#define TRCAS_ENTITLE_PAUSED (5)                 /* 5: Entitle paused */


#define TRCAS_PRODUCT_ERROR (3)                      /* 3 - No valid product */
#define TRCAS_NOT_SUPORTED  (5)                       /* 5 - NOT_SUPPORTED functionality */

#define TRCAS_IPP_NUM         (20)
#define TRCAS_MAX_PIN_LEN (6)
/*!
   The structure for save CA notification message,
   the messages will be used by Query control function.
  */

extern S16 (* SC_ATR_Notify_fun)(U8, U8, U8 *, U8);
extern cas_adapter_priv_t g_cas_priv;
cas_tr_priv_t *tr_priv = NULL;
s_msg_queue_link_t msg_queue_link = {0};
s_msg_queue_node_t *p_msg_queue_node[MSG_QUEUE_MAX_NUM] = {NULL};

os_sem_t semaphore[4] = {0};
os_sem_t nvram_semaphore[4] = {0};
os_sem_t queue_semaphore = 0;

cas_mail_msg_t cas_mail_msg;
cas_announce_msg_t cas_announce_msg;
#ifdef INDENTIFY_CREATE_TINE
CAS_TIMESTAMP cas_message_identify_msg[MSG_IDENTIFY_MAX_NUM];
#else
u16 cas_message_identify_msg[MSG_IDENTIFY_MAX_NUM];
#endif
msg_info_t cas_notification_msg;
u32 notification_index[NOTIFICATION_MAX_NUM] = {0};
u32 cas_card_status = CAS_S_CARD_REMOVE;

void cas_tr_send_desc_event(u8 id)
{
    cas_tr_priv_t *p_priv  = (cas_tr_priv_t *)g_cas_priv.cam_op[CAS_ID_TR].p_priv;
    switch(id)
    {
        case TR_DESC_EVENT_KEY_SET:
            if(p_priv->card_status != SMC_CARD_REMOVE)
                cas_send_event(p_priv->slot, CAS_ID_TR, CAS_S_GET_KEY, 0);
        break;
        case TR_DESC_EVNET_KEY_CLEAR:
            if(p_priv->card_status == SMC_CARD_REMOVE)
                cas_send_event(p_priv->slot, CAS_ID_TR, CAS_S_CARD_REMOVE, 0);
        break;
        default:
           OS_PRINTF("Wrong desc event\n");
           break;
    }
}

u32 parse_event(TR_CAS_FUNC func, s16 ret)
{
    u32 event = CAS_E_IOCMD_NOT_OK;

    if(MC_NOT_OK == ret &&
        (TR_CAS_INIT_OPERATE_RET != func ||TR_ENTITLE_INFO_OPERATE_RET != func))
    {
        event = CAS_E_IOCMD_NOT_OK;
        return event;
    }
    switch(func)
    {
        case TR_CAS_INIT_OPERATE_RET:
            switch(ret)
            {
                case TRCAS_CREATE_TASK_ERROR:
                    event = CAS_E_CARD_INIT_FAIL;
                    break;
                case TRCAS_INIT_SMC_ERROR:
                    event = CAS_E_CARD_INIT_FAIL;
                    break;
                case TRCAS_INIT_ECM_ERROR:
                    event = CAS_E_CARD_INIT_FAIL;
                    break;
                case TRCAS_INIT_EMM_ERROR:
                    event = CAS_E_CARD_INIT_FAIL;
                    break;
                case TRCAS_INIT_NVRAM_ERROR:
                    event = CAS_E_CARD_INIT_FAIL;
                    break;
            }
            break;

        case TR_CAS_INFO_OPERATE_RET:
        case TR_EPURSE_INFO_OPERATE_RET:
        case TR_IPPV_INFO_OPERATE_RET:
            switch(ret)
            {
                case MC_PARAMETER_ERROR:
                    event = CAS_E_PARAM_ERR;
                    break;
                case TRCAS_PRODUCT_ERROR:
                    event = CAS_E_IPP_INFO_ERROR;
                    break;
                case TRCAS_NVRAM_OPERATE_ERROR:
                    event = CAS_E_NVRAM_ERROR;
                    break;
                case TRCAS_NOT_SUPORTED:
                    event = CAS_ERROR_CARD_NOTSUPPORT;
                    break;
            }
            break;
        case TR_ENTITLE_INFO_OPERATE_RET:
            switch(ret)
            {
                case TRCAS_ETITLE_PARAM_ERROR:
                    event = CAS_E_PARAM_ERR;
                    break;
                case TRCAS_NO_ENTITLE:
                    event = CAS_C_NO_ENTITLE;
                    break;
                case TRCAS_ENTITLE_LIMIT:
                    event = CAS_C_ENTITLE_LIMIT;
                    break;
                case TRCAS_ENTITLE_CLOSED:
                    event = CAS_C_ENTITLE_CLOSED;
                    break;
                case TRCAS_ENTITLE_PAUSED:
                    event = CAS_C_ENTITLE_PAUSED;
                    break;
            }
            break;
        case TR_CONTROL_MSG_OPERATE_RET:
            switch(ret)
            {
                case CAS_MC_QUERY_SUCCESS:
                  ;
                    break;
                case CAS_MC_QUERY_FAIL:
                    OS_PRINTF("Query control message failed.\n");
                    event = CAS_E_QUERY_CTRL_MSG_ERROR;
                    break;
                case CAS_MC_QUERY_NOT_AVAILABLE:
                    OS_PRINTF("Query failed, control message does not available.\n");
                    event = CAS_E_CTRL_MSG_NOT_AVAILABLE;
                    break;
            }
            break;
        case TR_PURCHASE_OPERATE_RET:
            switch(ret)
            {
                case MC_PARAMETER_ERROR:
                    event = CAS_E_PARAM_ERR;
                    break;
                case TRCAS_PIN_CODE_ERROR:
                    event = CAS_E_PIN_INVALID;
                    break;
                case TRCAS_NVRAM_OPERATE_ERROR:
                    event =CAS_E_NVRAM_ERROR;
                    break;
                case TRCAS_MEMORY_ALLOC_ERROR:
                    event = CAS_E_MEM_ALLOC_FAIL;
                    break;
                case TRCAS_MEMORY_SPACE_ERROR:
                    event = CAS_E_CARD_NO_ROOM;
                    break;
            }
            break;
        case TR_MASTER_SON_OPERATE_RET:
            switch(ret)
            {
                case MC_PARAMETER_ERROR:
                    event = CAS_E_PARAM_ERR;
                    break;
                case MC_DATA_ERROR:
                    event = CAS_E_FEED_DATA_ERROR;
                    break;
                case MC_MEMORY_RW_ERROR:
                    event = CAS_E_FEED_DATA_ERROR;
                    break;
                case MC_NOT_SUPPORTED:
                    event = CAS_ERROR_CARD_NOTSUPPORT;
                    break;
                case MC_STATE_ERROR:
                    event = CAS_E_FEED_DATA_ERROR;
                    break;
                case MC_SCSN_UNMATCHED:
                    event = CAS_E_CARD_PARTNER_FAILED;
                    break;
                case MC_UNKNOWN_ERROR:
                    event = CAS_E_UNKNOW_ERR;
                    break;
            }
            break;
         default:
            break;
        }
    return event;
}

static RET_CODE get_vacant_memory(u8 *pointerIndex)
{
	u8 uIndex = 0;

	for (uIndex = 0; uIndex < MSG_QUEUE_MAX_NUM; uIndex ++)
	{
		if (!p_msg_queue_node[uIndex]->used_flag)
		{
			break;
		}
	}

	if (uIndex == MSG_QUEUE_MAX_NUM)
	{
		return ERR_FAILURE;
	}

	*pointerIndex = uIndex;

	return SUCCESS;
}

static RET_CODE add_data_to_queue(CAS_MSG_STRUCT *p_cas_msg)
{
	u8 pointerIndex = 0;

	OS_PRINTF("%s:LINE:%d\n",__FUNCTION__, __LINE__);
	if (NULL == p_cas_msg)
	{
		OS_PRINTF("%s:LINE:%d, not valid pointer\n",__FUNCTION__, __LINE__);
		return ERR_FAILURE;
	}

	mtos_sem_take(&queue_semaphore, 0);
	if ((NULL != msg_queue_link.head) && (NULL != msg_queue_link.tail))	/* queue not emepty */
	{
		msg_queue_link.tail->next = p_msg_queue_node[pointerIndex];
		msg_queue_link.tail = p_msg_queue_node[pointerIndex];
	}
	else	/* queue empty */
	{
		msg_queue_link.head = msg_queue_link.tail = p_msg_queue_node[pointerIndex];
		msg_queue_link.tail->next = NULL;
	}
	mtos_sem_give(&queue_semaphore);

	return SUCCESS;
}

static RET_CODE get_data_from_queue(CAS_MSG_STRUCT *p_cas_msg, u8 *msg_num)
{
	CAS_MSG_STRUCT *p_msg_temp = NULL;
	s_msg_queue_node_t *p_queue_node = NULL;
	u8 msg_count = 0;

	//OS_PRINTF("%s:LINE:%d\n",__FUNCTION__, __LINE__);

	if ((NULL == p_cas_msg) || (NULL == msg_num))
	{
		OS_PRINTF("%s:LINE:%d, pointer is NULL!\n",__FUNCTION__, __LINE__);
		return ERR_FAILURE;
	}

	mtos_sem_take(&queue_semaphore, 0);
	if ((NULL == msg_queue_link.head) && (NULL == msg_queue_link.tail))
	{
		//OS_PRINTF("%s:LINE:%d, msg queue is empty!\n",__FUNCTION__, __LINE__);
		mtos_sem_give(&queue_semaphore);
		return SUCCESS;
	}

	p_msg_temp = p_cas_msg;

	while (msg_queue_link.head)
	{
		memcpy(p_msg_temp, &msg_queue_link.head->cas_msg_struct, sizeof(CAS_MSG_STRUCT));
		p_queue_node = msg_queue_link.head->next;
		memset(msg_queue_link.head, 0, sizeof(s_msg_queue_node_t));
		msg_queue_link.head = p_queue_node;
		p_msg_temp ++;
		msg_count ++;
	}
	*msg_num = msg_count;
	mtos_sem_give(&queue_semaphore);

	return SUCCESS;
}


/**
 *  Flush msg (mail/announce) to nvram
 *  it should be used when mail/announce list change(add/delete)
 *
 *  param[in] type: msg type, mail or announce
 *                  it get from CAS_MSG_STRUCT.bType
 *
 *  return
 *			  if flush success, return SUCCESS,else ERR_FAILURE
*/
static RET_CODE cas_tr_flush_msg_to_nvram(u8 type)
{
	cas_mail_headers_store_t *p_cas_all_headers = NULL;
	cas_mail_store_t *p_cas_all_bodys = NULL;
	cas_mail_body_node_t *p_cas_body_node = NULL;
	cas_mail_msg_t *p_cas_msg = NULL;

	cas_tr_priv_t *p_priv = NULL;

	u8 *p_start_addr = NULL;
	u32 headers_len = 0;
	u32 body_len = 0;
	u32 all_msg_len = 0;
	u16 msg_num = 0;
	u32 offset = 0;
	s16 ret = 0;

	//OS_PRINTF("%s:LINE:%d\n",__FUNCTION__, __LINE__);

	p_priv = g_cas_priv.cam_op[CAS_ID_TR].p_priv;
       switch(type)
       {
           case CAS_MSG_MAIL:
               offset = FLASH_OFFSET_MAIL;
               msg_num = cas_mail_msg.cas_mail_headers.max_num;
               p_cas_msg = &cas_mail_msg;
               break;
           case CAS_MSG_ANNOUNCE:
               offset = FLASH_OFFSET_ANNOUNCE;
               msg_num =  cas_announce_msg.cas_mail_headers.max_num;
               p_cas_msg = &cas_announce_msg;
               break;
           //case CAS_MSG_NOTIFICATION:
           case CAS_MSG_INDEX:
               offset = FLASH_OFFSET_STORE_MSG_INDEX;
               if(NULL == p_priv->nv_write)
               {
                   OS_PRINTF("%s:LINE:%d,flash write function NULL!",__FUNCTION__, __LINE__);
                   return ERR_FAILURE;
               }
               mtos_sem_take(&nvram_semaphore[type], 0);
			   #ifdef INDENTIFY_CREATE_TINE
			   ret = p_priv->nv_write(offset, (u8 *)cas_message_identify_msg, MSG_IDENTIFY_MAX_NUM * sizeof(CAS_TIMESTAMP));
			   #else
               ret = p_priv->nv_write(offset, (u8 *)cas_message_identify_msg, MSG_IDENTIFY_MAX_NUM * sizeof(u16));
			   #endif
               mtos_sem_give(&nvram_semaphore[type]);
               return SUCCESS;
               break;
           default:
               OS_PRINTF("Invalid message type.\n");
       }
#if 0
	offset = (type == CAS_MSG_MAIL) ? FLASH_OFFSET_MAIL : FLASH_OFFSET_ANNOUNCE;
	msg_num = (type == CAS_MSG_MAIL) ?
				cas_mail_msg.cas_mail_headers.max_num : cas_announce_msg.cas_mail_headers.max_num;
	p_cas_msg = (type == CAS_MSG_MAIL) ? (&cas_mail_msg) : (&cas_announce_msg);
#endif
	headers_len = sizeof(cas_mail_headers_store_t);
	body_len = sizeof(cas_mail_store_t);

	all_msg_len = headers_len + body_len * msg_num;
	p_start_addr = mtos_malloc(all_msg_len);
	MT_ASSERT(NULL != p_start_addr);
	memset(p_start_addr, 0, all_msg_len);

	/* store all msg heade */
	p_cas_all_headers = (cas_mail_headers_store_t *)p_start_addr;
	memcpy(p_cas_all_headers, &p_cas_msg->cas_mail_headers, headers_len);
	p_cas_all_headers ++;

	/* store all msg body */
	p_cas_all_bodys = (cas_mail_store_t *)p_cas_all_headers;
	p_cas_body_node = p_cas_msg->p_cas_mail_body_link;
	while(NULL != p_cas_body_node)
	{
		memcpy(p_cas_all_bodys,
			&p_cas_body_node->cas_mail_store,
			body_len);
		p_cas_all_bodys ++;
		p_cas_body_node = p_cas_body_node->p_next;
	}

	/* flush to nvram */
	if(NULL == p_priv->nv_write)
	{
		OS_PRINTF("%s,LINE:%d,nvram write NULL!\n", __FUNCTION__, __LINE__);
		mtos_free(p_start_addr);
		return ERR_FAILURE;
	}

	mtos_sem_take(&nvram_semaphore[type], 0);
	ret = p_priv->nv_write(offset, p_start_addr, all_msg_len);
	if(SUCCESS != ret)
	{
		OS_PRINTF("%s,LINE:%d,flush to nvram failed!\n", __FUNCTION__, __LINE__);
		mtos_free(p_start_addr);
		mtos_sem_give(&nvram_semaphore[type]);
		return ERR_FAILURE;
	}
	mtos_sem_give(&nvram_semaphore[type]);

	mtos_free(p_start_addr);

	OS_PRINTF("%s:LINE:%d,flush msg to nvram success!\n",__FUNCTION__, __LINE__);

	return SUCCESS;
}

/**
 *  Get msg (mail/announce) from nvram
 *  it should be used when need to get all mail/announce
 *
 *  param[in] type: msg type, mail or announce
 *				   it get from CAS_MSG_STRUCT.bType
 *
 *  return
 *		     if get success,return SUCCESS,else ERR_FAILURE
*/
static RET_CODE cas_tr_get_msg_from_nvram(u8 type)
{
	cas_mail_headers_store_t *p_cas_all_headers = NULL;
	cas_mail_store_t *p_cas_all_bodys = NULL;
	cas_mail_store_t *p_cas_all_bodys_temp = NULL;
	cas_mail_body_node_t *p_cas_body_node = NULL;
	cas_mail_body_node_t *p_cas_body_node_temp = NULL;
	cas_mail_msg_t *p_cas_msg = NULL;

	cas_tr_priv_t *p_priv = NULL;

	u32 headers_len = 0;
	u32 body_len = 0;
	u32 all_msg_body_len = 0;
	u16 msg_num = 0;
	u32 offset = 0;
	s16 ret = 0;

	//OS_PRINTF("%s:LINE:%d\n",__FUNCTION__, __LINE__);

	p_priv = g_cas_priv.cam_op[CAS_ID_TR].p_priv;

       switch(type)
       {
           case CAS_MSG_MAIL:
               offset = FLASH_OFFSET_MAIL;
               p_cas_msg = &cas_mail_msg;
               break;
           case CAS_MSG_ANNOUNCE:
               offset = FLASH_OFFSET_ANNOUNCE;
               p_cas_msg = &cas_announce_msg;
               break;
           //case CAS_MSG_NOTIFICATION:
           case CAS_MSG_INDEX:
               offset = FLASH_OFFSET_STORE_MSG_INDEX;
               if(NULL == p_priv->nv_read)
               {
                   OS_PRINTF("%s:LINE:%d,flash read function NULL!",__FUNCTION__, __LINE__);
                   return ERR_FAILURE;
               }
			   #ifdef INDENTIFY_CREATE_TINE
               body_len = MSG_IDENTIFY_MAX_NUM * sizeof(CAS_TIMESTAMP);
			   #else
			   body_len = MSG_IDENTIFY_MAX_NUM * sizeof(u16);
			   #endif
               mtos_sem_take(&nvram_semaphore[type], 0);
               ret = p_priv->nv_read(offset, (u8 *)cas_message_identify_msg, &body_len);
               mtos_sem_give(&nvram_semaphore[type]);
               for(ret = 0; ret < MSG_IDENTIFY_MAX_NUM; ret++)
               {
			   #ifdef INDENTIFY_CREATE_TINE
                   if(cas_message_identify_msg[ret].bYear[0] != INVALID_INDEX)
                       OS_PRINTF("#### %d %02d%02d\n", __LINE__, cas_message_identify_msg[ret].bYear[0], cas_message_identify_msg[ret].bYear[1]);
		       #else
                   if(cas_message_identify_msg[ret] != INVALID_INDEX)
                       OS_PRINTF("#### %d 0x%4x\n", __LINE__, cas_message_identify_msg[ret]);
			   #endif
               }
               return SUCCESS;
               break;
           default:
               OS_PRINTF("Invalid message type.\n");
       }
	//offset = (type == CAS_MSG_MAIL) ? FLASH_OFFSET_MAIL : FLASH_OFFSET_ANNOUNCE;
	//OS_PRINTF("%s:LINE:%d, offset = %d\n",__FUNCTION__, __LINE__, offset);
	//p_cas_msg = (type == CAS_MSG_MAIL) ? (&cas_mail_msg) : (&cas_announce_msg);
	headers_len = sizeof(cas_mail_headers_store_t);
	body_len = sizeof(cas_mail_store_t);

	p_cas_body_node = p_cas_msg->p_cas_mail_body_link;

	/* check msg linklist whether null */
	if(0 != p_cas_msg->cas_mail_headers.max_num)
	{
		while(NULL != p_cas_body_node)
		{
			p_cas_body_node_temp = p_cas_body_node->p_next;
			p_cas_body_node->p_next = NULL;
			mtos_free(p_cas_body_node);
			p_cas_body_node = p_cas_body_node_temp;
		}
		p_cas_body_node = NULL;
		p_cas_body_node_temp = NULL;
	}

	p_cas_all_headers = mtos_malloc(headers_len);
	MT_ASSERT(NULL != p_cas_all_headers);
	memset(p_cas_all_headers, 0, headers_len);

	if(NULL == p_priv->nv_read)
	{
		OS_PRINTF("%s:LINE:%d,flash read function NULL!",__FUNCTION__, __LINE__);
		mtos_free(p_cas_all_headers);
		return ERR_FAILURE;
	}

	mtos_sem_take(&nvram_semaphore[type], 0);
	ret = p_priv->nv_read(offset, (u8 *)p_cas_all_headers, &headers_len);
	if(SUCCESS != ret)
	{
		OS_PRINTF("%s:LINE:%d,read msg header from flash failed!",__FUNCTION__, __LINE__);
		mtos_free(p_cas_all_headers);
		mtos_sem_give(&nvram_semaphore[type]);
		return ERR_FAILURE;
	}
	mtos_sem_give(&nvram_semaphore[type]);

	/* has no mail/announce */
	if((0 == p_cas_all_headers->max_num) || (0xFFFF == p_cas_all_headers->max_num))
	{
		mtos_free(p_cas_all_headers);
		return SUCCESS;
	}

	mtos_sem_take(&semaphore[type], 0);
	memcpy(&p_cas_msg->cas_mail_headers, p_cas_all_headers, headers_len);
	mtos_free(p_cas_all_headers);
	msg_num = p_cas_msg->cas_mail_headers.max_num;
	mtos_sem_give(&semaphore[type]);

	all_msg_body_len = msg_num * body_len;
	p_cas_all_bodys = mtos_malloc(all_msg_body_len);
	MT_ASSERT(NULL != p_cas_all_bodys);
	memset(p_cas_all_bodys, 0, all_msg_body_len);
	p_cas_all_bodys_temp = p_cas_all_bodys;

	mtos_sem_take(&nvram_semaphore[type], 0);
	ret = p_priv->nv_read(offset+headers_len, (u8 *)p_cas_all_bodys, &all_msg_body_len);
	if(SUCCESS != ret)
	{
		OS_PRINTF("%s:LINE:%d,read msg bodys from flash failed!",__FUNCTION__, __LINE__);
		mtos_free(p_cas_all_bodys);
		mtos_sem_give(&nvram_semaphore[type]);
		return ERR_FAILURE;
	}
	mtos_sem_give(&nvram_semaphore[type]);

	mtos_sem_take(&semaphore[type], 0);
	p_cas_body_node = p_cas_msg->p_cas_mail_body_link;

	if(NULL != p_cas_body_node)
	{
		while(NULL != p_cas_body_node->p_next)
		{
			p_cas_body_node = p_cas_body_node->p_next;
		}
	}

	while(0 != msg_num)
	{
		//OS_PRINTF("%s:LINE:%d,++++++++++++++ mail_id=%d\n",__FUNCTION__, __LINE__,
		//		p_cas_all_bodys_temp->cas_mail_body.mail_id);

		if(NULL == p_cas_msg->p_cas_mail_body_link)
		{
			p_cas_msg->p_cas_mail_body_link = mtos_malloc(sizeof(cas_mail_body_node_t));
			MT_ASSERT(NULL != p_cas_msg->p_cas_mail_body_link);
			memset(p_cas_msg->p_cas_mail_body_link, 0, sizeof(cas_mail_body_node_t));
			memcpy(&p_cas_msg->p_cas_mail_body_link->cas_mail_store, p_cas_all_bodys_temp, body_len);
			p_cas_msg->p_cas_mail_body_link->p_next = NULL;
			p_cas_body_node = p_cas_msg->p_cas_mail_body_link;
		}
		else
		{
			p_cas_body_node_temp = mtos_malloc(sizeof(cas_mail_body_node_t));
			MT_ASSERT(NULL != p_cas_body_node_temp);
			memset(p_cas_body_node_temp, 0, sizeof(cas_mail_body_node_t));
			memcpy(&p_cas_body_node_temp->cas_mail_store, p_cas_all_bodys_temp, body_len);
			p_cas_body_node->p_next = p_cas_body_node_temp;
			p_cas_body_node_temp->p_next = NULL;
			p_cas_body_node = p_cas_body_node->p_next;
		}
		p_cas_all_bodys_temp ++;
		msg_num --;
	}
	mtos_sem_give(&semaphore[type]);

	mtos_free(p_cas_all_bodys);

	return SUCCESS;
}

/**
 *  Delete all msg(mail/announce)
 *  it should use when need to delete all msg
 *
 *  param[in] type: msg type, mail or announce
 *
*/
static RET_CODE cas_tr_delete_all_msg(u8 type)
{
	cas_mail_msg_t *p_cas_msg = NULL;
	cas_mail_body_node_t *p_mail_body_node = NULL;
	cas_mail_body_node_t *p_mail_body_node_next = NULL;

	s16 ret = 0;
	u16 msg_num = 0;

	//OS_PRINTF("%s:LINE:%d\n",__FUNCTION__, __LINE__);


	p_cas_msg = (type == CAS_MSG_MAIL) ? (&cas_mail_msg) : (&cas_announce_msg);
	msg_num = p_cas_msg->cas_mail_headers.max_num;

	if(0 == msg_num)
	{
		OS_PRINTF("%s:LINE:%d,msg list NULL", __FUNCTION__, __LINE__);
		return SUCCESS;
	}

	mtos_sem_take(&semaphore[type], 0);

	memset(&p_cas_msg->cas_mail_headers.p_mail_head, 0,
			sizeof(cas_mail_header_store_t)*msg_num);
	p_cas_msg->cas_mail_headers.max_num = 0;

	p_mail_body_node = p_cas_msg->p_cas_mail_body_link;
	MT_ASSERT(NULL != p_mail_body_node);	/* should not NULL when mg_num != 0 */
	p_mail_body_node_next = p_mail_body_node->p_next;
	while(NULL != p_mail_body_node_next)
	{
		p_mail_body_node->p_next = p_mail_body_node_next->p_next;
		p_mail_body_node_next->p_next = NULL;
		mtos_free(p_mail_body_node_next);
		p_mail_body_node_next = p_mail_body_node->p_next;
	}
	mtos_free(p_cas_msg->p_cas_mail_body_link);
	p_cas_msg->p_cas_mail_body_link = NULL;

	mtos_sem_give(&semaphore[type]);

	ret = cas_tr_flush_msg_to_nvram(type);
	if(SUCCESS != ret)
	{
		OS_PRINTF("%s:LINE:%d, flush to nvram failed!\n", __FUNCTION__, __LINE__);
		return ERR_FAILURE;
	}

	return SUCCESS;

}

/**
 *  Delete msg(mail/announce) by index
 *  it should use when need to delete one msg
 *
 *  param[in] msg_id: msg id will to delete
 * 			  type: msg type, mail or announce
 *
*/
static RET_CODE cas_tr_delete_msg_by_index(u8 msg_id, u8 type)
{
	cas_mail_msg_t *p_cas_msg = NULL;
	cas_mail_body_node_t *p_mail_body_node = NULL;
	cas_mail_body_node_t *p_mail_body_node_priv = NULL;
	cas_mail_body_node_t *p_mail_body_node_next = NULL;

	u8 msg_id_inner = msg_id;
	u8 index = 0;
	s16 ret = 0;

	//OS_PRINTF("%s:LINE:%d, ===============msg_id=%d\n",__FUNCTION__, __LINE__, msg_id);


	p_cas_msg = (type == CAS_MSG_MAIL) ? (&cas_mail_msg) : (&cas_announce_msg);

	mtos_sem_take(&semaphore[type], 0);

	if(msg_id_inner > p_cas_msg->cas_mail_headers.max_num)
	{
		OS_PRINTF("%s:LINE:%d, msg_id greater than max num!\n", __FUNCTION__, __LINE__);
		mtos_sem_give(&semaphore[type]);
		return ERR_FAILURE;
	}

	p_mail_body_node = p_cas_msg->p_cas_mail_body_link;
	p_mail_body_node_priv = p_mail_body_node;

	if(NULL == p_mail_body_node)
	{
		OS_PRINTF("%s:LINE:%d, body link list is NULL!\n", __FUNCTION__, __LINE__);
		mtos_sem_give(&semaphore[type]);
		return ERR_FAILURE;
	}


	/* delete msg is the first one */
	if(msg_id_inner == 0)
	{
		if(NULL == p_mail_body_node->p_next)	/* only one msg in msg body list */
		{
			p_cas_msg->cas_mail_headers.max_num = 0;
			memset(&p_cas_msg->cas_mail_headers.p_mail_head[msg_id_inner], 0, sizeof(cas_mail_header_store_t));
		}
		else
		{
			p_mail_body_node_next = p_mail_body_node->p_next;
			while(NULL != p_mail_body_node_next->p_next)
			{
				p_mail_body_node_next->cas_mail_store.cas_mail_body.mail_id --;
				p_mail_body_node_next = p_mail_body_node_next->p_next;
			}
			p_cas_msg->p_cas_mail_body_link = p_mail_body_node->p_next;
			for(index=msg_id_inner+1; index<p_cas_msg->cas_mail_headers.max_num; index++)
			{
				p_cas_msg->cas_mail_headers.p_mail_head[index].mail_id --;
				//OS_PRINTF("%s:LINE:%d, @@@@@p_cas_msg->cas_mail_headers.p_mail_head[index].mail_id=%d!\n", __FUNCTION__, __LINE__,
				//			p_cas_msg->cas_mail_headers.p_mail_head[index].mail_id);
			}
			memset(&p_cas_msg->cas_mail_headers.p_mail_head[msg_id_inner], 0, sizeof(cas_mail_header_store_t));
			memcpy(&p_cas_msg->cas_mail_headers.p_mail_head[msg_id_inner],
						&p_cas_msg->cas_mail_headers.p_mail_head[msg_id_inner+1],
						sizeof(cas_mail_header_store_t)*(p_cas_msg->cas_mail_headers.max_num-1));
			p_cas_msg->cas_mail_headers.max_num --;
		}

		mtos_free(p_mail_body_node);

		mtos_sem_give(&semaphore[type]);

        ret = cas_tr_flush_msg_to_nvram(type);
    	if(SUCCESS != ret)
    	{
    		OS_PRINTF("%s:LINE:%d, flush to nvram failed!\n", __FUNCTION__, __LINE__);
    		return ERR_FAILURE;
    	}

		return SUCCESS;
	}

	while(msg_id_inner != 0)
	{
		p_mail_body_node_priv = p_mail_body_node;
		p_mail_body_node = p_mail_body_node->p_next;
		msg_id_inner --;
	}

	if(NULL == p_mail_body_node)
	{
		OS_PRINTF("%s:LINE:%d, msg_id to delete not exist!\n", __FUNCTION__, __LINE__);
		mtos_sem_give(&semaphore[type]);
		return ERR_FAILURE;
	}

	if(NULL == p_mail_body_node->p_next)	/* delete msg is the last one */
	{
		p_cas_msg->cas_mail_headers.max_num --;
		mtos_free(p_mail_body_node);
		p_mail_body_node_priv->p_next = NULL;
		memset(&p_cas_msg->cas_mail_headers.p_mail_head[msg_id], 0, sizeof(cas_mail_header_store_t));
	}
	else	/* delete msg is not the last one */
	{
		p_mail_body_node_next = p_mail_body_node->p_next;
		p_mail_body_node_priv->p_next = p_mail_body_node_next;
		while (NULL != p_mail_body_node_next->p_next)
		{
			p_mail_body_node_next->cas_mail_store.cas_mail_body.mail_id --;
			p_mail_body_node_next = p_mail_body_node_next->p_next;
		}

		msg_id_inner = msg_id;
		for(index=msg_id_inner+1; index<p_cas_msg->cas_mail_headers.max_num; index++)
		{
			p_cas_msg->cas_mail_headers.p_mail_head[index].mail_id --;
		}
		memset(&p_cas_msg->cas_mail_headers.p_mail_head[msg_id_inner], 0, sizeof(cas_mail_header_store_t));
		memcpy(&p_cas_msg->cas_mail_headers.p_mail_head[msg_id_inner],
				&p_cas_msg->cas_mail_headers.p_mail_head[msg_id_inner+1],
				sizeof(cas_mail_header_store_t)*(p_cas_msg->cas_mail_headers.max_num-(msg_id_inner+1)+1));
		p_cas_msg->cas_mail_headers.max_num --;

		mtos_free(p_mail_body_node);
	}

	mtos_sem_give(&semaphore[type]);

	ret = cas_tr_flush_msg_to_nvram(type);
	if(SUCCESS != ret)
	{
		OS_PRINTF("%s:LINE:%d, flush to nvram failed!\n", __FUNCTION__, __LINE__);
		return ERR_FAILURE;
	}

	return SUCCESS;
}

/**
 p_cas_msg = (type == CAS_MSG_MAIL) ? (&cas_mail_msg) : (&cas_announce_msg);
 *  Get msg(mail/announce) body by index
 *  it should use when application need get msg body use index
 *
 *  param[in] p_cas_msg: msg struct point,use to store getted msg bog
 *            msg_id: msg id need to get
 * 			  type: msg type, mail or announce
*/
static RET_CODE cas_tr_get_msg_body_by_index(cas_mail_body_t *p_msg_body, u8 msg_id, u8 type)
{
	cas_mail_msg_t *p_cas_msg = NULL;
	cas_mail_body_node_t *p_mail_body_node = NULL;

	u8 msg_id_inner = msg_id;

	MT_ASSERT(NULL != p_msg_body);

	//OS_PRINTF("%s:LINE:%d\n",__FUNCTION__, __LINE__);

	memset(p_msg_body, 0, sizeof(cas_mail_body_t));

	if(type == CAS_MSG_MAIL)
	{
		p_cas_msg = &cas_mail_msg;
	}
	else if(type == CAS_MSG_ANNOUNCE)
	{
		p_cas_msg = &cas_announce_msg;
	}
	else	/* notification */
	{
		return SUCCESS;
	}

	mtos_sem_take(&semaphore[type], 0);

	if(msg_id_inner > p_cas_msg->cas_mail_headers.max_num)
	{
		OS_PRINTF("%s:LINE:%d, msg_id greater than max num!\n", __FUNCTION__, __LINE__);
		mtos_sem_give(&semaphore[type]);
		return ERR_FAILURE;
	}

	p_mail_body_node = p_cas_msg->p_cas_mail_body_link;
	if(NULL == p_mail_body_node)
	{
		OS_PRINTF("%s:LINE:%d, cas msg null!\n", __FUNCTION__, __LINE__);
		mtos_sem_give(&semaphore[type]);
		return ERR_FAILURE;
	}

	while(msg_id_inner != 0)
	{
		p_mail_body_node = p_mail_body_node->p_next;
		msg_id_inner --;
	}

	if(NULL == p_mail_body_node)
	{
		OS_PRINTF("%s:LINE:%d, msg body link list error!\n", __FUNCTION__, __LINE__);
		mtos_sem_give(&semaphore[type]);
		return ERR_FAILURE;
	}

	memcpy(p_msg_body, &p_mail_body_node->cas_mail_store.cas_mail_body, sizeof(cas_mail_body_t));

	p_cas_msg->cas_mail_headers.p_mail_head[msg_id].has_read = 1;

	mtos_sem_give(&semaphore[type]);

	/* here, we need to flush to nvram */
	if(SUCCESS != cas_tr_flush_msg_to_nvram(type))
	{
		OS_PRINTF("%s:LINE:%d,flush to nvram failed!\n",__FUNCTION__, __LINE__);
		return ERR_FAILURE;
	}

	return SUCCESS;
}

/**
 *  To store mail/announce new msg.
 *  it should use after cas_tr_store_new_short_msg calls.
 *
 *  param[in] p_cas_msg_struct: msg need to store.
 *  param[in] mail_num_state: mail num max than
 *
 *  return
 *  	      if store success, return SUCCESS.
 *			  if msg has been received, return RET_HAS_RECEIVED.
*/
static cas_store_msg_ret_e cas_tr_store_new_short_msg(CAS_MSG_STRUCT *p_cas_msg_struct)
{
	cas_mail_header_store_t *p_cas_msg_header = NULL;
	cas_mail_body_node_t *p_cas_msg_body_link = NULL;
	cas_mail_body_node_t *p_cas_msg_body_node = NULL;
	cas_mail_msg_t *p_cas_msg = NULL;

	//OS_PRINTF("%s:LINE:%d\n",__FUNCTION__, __LINE__);

	if(p_cas_msg_struct->bType == CAS_MSG_MAIL)
	{
		p_cas_msg = &cas_mail_msg;
	}
	else if(p_cas_msg_struct->bType == CAS_MSG_ANNOUNCE)
	{
		p_cas_msg = &cas_announce_msg;
	}
	else	/* notification */
	{
      	return RET_SUCCESS;
	}
	if(p_cas_msg->cas_mail_headers.max_num >= FLASH_MSG_STORE_MAX_NUM)
	{
	    p_cas_msg_header = &p_cas_msg->cas_mail_headers.p_mail_head[0];
	    if(p_cas_msg_struct->bType == CAS_MSG_ANNOUNCE)
	        cas_tr_delete_msg_by_index(p_cas_msg_header->mail_id, CAS_MSG_ANNOUNCE);
	    else
	        cas_tr_delete_msg_by_index(p_cas_msg_header->mail_id, CAS_MSG_MAIL);
	}
	mtos_sem_take(&semaphore[p_cas_msg_struct->bType], 0);
#if 0 /* Remove the duplicate check because we have did it use cas_tr_find_duplicate_msg function */
	/* check msg whether has been received */
	p_cas_msg_body_link = p_cas_msg->p_cas_mail_body_link;

	while(p_cas_msg_body_link != NULL)
	{
		//OS_PRINTF("%s,LINE:%d,======p_cas_msg_body_link->cas_mail_store.wIndex=%d!\n", __FUNCTION__, __LINE__,p_cas_msg_body_link->cas_mail_store.wIndex);
		//OS_PRINTF("%s,LINE:%d,======p_cas_msg_struct->wIndex=%d!\n", __FUNCTION__, __LINE__,p_cas_msg_struct->wIndex);
		if(p_cas_msg_body_link->cas_mail_store.wIndex == p_cas_msg_struct->wIndex)
		{
			OS_PRINTF("%s,LINE:%d,%d ======msg has stored,will not store again!\n", __FUNCTION__, __LINE__,p_cas_msg_struct->bType);
			mtos_sem_give(&semaphore[p_cas_msg_struct->bType]);
			return RET_HAS_RECEIVED;
		}
		p_cas_msg_body_link = p_cas_msg_body_link->p_next;
	}
#endif

	/* store msg header msg */
	p_cas_msg_header = &p_cas_msg->cas_mail_headers.p_mail_head[p_cas_msg->cas_mail_headers.max_num];
	memcpy(p_cas_msg_header->subject, p_cas_msg_struct->bMsgTitle, p_cas_msg_struct->wTitleLen);
	memcpy(p_cas_msg_header->creat_date, &p_cas_msg_struct->sCreateTime, sizeof(CAS_TIMESTAMP));
	p_cas_msg_header->mail_id = p_cas_msg->cas_mail_headers.max_num;
	p_cas_msg_header->body_len = p_cas_msg_struct->wDataLen;
	p_cas_msg_header->priority = p_cas_msg_struct->bPriority;
	p_cas_msg_header->has_read = 0;
	p_cas_msg->cas_mail_headers.max_num += 1;

	//OS_PRINTF("%s:LINE:%d,p_cas_msg_header->subject=%s\n",__FUNCTION__, __LINE__,p_cas_msg_header->subject);
	//OS_PRINTF("%s:LINE:%d,p_cas_msg_header->body_len=%d\n",__FUNCTION__, __LINE__,p_cas_msg_header->body_len);
	//OS_PRINTF("%s:LINE:%d,p_cas_msg_header->priority=%d\n",__FUNCTION__, __LINE__,p_cas_msg_header->priority);
	//OS_PRINTF("%s:LINE:%d,p_cas_msg->cas_mail_headers.max_num=%d\n",__FUNCTION__, __LINE__,p_cas_msg->cas_mail_headers.max_num);

	/* store msg body */
	p_cas_msg_body_link = p_cas_msg->p_cas_mail_body_link;
	if(NULL == p_cas_msg_body_link)
	{
		p_cas_msg_body_link = mtos_malloc(sizeof(cas_mail_body_node_t));
		MT_ASSERT(NULL != p_cas_msg_body_link);
		memset(p_cas_msg_body_link, 0, sizeof(cas_mail_body_node_t));
		memcpy(p_cas_msg_body_link->cas_mail_store.cas_mail_body.data, p_cas_msg_struct->bMsgData, p_cas_msg_struct->wDataLen);
		p_cas_msg_body_link->cas_mail_store.cas_mail_body.mail_id = 0;
		p_cas_msg_body_link->cas_mail_store.wIndex = p_cas_msg_struct->wIndex;
		p_cas_msg_body_link->p_next = NULL;
		p_cas_msg->p_cas_mail_body_link = p_cas_msg_body_link;
		//OS_PRINTF("%s:LINE:%d,p_cas_msg_body_link->cas_mail_store.cas_mail_body.data=%s\n",__FUNCTION__, __LINE__,p_cas_msg_body_link->cas_mail_store.cas_mail_body.data);
		//OS_PRINTF("%s:LINE:%d,p_cas_msg_body_link->cas_mail_store.cas_mail_body.mail_id=%d\n",__FUNCTION__, __LINE__,p_cas_msg_body_link->cas_mail_store.cas_mail_body.mail_id);
	}
	else
	{
		while(p_cas_msg_body_link->p_next != NULL)
		{
			p_cas_msg_body_link = p_cas_msg_body_link->p_next;
		}

		p_cas_msg_body_node = mtos_malloc(sizeof(cas_mail_body_node_t));
		MT_ASSERT(NULL != p_cas_msg_body_node);
		memset(p_cas_msg_body_node, 0, sizeof(cas_mail_body_node_t));
		memcpy(p_cas_msg_body_node->cas_mail_store.cas_mail_body.data, p_cas_msg_struct->bMsgData, p_cas_msg_struct->wDataLen);
		p_cas_msg_body_node->cas_mail_store.cas_mail_body.mail_id =
				p_cas_msg_body_link->cas_mail_store.cas_mail_body.mail_id + 1;
		p_cas_msg_body_node->cas_mail_store.wIndex = p_cas_msg_struct->wIndex;
		p_cas_msg_body_link->p_next = p_cas_msg_body_node;
		p_cas_msg_body_node->p_next = NULL;
		//OS_PRINTF("%s:LINE:%d,p_cas_msg_body_node->cas_mail_store.cas_mail_body.data=%s\n",__FUNCTION__, __LINE__,p_cas_msg_body_node->cas_mail_store.cas_mail_body.data);
		//OS_PRINTF("%s:LINE:%d,p_cas_msg_body_node->cas_mail_store.cas_mail_body.mail_id=%d\n",__FUNCTION__, __LINE__,p_cas_msg_body_node->cas_mail_store.cas_mail_body.mail_id);
	}
	mtos_sem_give(&semaphore[p_cas_msg_struct->bType]);

	/* here, we need to flush to nvram */
	if(SUCCESS != cas_tr_flush_msg_to_nvram(p_cas_msg_struct->bType))
	{
		OS_PRINTF("%s:LINE:%d,flush to nvram failed!\n",__FUNCTION__, __LINE__);
		return RET_FLUSH_FAILED;
	}
	return RET_SUCCESS;
}
#ifdef INDENTIFY_CREATE_TINE
#define TOPREAL_MAIL_TAG (0x80)
static RET_CODE cas_tr_find_duplicate_msg(CAS_TIMESTAMP create_time, u8 type)
{
    u16 i = 0;
    CAS_TIMESTAMP *time_stamp = NULL;
    u8 type_tag = 0;

    if(type == CAS_MSG_MAIL)
        type_tag = TOPREAL_MAIL_TAG;

    for(i = 0; i < MSG_IDENTIFY_MAX_NUM; i++)
    {
        time_stamp = &cas_message_identify_msg[i];
        //OS_PRINTF("C_Time:%02d%02d-%02d-%02d %02d:%02d:%02d  C_Time:%02d%02d-%02d-%02d %02d:%02d:%02d\n",
        //     time_stamp->bYear[0], time_stamp->bYear[1], time_stamp->bMonth, time_stamp->bDay, time_stamp->bHour,
        //     time_stamp->bMinute, time_stamp->bSecond,
        //     create_time.bYear[0], create_time.bYear[1],  create_time.bMonth, create_time.bDay, create_time.bHour, create_time.bMinute, create_time.bSecond);

        if(time_stamp->bYear[0] == (create_time.bYear[0] | type_tag) && time_stamp->bYear[1] == create_time.bYear[1]
           && time_stamp->bMonth == create_time.bMonth && time_stamp->bDay == create_time.bDay
           && time_stamp->bHour == create_time.bHour && time_stamp->bMinute == create_time.bMinute
           && time_stamp->bSecond == create_time.bSecond)
        {
            return ERR_FAILURE;
        }
        if(time_stamp->bYear[0] == INVALID_INDEX)
            break;
    }
    if(i >= MSG_IDENTIFY_MAX_NUM)
        i = 0;
    time_stamp = &cas_message_identify_msg[i];
    time_stamp->bYear[0] = create_time.bYear[0] | type_tag;
    time_stamp->bYear[1] = create_time.bYear[1];
    time_stamp->bMonth = create_time.bMonth;
    time_stamp->bDay = create_time.bDay;
    time_stamp->bHour = create_time.bHour;
    time_stamp->bMinute = create_time.bMinute;
    time_stamp->bSecond = create_time.bSecond;
    if(i < MSG_IDENTIFY_MAX_NUM -1)
        cas_message_identify_msg[i+1].bYear[0]= INVALID_INDEX;
    if(SUCCESS != cas_tr_flush_msg_to_nvram(CAS_MSG_INDEX))
    {
	   OS_PRINTF("%s:LINE:%d, flush to nvram failed!\n", __FUNCTION__, __LINE__);
          return ERR_FAILURE;
    }
    return SUCCESS;
}
#else
static RET_CODE cas_tr_find_duplicate_msg(u16 index)
{
    u16 i;

    for(i = 0; i < MSG_IDENTIFY_MAX_NUM; i++)
    {
        if(cas_message_identify_msg[i] == index)
        {
            return ERR_FAILURE;
        }
        if(cas_message_identify_msg[i] == INVALID_INDEX)
            break;
    }
    if(i >= MSG_IDENTIFY_MAX_NUM)
        i = 0;
    cas_message_identify_msg[i] = index;
    if(i < MSG_IDENTIFY_MAX_NUM -1)
        cas_message_identify_msg[i+1] = INVALID_INDEX;
    if(SUCCESS != cas_tr_flush_msg_to_nvram(CAS_MSG_INDEX))
    {
	   OS_PRINTF("%s:LINE:%d, flush to nvram failed!\n", __FUNCTION__, __LINE__);
          return ERR_FAILURE;
    }
    return SUCCESS;
}
#endif
static void cas_tr_store_message(void *pData)
{
	RET_CODE ret = SUCCESS;
	u8 msg_num = 0;
	u8 uIndex = 0;
    u8 ctrl_type = CAS_MC_MAX_MESSAGE;
	u8 type = 0;
	u32 i,j = 0;

	cas_mail_msg_t *p_msg = NULL;
	cas_store_msg_ret_e cas_store_msg_ret = 0;
	CAS_MSG_STRUCT cas_msg[MSG_QUEUE_MAX_NUM] = {{0,},};
    cas_tr_priv_t *p_priv = g_cas_priv.cam_op[CAS_ID_TR].p_priv;

	while (1)
	{
		mtos_task_sleep(20);
		//OS_PRINTF("%s:LINE:%d,++++++!\n", __FUNCTION__, __LINE__);
		memset(cas_msg, 0, MSG_QUEUE_MAX_NUM * sizeof(CAS_MSG_STRUCT));
		ret = get_data_from_queue(cas_msg, &msg_num);
		if ((SUCCESS == ret) && (0 == msg_num))
		{
			//OS_PRINTF("%s:LINE:%d,get_data_from_queue failed!\n", __FUNCTION__, __LINE__);
			continue;
		}

		for (uIndex = 0; uIndex < msg_num; uIndex ++)
		{
			cas_store_msg_ret = cas_tr_store_new_short_msg(&cas_msg[uIndex]);
			if(RET_FLUSH_FAILED == cas_store_msg_ret)
			{
				OS_PRINTF("%s:LINE:%d,store flush new msg failed!\n", __FUNCTION__, __LINE__);
				continue;
			}
			else if(RET_HAS_RECEIVED == cas_store_msg_ret)
			{
				OS_PRINTF("%s:LINE:%d,new msg has received!\n", __FUNCTION__, __LINE__);
				continue;
			}

			type = cas_msg[uIndex].bType;
			if (type == CAS_MSG_NOTIFICATION)
			{
				//OS_PRINTF("%s:LINE:%d,++++++CAS_MSG_NOTIFICATION!\n", __FUNCTION__, __LINE__);
				i = cas_msg[uIndex].wIndex / 8;
				j = cas_msg[uIndex].wIndex % 8;

				mtos_sem_take(&semaphore[2], 0);
				if (test_bit((u32)j, (u32 *)notification_index[i]) == 1)
				{
					OS_PRINTF("%s:LINE:%d,new notification has received!\n", __FUNCTION__, __LINE__);
					mtos_sem_give(&semaphore[2]);
					continue;
				}
				set_bit((u32)j, (u32 *)notification_index[i]);
				memset(&cas_notification_msg, 0, sizeof(cas_notification_msg));
				cas_notification_msg.type = type;	/* notification */
				cas_notification_msg.class = cas_msg[uIndex].bClass;
				cas_notification_msg.index = cas_msg[uIndex].wIndex;
				cas_notification_msg.period = cas_msg[uIndex].wPeriod;
				cas_notification_msg.priority = cas_msg[uIndex].bPriority;
				memcpy(cas_notification_msg.title, cas_msg[uIndex].bMsgTitle, cas_msg[uIndex].wTitleLen);
				memcpy(cas_notification_msg.data, cas_msg[uIndex].bMsgData, cas_msg[uIndex].wDataLen);
				memcpy(cas_notification_msg.create_time, (u8 *)&cas_msg[uIndex].sCreateTime, sizeof(CAS_TIMESTAMP));
				mtos_sem_give(&semaphore[2]);

				ctrl_type = CAS_C_SHOW_OSD;

			}
			else if(type == CAS_MSG_MAIL)
			{
				//OS_PRINTF("%s:LINE:%d,++++++CAS_MSG_MAIL!\n", __FUNCTION__, __LINE__);
				p_msg = &cas_mail_msg;

				if (TRCAS_STORE_MAX_MAIL_NUM == p_msg->cas_mail_headers.max_num)
				{
					ctrl_type = CAS_C_SHOW_SPACE_EXHAUST;
				}
				else
				{
					ctrl_type = CAS_C_SHOW_NEW_EMAIL;
				}
			}
			else if(type == CAS_MSG_ANNOUNCE)
			{
				//OS_PRINTF("%s:LINE:%d,++++++CAS_MSG_ANNOUNCE!\n", __FUNCTION__, __LINE__);

				mtos_sem_take(&semaphore[2], 0);

				/* for announce pop display */
				memset(&cas_notification_msg, 0, sizeof(cas_notification_msg));
				cas_notification_msg.type = type;	/* notification */
				cas_notification_msg.class = cas_msg[uIndex].bClass;
				cas_notification_msg.index = cas_msg[uIndex].wIndex;
				cas_notification_msg.period = cas_msg[uIndex].wPeriod;
				cas_notification_msg.priority = cas_msg[uIndex].bPriority;
				memcpy(cas_notification_msg.title, cas_msg[uIndex].bMsgTitle, cas_msg[uIndex].wTitleLen);
				memcpy(cas_notification_msg.data, cas_msg[uIndex].bMsgData, cas_msg[uIndex].wDataLen);
				memcpy(cas_notification_msg.create_time, (u8 *)&cas_msg[uIndex].sCreateTime, sizeof(CAS_TIMESTAMP));

				mtos_sem_give(&semaphore[2]);

				p_msg = &cas_announce_msg;

				if (TRCAS_STORE_MAX_ANNOUNCE_NUM == p_msg->cas_mail_headers.max_num)
				{
					ctrl_type = CAS_ANNOUNCE_SHOW_SPACE_EXHAUST;
				}
				else
				{
					ctrl_type = CAS_C_SHOW_OSD;
				}
			}
			cas_send_event(p_priv->slot, CAS_ID_TR, ctrl_type, CAS_MC_NOTIFY_SHORT_MESSAGE);
		}
	}
}

/**
 *  To get short msg when topreal notify there new short msg.
 *
 *  param[in] p_cas_msg: point of short msg struct.
 *
 *  return
 *            if get short msg success, return SUCCESS, else
 *			  return event.
*/
static RET_CODE cas_tr_get_new_short_msg(CAS_MSG_STRUCT *p_cas_msg, CAS_MESSAGE_CONTROL eCaMsg, U16 wDemux)
{
	s16 ret = SUCCESS;
	s16 event = 1;

	//OS_PRINTF("%s:LINE:%d\n",__FUNCTION__, __LINE__);

	MT_ASSERT(NULL != p_cas_msg);

	memset(p_cas_msg, 0, sizeof(CAS_MSG_STRUCT));
	ret = MC_STATE_QueryControl(eCaMsg, wDemux, p_cas_msg);

	if(MC_OK != ret)
	{
		event = parse_event(TR_CONTROL_MSG_OPERATE_RET, ret);
	}

	if(ret != SUCCESS)
	{
		return event;
	}

	return SUCCESS;
}

/*!
  *
  * \param[in] The control information type
  * \param[in] The index of message. When eCaMsg is CAS_MC_IPP_INFO_UPDATE
  *                  this param indicates whether the current channel is IPP, 0 indicates no
  *                  1 indicates yes and shall call MC_STATE_QueryControl to get IPP info
  *                  by CAS_IPPNOTIFY_STRUCT and display the IPPV/IPPT indicator OSD.
  *                  If the eCaMsg is CAS_MC_IPP_NOTIFY and wDemux is 0, please check
  *                  if the CAS_IPPNOTIFY_STRUCT.wChannelID is same as the current
  *                  playing channel. If they are same, show the IPP purchasing menu, if
  *                  different, ignore the NOTIFY and clear existing menu.
  *                  If the eCaMsg is CAS_MC_IPP_NOTIFY, and wDemux is 1, it indicates
  *                  that the current chennel is IPPT and deduct from epurse one time automatically
  * \param[in] reserved for future using
  */
void cas_tr_ca_notify(CAS_MESSAGE_CONTROL eCaMsg, U16 wDemux, U32 ulParam)
{
    cas_tr_priv_t *p_priv = g_cas_priv.cam_op[CAS_ID_TR].p_priv;
	CAS_MSG_STRUCT *p_cas_msg = NULL;

    u8 ctrl_type = CAS_MC_MAX_MESSAGE;
	u8 event = CAS_E_UNKNOW_ERR;
	s16 ret = SUCCESS;
	u8 pointerIndex = 0;

    switch(eCaMsg)
    {
        case CAS_MC_DECODER_INFO:
            ctrl_type = CAS_C_DECODER_INFO;
            break;
        case CAS_MC_SERVICE_INFO:
            ctrl_type = CAS_C_SERVICE_INFO;
            break;
        case CAS_MC_SC_ACCESS:
            ctrl_type = CAS_C_SC_ACCESS;
            break;
        case CAS_MC_PIN_CODE_CHANGE:
            ctrl_type = CAS_C_PIN_CODE_CHANGE;
            break;
        case CAS_MC_PIN_CODE_CHECK:
            ctrl_type = CAS_C_PIN_CODE_CHECK;
            break;
        case CAS_MC_MONITOR_ECM:
            ctrl_type = CAS_C_MONITOR_ECM;
            break;
        case CAS_MC_MONITOR_EMM:
            ctrl_type = CAS_C_MONITOR_EMM;
            break;
        case CAS_MC_IPP_NOTIFY:
            if(wDemux == 0)
                ctrl_type = CAS_C_IPP_BUY_OR_NOT; //corresponding command CAS_IOCMD_IPP_NOTIFY_INFO_GET
            else
                ctrl_type = CAS_MC_MAX_MESSAGE;
            break;
        case CAS_MC_IPP_INFO_UPDATE:
            if(wDemux != 0)
                ctrl_type = CAS_C_IPP_BUY_OR_NOT; //CAS_C_IPP_INFO_UPDATE;
            break;
        case CAS_MC_FORCE_CHANNEL:
            ctrl_type = CAS_C_FORCE_CHANNEL;
            break;
        case CAS_MC_FINGER_PRINT:
            ctrl_type = CAS_C_SHOW_NEW_FINGER;
            break;
        case CAS_MC_EMM_DOWNLOAD:
            ctrl_type = CAS_C_EMM_DOWNLOAD;
            break;
        case CAS_MC_EMM_CHANGEPIN:
            ctrl_type = CAS_C_EMM_CHANGEPIN;
            break;
        case CAS_MC_NOTIFY_EXPIRY_STATE:
            ctrl_type = CAS_C_NOTIFY_EXPIRY_STATE;
            break;
        case CAS_MC_NOTIFY_CURRENT_STATE:
            ctrl_type = CAS_C_NOTIFY_CURRENT_STATE;
            break;
		case CAS_MC_MAIL:
        case CAS_MC_ANNOUNCE:
        case CAS_MC_NOTIFY_SHORT_MESSAGE:
           ctrl_type = CAS_C_NOTIFY_SHORT_MESSAGE;
			mtos_sem_take(&queue_semaphore, 0);
			ret = get_vacant_memory(&pointerIndex);
			if (SUCCESS != ret)
			{
				OS_PRINTF("%s:LINE:%d, queue is full!\n",__FUNCTION__, __LINE__);
				mtos_sem_give(&queue_semaphore);
				return;
			}

			if (NULL == p_msg_queue_node[pointerIndex])
			{
				OS_PRINTF("%s:LINE:%d, memory pointer is NULL!\n",__FUNCTION__, __LINE__);
				mtos_sem_give(&queue_semaphore);
				return;
			}
			mtos_sem_give(&queue_semaphore);

			p_cas_msg = &p_msg_queue_node[pointerIndex]->cas_msg_struct;
			ret = cas_tr_get_new_short_msg(p_cas_msg, eCaMsg, wDemux);
			if (SUCCESS != ret)
			{
				OS_PRINTF("%s:LINE:%d,cas_tr_get_new_short_msg failed!\n", __FUNCTION__, __LINE__);
				return;
			}
			if(p_cas_msg->bType == CAS_MSG_MAIL || p_cas_msg->bType == CAS_MSG_ANNOUNCE)
			{
			    OS_PRINTF("Message Type: %s Index:%d\n", p_cas_msg->bType == CAS_MSG_MAIL?"Mail":"Announce", p_cas_msg->wIndex);
				#ifdef INDENTIFY_CREATE_TINE
				if(cas_tr_find_duplicate_msg(p_cas_msg->sCreateTime, p_cas_msg->bType) == ERR_FAILURE)
				#else
			    if(cas_tr_find_duplicate_msg(p_cas_msg->wIndex) == ERR_FAILURE)
				#endif
	           {
	                 //OS_PRINTF("Get duplicate control message. Idx:%d\n", p_cas_msg->wIndex);
	                 return;
	            }
	       }
			p_msg_queue_node[pointerIndex]->used_flag = 1;	/* has used */
			p_msg_queue_node[pointerIndex]->next = NULL;

			ret = add_data_to_queue(p_cas_msg);
			if (SUCCESS != ret)
			{
				OS_PRINTF("%s:LINE:%d,add_data_to_queue failed!\n", __FUNCTION__, __LINE__);
				return;
			}
          break;
        case CAS_MC_MAX_MESSAGE:
        default:
            OS_PRINTF("Invalid message control type: %d\n", eCaMsg);
            break;
    }
    //OS_PRINTF("Control message type: %d\n", eCaMsg);
    p_priv->ctrl_num = 1;
    p_priv->ctrl.query = eCaMsg;
    p_priv->ctrl.demux = wDemux;
    p_priv->ctrl.param = ulParam;
    OS_PRINTF("CaMsg:%d wDemux:%d ulParam:%d\n", eCaMsg, wDemux, ulParam);
   if(CAS_MC_FINGER_PRINT!= eCaMsg && CAS_MC_FORCE_CHANNEL != eCaMsg
       && CAS_MC_IPP_INFO_UPDATE != eCaMsg && CAS_MC_NOTIFY_SHORT_MESSAGE != eCaMsg
       && ctrl_type != CAS_C_IPP_BUY_OR_NOT)
   {
       switch(wDemux)
       {
           case CAS_STATE_E00:  // Service not scrambled
               event = CAS_S_SEVICE_NOT_SCRAMBLED;
               break;
           case CAS_STATE_E01:  //
               event = CAS_S_CARD_MODULE_REMOVE;
               break;
           case CAS_STATE_E02:  //
               event = CAS_E_CARD_MODULE_EEPROM_FAILED;
               break;
           case CAS_STATE_E03:  //
               event = CAS_E_CARD_MODULE_FAIL;
               break;
           case CAS_STATE_E04:  // Please insert smart card
               event = CAS_S_CARD_REMOVE;
               break;
           case CAS_STATE_E05:  // Unknown smart card
               event = CAS_E_ILLEGAL_CARD;
               break;
           case CAS_STATE_E06:  // Smart card failure
               event = CAS_E_CARD_INIT_FAIL;
               break;
           case CAS_STATE_E07:  // Checking smart card
               event = CAS_S_CARD_CHECKING;
               break;
           case CAS_STATE_E08:  //
               event  = CAS_E_CARD_MODULE_FAIL;
               break;
           case CAS_STATE_E09:  // Smart card EEPROM failure
               event = CAS_E_CARD_EEPROM_FAILED;
               break;
           case CAS_STATE_E10:  //
               event = CAS_E_PMT_FORMAT_WRONG;
               break;
           case CAS_STATE_E11:  // Card marriage unmatched
               event = CAS_E_CARD_DIS_PARTNER;
               break;
           case CAS_STATE_E12:  // Please feed child smart card
               event = CAS_C_SON_CARD_NEED_FEED;
               break;
           case CAS_STATE_E13:  // No service available
               event = CAS_E_SEVICE_NOT_AVAILABLE;
               break;
           case CAS_STATE_E14:  // No authorization
               event = CAS_E_PROG_UNAUTH;
               break;
           case CAS_STATE_E15:  // Valid authorization availble
               event = CAS_S_PROG_AUTH_CHANGE;
               break;
           case CAS_STATE_E16:  // Service is currently scrambled
               event = CAS_E_SEVICE_SCRAMBLED;
               break;
           case CAS_STATE_E17:  // Service is currently scrambled
               event = CAS_E_SEVICE_SCRAMBLED2;
               break;
           case CAS_STATE_E18:  //
               event = CAS_E_ERROR_E18;
               break;
           case CAS_STATE_E19:  //
               event = CAS_E_ERROR_E19;
               break;
           case CAS_STATE_E20:  // Not allowed in this broadcaster
               event = CAS_E_NOT_ALLOWED_BROADCASTER;
               break;
           case CAS_STATE_E21:  //
               event = CAS_E_ERROR_E21;
               break;
           case CAS_STATE_E22:  //
               event = CAS_E_ERROR_E22;
               break;
           case CAS_STATE_E23:  // Service is currently descrambled
               event = CAS_S_SERVICE_DISCRAMBLING;
               break;
           case CAS_STATE_E24:  // Not allowed in this region
               event = CAS_E_NOT_ALLOWED_REGION;
               break;
           case CAS_STATE_E25:  // Smart card not compatible(AS)
               event = CAS_E_CARD_PARTNER_FAILED;
               break;
           case CAS_STATE_E26:  // Unknown service(No TOPREAL CA)
               event = CAS_E_SEVICE_UNKNOWN;
               break;
           case CAS_STATE_E27:  // Service is not currently running
              event = CAS_E_SEVICE_NOT_RUNNING;
              break;
           case CAS_STATE_E28:  // Smart card is locked
               event = CAS_E_CARD_LOCKED;
               break;
           case CAS_STATE_E29:  //
               event = CAS_E_ERROR_E29;
               break;
           case CAS_STATE_E30:  //
               event = CAS_E_ERROR_E30;
               break;
           case CAS_STATE_E31:  // Parental control locked
               event = CAS_E_PARENTAL_CTRL_LOCKED;
               break;
           case CAS_STATE_E32:  // Not allowed in this country
               event = CAS_E_NOT_ALLOWED_COUNTRY;
               break;
           case CAS_STATE_E33:  // No authorization data.
               event  = CAS_E_NO_AUTH_DATA;
               break;
           case CAS_STATE_E34:  // Illegal box
               event = CAS_E_NO_AUTH_STB;
               break;
           case CAS_STATE_E35:  // No Signal
               event = CAS_E_NO_SIGNAL;
               break;
           default:
               OS_PRINTF("wDemux:%d \n", wDemux);
               break;
       }
       cas_send_event(p_priv->slot, CAS_ID_TR, event, 0);
   }
    /* the third parameter tells the APP the type of control messages */
    if(CAS_MC_MAX_MESSAGE == ctrl_type)
        return;
    cas_send_event(p_priv->slot, CAS_ID_TR, ctrl_type, p_priv->ctrl.query);
    return;
}

static RET_CODE cas_adt_tr_init(s16 level)
{
   s16 ret;
/*!
  *  Initialize MCELL to start the TRCAS, it will create all tasks and semaphores.
  *
  * \param[in] The level of initializing MCELL. Normally is 0. The special card marriage
  *                 solution will be used if iLevel is set to 1.
  *
  * Return
  *    0 - Initialization is OK.
  *    1 - Failed to create the task whose Priority is 6.
  *    2 - Failed to initialize smart card.
  *    3 - Failed to initialize ECM driver.
  *    4 - Failed to initialize EMM driver.
  *    5 - Failed to initialize NVRAM driver.
  */

    ret = MC_CoreInit(level);
    OS_PRINTF("### %s %d\n", __FUNCTION__, __LINE__);
    if(ret != 0)
    {
        OS_PRINTF("TR CAS init failed, reason:%d\n", ret);
        return ERR_FAILURE;
    }

	MT_ASSERT(TRUE == mtos_sem_create(&semaphore[0], TRUE));	/* for mail */
	MT_ASSERT(TRUE == mtos_sem_create(&semaphore[1], TRUE));	/* for announce */
	MT_ASSERT(TRUE == mtos_sem_create(&semaphore[2], TRUE));	/* for notification */
	MT_ASSERT(TRUE == mtos_sem_create(&nvram_semaphore[0], TRUE));	/* for mail nvram */
	MT_ASSERT(TRUE == mtos_sem_create(&nvram_semaphore[1], TRUE));	/* for announce nvram */
	MT_ASSERT(TRUE == mtos_sem_create(&nvram_semaphore[3], TRUE));	/* for msg index nvram */
	MT_ASSERT(TRUE == mtos_sem_create(&queue_semaphore, TRUE));

    OS_PRINTF("### %s %d\n", __FUNCTION__, __LINE__);
    ret = MC_STATE_RegisterCaNotify((CAS_NOTIFY)cas_tr_ca_notify);

    if(ret != 0)
         ret= ERR_FAILURE;
    return SUCCESS;
}

static RET_CODE cas_adt_tr_io_ctrl(u32 cmd, void *param)
{
    s16 event = 0;
    S16 ret  = SUCCESS;
    u8 *rev = NULL;
    u32 index;

    cas_card_info_t *p_card_info = NULL;
    burses_info_t *p_epu_info = NULL;
    CAS_EPURSEINFO_STRUCT epurse;
    ipps_info_t *p_ipp_info = NULL;
    product_entitles_info_t *p_entitle_info = NULL;
    CAS_IPPPRODUCT_STRUCT ipp_product;

    CAS_SCARD_STATUS *p_sta_info = NULL;
    ipp_buy_info_t *p_purch_info = NULL;
    ipp_total_record_info_t *p_ipp_rec_info = NULL;
    CAS_IPPPURCHASE_STRUCT ipp_purchase;
    cas_channel_state_set *p_chan_state = NULL;
    cas_pin_modify_t *p_change_pin = NULL;
    cas_rating_set_t *p_rate = NULL;
    chlid_card_status_info *p_feed_info = NULL;
    CAS_FINGERPRINT_STRUCT finger;
    finger_msg_t *p_finger_info = NULL;
    CAS_FORCECHANNEL_STRUCT force_channel;
    cas_force_channel_t *p_force_channel_info = NULL;
    CAS_IPPNOTIFY_STRUCT ipp_notify;
//    ipp_info_t *p_ipp_info = NULL;
    CAS_MSG_STRUCT cas_msg;
    msg_info_t *p_cas_msg = NULL;
	cas_mail_headers_t *p_cas_mail_headers = NULL;
	cas_announce_headers_t *p_cas_announce_headers = NULL;
	cas_mail_body_t *p_cas_mail_body = NULL;
	cas_announce_body_t *p_cas_announce_body = NULL;
#if 1
    //ipp_buy_info_t *p_ips_info =NULL;
    CAS_IPPRECORD_STRUCT ipp_record;
#endif
    u16 region_code = 0;
	u32 msg_id = 0;
    cas_tr_priv_t *p_priv = g_cas_priv.cam_op[CAS_ID_TR].p_priv;

    switch(cmd)
    {
        case CAS_IOCMD_CARD_INFO_GET:
            p_card_info = (cas_card_info_t *)param;
            /* Get the string of the version information. The string buffer size
                 shouldn't be less than 26 bytes. */
            rev = MC_GetRevisionString();
            if(NULL == rev)
                break;
            index = 0;
            while(*rev != '\0')
            {
                p_card_info->cas_ver[index] = *rev;
                rev++;
                index++;
            }
            p_card_info->cas_ver[index] = '\0';
/*!
  * Read the card number from smart card.
  * \param[out] The smart card number pointer, the string buffer size should be larger than 24 bytes.
  * \param[in] The smart card number buffer to be copied.
  *
  * Return
  *    0 - OK
  *    1 - Not OK
  */
            ret = MC_GetScSerialNumber((char *)p_card_info->sn, CAS_CARD_SN_MAX_LEN);
            if(MC_OK != ret)
            {
                OS_PRINTF("Get smart card serail number failed. Error code:%d\n", ret);
                event = parse_event(TR_COMMON_OPERATE_RET, ret);
            }
/*!
  * Read the smart card version from smart card then write to string.
  *
  * \param[out] The string pointer of the smart card version. The buffer size
  *                    shouldn't be less than 7.
  * \param[in] The max length of string buffer, it should not be less than 7.
  *
  * Return
  *    0 - OK
  *    1 - Not OK
  */
           ret = MC_GetSmartCardVersion((char *)p_card_info->card_version, CAS_CARD_VER_MAX_LEN);
            if(MC_OK != ret)
            {
                OS_PRINTF("Get smart card version failed. Error code:%d\n", ret);
                event = parse_event(TR_COMMON_OPERATE_RET, ret);
            }
/*!
  * 0 - OK, 1 Not ok.
  */
            ret = MC_SCARD_GetRating(&(p_card_info->cas_rating));
            if(MC_OK != ret)
            {
                OS_PRINTF("Get smart card version failed. Error code:%d\n", ret);
                event = parse_event(TR_COMMON_OPERATE_RET, ret);
            }

            break;

        case CAS_IOCMD_BURSE_INFO_GET:
         p_epu_info = (burses_info_t *)param;
/*!
  * Get the electronic purse state which used for IPPT/IPPV.
  *
  * \param[out] psEpurseInfo - the electronic purse information.
  * \param[in] bIndex - the purse index.
  *
  * Return
  *    0 - OK
  *    1 - Not OK
  *    2 - Input parameter error
  *    5 - NOT_SUPPORTED functionality
  */
              ret  =  MC_GetEPurseState(&epurse, 0);
                if(MC_OK != ret)
               {
                    OS_PRINTF("Get Epurse state failed. Error code:%d\n", ret);
                    event = parse_event(TR_EPURSE_INFO_OPERATE_RET, ret);
                    break;
               }
              p_epu_info->p_burse_info[0].cash_value = epurse.ulCashValue;
              p_epu_info->p_burse_info[0].credit_limit = epurse.ulCreditValue;
              p_epu_info->p_burse_info[0].record_index = epurse.wRecordIndex;
              p_epu_info->max_num = 1;
              p_epu_info->index = 0;
              OS_PRINTF("cash:%d credit:%d index:%d\n", epurse.ulCashValue, epurse.ulCreditValue, epurse.wRecordIndex);
              break;

          case CAS_IOCMD_IPPV_INFO_GET:

            p_ipp_info = (ipps_info_t *)param;
            p_ipp_info->max_num = 0;

/*!
  * Read IPPT/IPPV information from SMC.
  *
  * \param[out] The IPPV/IPPT product information.
  * \param[in] The IPPV/IPPT product index saved in the smart card, it value range from 0 to 19.
  *
  * Return
  *    0 - OK
  *    1 - Not OK
  *    2 - Input parameter error
  *    3 - No valid product
  *    4 - Nvram error from smart card
  *    5 - NOT_SUPPORTED functionality
  */
               for (index = 0; index < TRCAS_IPP_NUM; index++)
               {
                 ret = MC_GetIppProduct(&ipp_product, index);
                  if(MC_OK != ret)
                 {
                      OS_PRINTF("Get IPP product info failed. Error code:%d\n", ret);
                      event = parse_event(TR_IPPV_INFO_OPERATE_RET, ret);
                      break;
                 }

                 p_ipp_info->p_ipp_info[p_ipp_info->max_num].index = index;
                 p_ipp_info->p_ipp_info[p_ipp_info->max_num].type = ipp_product.bProductType;
                 p_ipp_info->p_ipp_info[p_ipp_info->max_num].running_num = ipp_product.ulRunningNum;

                 memcpy(p_ipp_info->p_ipp_info[p_ipp_info->max_num].start_time,
                                        &(ipp_product.ulStartTime), sizeof(ipp_product.ulStartTime));
                 memcpy(p_ipp_info->p_ipp_info[p_ipp_info->max_num].expired_time,
                                        &(ipp_product.ulEndTime), sizeof(ipp_product.ulEndTime));
                 p_ipp_info->max_num++;
               }
               break;

          case CAS_IOCMD_IPP_EXCH_RECORD_GET:
               p_ipp_rec_info = (ipp_total_record_info_t *)param;
               memset(p_ipp_rec_info, 0, sizeof(ipp_total_record_info_t));
/*!
  * Read IPPT/IPPV consumptive record from SMC.
  *
  * \param[out] The IPPV/IPPT product consumptive record.
  * \param[in] The IPPV/IPPT product index saved in the smart card, it value range from 0 to 99.
  *
  * Return
  *    0 - OK
  *    1 - Not OK
  *    2 - Input parameter error
  *    3 - No valid product
  *    4 - Nvram error from smart card
  *    5 - NOT_SUPPORTED functionality
  */
               for (index = 0; index < CAS_MAX_IPP_RECORD_NUM; index++)
               {
                 ret = MC_GetIppRecord(&ipp_record, index);

                  if(MC_OK != ret)
                 {
                      OS_PRINTF("Get IPP record info failed. Error code:%d\n", ret);
                      event = parse_event(TR_IPPV_INFO_OPERATE_RET, ret);
                      break;
                 }

                 p_ipp_rec_info->record_info[p_ipp_rec_info->max_num].state_flag = ipp_record.bStateFlag;
                 p_ipp_rec_info->record_info[p_ipp_rec_info->max_num].index = index;
                 p_ipp_rec_info->record_info[p_ipp_rec_info->max_num].content_len = ipp_record.bContentLen;
                 p_ipp_rec_info->record_info[p_ipp_rec_info->max_num].value = ipp_record.ulExchValue;
                 p_ipp_rec_info->record_info[p_ipp_rec_info->max_num].runningNum = ipp_record.ulExchRunningNum;

                 memcpy(&p_ipp_rec_info->record_info[p_ipp_rec_info->max_num].start_time ,
                                &(ipp_record.ulExchTime), sizeof(ipp_record.ulExchTime));
                 memcpy(&p_ipp_rec_info->record_info[p_ipp_rec_info->max_num].content,
                                &ipp_record.pbContent,
                               (ipp_record.bContentLen < CAS_MAX_IPP_CONTENT_NUM) ? ipp_record.bContentLen : CAS_MAX_IPP_CONTENT_NUM);
                 p_ipp_rec_info->max_num++;
                 OS_PRINTF("index:%d state:%d value:%d rnum:%d\n", index, ipp_record.bStateFlag, ipp_record.ulExchValue, ipp_record.ulExchRunningNum);
               }
               break;

           case CAS_IOCMD_PRODUCT_TYPE_GET:
/*!
  * Read product type(PPC/PPV) from SMC.
  *
  * \param[out] The product type. 0 means PPC; 1 means PPV; 0xFF means have not get the type.
  *
  * Return
  *    0 - OK
  *    1 - Not OK
  */
               ret = MC_GetProductType((U8 *)param);
              if(MC_OK != ret)
              {
                  OS_PRINTF("Get product type failed. Error code:%d\n", ret);
                  event = parse_event(TR_COMMON_OPERATE_RET, ret);
              }
               break;

           case CAS_IOCMD_SMC_STATUS_GET:
               p_sta_info = (CAS_SCARD_STATUS *)param;
/*!
  * Read the card state from the smart card and return it by the pointer.
  *
  * \param[out] The smart card state. 0:CAS_SCARD_INSERT, 1: CAS_SCARD_REMOVE
  *                  2: CAS_SCARD_ERROR, 3: CAS_SCARD_UNKNOWN
  *
  * Return
  *    0 - OK
  *    1 - Not OK
  */
                if(MC_GetScStatus(p_sta_info) != MC_OK)
                {
                    OS_PRINTF("Get smart card status failed. Error code:%d\n", ret);
                    event = parse_event(TR_COMMON_OPERATE_RET, ret);
                }
                break;

       case CAS_IOCMD_ZONE_CODE_GET:
                ret =MC_SCARD_GetRegionCode((U16 *)param);
                if(MC_OK != ret)
                {
                    OS_PRINTF("Get region code failed. Error code:%d\n", ret);
                    event = parse_event(TR_COMMON_OPERATE_RET, ret);
                }
                break;
       case CAS_IOCMD_ENTITLE_INFO_GET:
                 p_entitle_info = (product_entitles_info_t *)param;
                 p_entitle_info->max_num = 0;
                 if(p_entitle_info->start_pos >= CAS_MAX_PRODUCT_ENTITLE_NUM)
                     break;
#if 1
                 /* Return value, 0: Valid entitle; 1: Input parameters error; 2: No entitle info;
                       3: Valid entitle, but limited; 4: Entitle closed; 5: Entitle paused;  */
                //OS_PRINTF("### %s start_pos:%d end_pos:%d \n", __FUNCTION__, p_entitle_info->start_pos, index < p_entitle_info->end_pos);
                for (index = p_entitle_info->start_pos; index < p_entitle_info->end_pos; index++)
                {
                  ret = MC_TIME_GetEntitleInfo(
                    (u8 *)&(p_entitle_info->p_entitle_info[index - p_entitle_info->start_pos].start_time),
                                            (u8 *)&(p_entitle_info->p_entitle_info[index - p_entitle_info->start_pos].expired_time),
                                            &(p_entitle_info->p_entitle_info[index - p_entitle_info->start_pos].remain_time), index);
				    p_entitle_info->p_entitle_info[index - p_entitle_info->start_pos].states = ret;
				    p_entitle_info->p_entitle_info[index - p_entitle_info->start_pos].product_id = index + 1;
				    p_entitle_info->max_num++;
				    //OS_PRINTF("#### %s %d product_id:%d max:%d\n", __FUNCTION__, __LINE__,
				    //    p_entitle_info->p_entitle_info[index - p_entitle_info->start_pos].product_id, p_entitle_info->max_num);
                  if(index >= CAS_MAX_PRODUCT_ENTITLE_NUM - 1)
                      break;
                 }
#endif
                 OS_PRINTF("Get Entitle Info finished.\n");
                 break;
       case CAS_IOCMD_FINGER_MSG_GET:
           p_finger_info = (finger_msg_t *)param;
           ret = MC_STATE_QueryControl(p_priv->ctrl.query, p_priv->ctrl.demux, &finger);
           if(MC_OK != ret)
           {
                event = parse_event(TR_CONTROL_MSG_OPERATE_RET, ret);
                break;
           }
		   memcpy(p_finger_info->data, finger.pbHashedNum, finger.bHashedNumLen);
           p_finger_info->during = 8;

           break;
       case CAS_IOCMD_FORCE_CHANNEL_INFO_GET:
           p_force_channel_info = (cas_force_channel_t *)param;
           force_channel.pbContent = p_force_channel_info->note;
           ret = MC_STATE_QueryControl(p_priv->ctrl.query, p_priv->ctrl.demux, &force_channel);
           if(MC_OK != ret)
           {
                event = parse_event(TR_CONTROL_MSG_OPERATE_RET, ret);
                break;
           }
           p_force_channel_info->lock_flag = force_channel.bLockFlag;
           p_force_channel_info->netwrok_id = force_channel.wNetwrokId;
           p_force_channel_info->serv_id= force_channel.wServId;
           p_force_channel_info->ts_id= force_channel.wTsId;
           p_force_channel_info->note[force_channel.wContentLen] = '\0';
           break;
       case CAS_IOCMD_IPP_BUY_INFO_GET:
           p_purch_info = (ipp_buy_info_t *)param;
           ret = MC_STATE_QueryControl(p_priv->ctrl.query, p_priv->ctrl.demux, &ipp_notify);
           if(MC_OK != ret)
           {
                event = parse_event(TR_CONTROL_MSG_OPERATE_RET, ret);
                break;
           }
		   memcpy(p_purch_info->note, ipp_notify.pbContent, ipp_notify.bContentLen);
           p_purch_info->channel_id = ipp_notify.wChannelId;
           p_purch_info->message_type = ipp_notify.bIppType;
           p_purch_info->ipp_charge = ipp_notify.ulIppCharge;
           p_purch_info->ipp_running_num = ipp_notify.ulIppRunningNum;
           memcpy(p_purch_info->expired_time, &(ipp_notify.ulIppEnd), sizeof(CAS_TIMESTAMP));
           memcpy(p_purch_info->start_time, &(ipp_notify.ulIppStart), sizeof(CAS_TIMESTAMP));
           p_purch_info->ipp_unit_time = ipp_notify.ulIppUnitTime;

           break;
       case CAS_IOCMD_SHORT_MSG_GET:
           p_cas_msg = (msg_info_t *)param;
           ret = MC_STATE_QueryControl(p_priv->ctrl.query, p_priv->ctrl.demux, &cas_msg);
           if(MC_OK != ret)
           {
                event = parse_event(TR_CONTROL_MSG_OPERATE_RET, ret);
                break;
           }
           memcpy(p_cas_msg->data, cas_msg.bMsgData, cas_msg.wDataLen);
           memcpy(p_cas_msg->title, cas_msg.bMsgTitle, cas_msg.wTitleLen);
           p_cas_msg->class = cas_msg.bClass;
           memcpy(p_cas_msg->create_time, &(cas_msg.sCreateTime), sizeof(CAS_TIMESTAMP));
           p_cas_msg->index = cas_msg.wIndex;
           p_cas_msg->priority = cas_msg.bPriority;
           p_cas_msg->type = cas_msg.bType;
           p_cas_msg->period= cas_msg.wPeriod;
           break;
       case CAS_IOCMD_PURCHASE_SET:
                p_purch_info = (ipp_buy_info_t *)param;
                ipp_purchase.wChannelId = p_purch_info->channel_id;
                ipp_purchase.ulIppCharge = p_purch_info->ipp_charge;
                ipp_purchase.ulIppRunningNum = p_purch_info->ipp_running_num;
                ipp_purchase.ulIppUnitTime = p_purch_info->ipp_unit_time;
                ipp_purchase.ulPurchaseNum = p_purch_info->purchase_num;
                ipp_purchase.bIppType = p_purch_info->message_type;

                ret = MC_IppPurchase(p_purch_info->pin, TRCAS_MAX_PIN_LEN,  ipp_purchase);
                if(MC_OK != ret)
                {
                    OS_PRINTF("Config IPP purchase failed. Error code:%d\n", ret);
                    event = parse_event(TR_PURCHASE_OPERATE_RET, ret);
                }
                break;

       case CAS_IOCMD_CHANNEL_STATE_SET:
                p_chan_state = (cas_channel_state_set *)param;
                MC_MNGR_PostChanChangeState(p_chan_state->channel_id, p_chan_state->state);
                break;

       case CAS_IOCMD_PIN_SET:
                p_change_pin = (cas_pin_modify_t *)param;
                ret = MC_SCARD_ChangePin(p_change_pin->old_pin, p_change_pin->new_pin, TRCAS_MAX_PIN_LEN);
                if(MC_OK != ret)
                {
                    OS_PRINTF("Change PIN code failed. Error code:%d\n", ret);
                    event = parse_event(TR_COMMON_OPERATE_RET, ret);
                }
                break;

       case CAS_IOCMD_RATING_SET:
                p_rate= (cas_rating_set_t *)param;
                ret = MC_SCARD_SetRating(p_rate->pin, TRCAS_MAX_PIN_LEN, p_rate->rate);
                if(MC_OK != ret)
                {
                    OS_PRINTF("Set rating info failed. Error code:%d\n", ret);
                    event = parse_event(TR_COMMON_OPERATE_RET, ret);
                }
                break;
        case CAS_IOCMD_RATING_GET:
          {
            u8 *p_rate = (u8 *)param;
            ret = MC_SCARD_GetRating(p_rate);
          }
          break;
       case CAS_IOCMD_PARENT_UNLOCK_SET:
                ret = MC_SCARD_ParentalCtrlUnlock((u8 *)param, TRCAS_MAX_PIN_LEN);
                if(MC_OK != ret)
                {
                    OS_PRINTF("Unlock parental control failed. Error code:%d\n", ret);
                    event = parse_event(TR_COMMON_OPERATE_RET, ret);
                }
                break;

       case CAS_IOCMD_ZONE_CODE_SET:
                region_code = *((u16 *)param);
                ret = MC_PostRegionCode(region_code);
                if(MC_OK != ret)
                {
                    OS_PRINTF("Set region code failed. Error code:%d\n", ret);
                    event = parse_event(TR_COMMON_OPERATE_RET, ret);
                }
                break;

       case CAS_IOCMD_MON_CHILD_STATUS_GET:
                p_feed_info = (chlid_card_status_info *)param;
                ret = MC_ReadFeedDataFromMaster(p_feed_info->feed_data, &(p_feed_info->length));
                if(MC_OK != ret)
               {
                    OS_PRINTF("Read feed data from master failed. Error code:%d\n", ret);
                    event = parse_event(TR_MASTER_SON_OPERATE_RET, ret);
                    break;
               }
                break;
       case CAS_IOCMD_MON_CHILD_STATUS_SET:
                p_feed_info = (chlid_card_status_info *)param;
                ret = MC_WriteFeedDataToSlaver(p_feed_info->feed_data, p_feed_info->length);
                if(MC_OK != ret)
               {
                    OS_PRINTF("Write feed data to slaver failed. Error code:%d\n", ret);
                    event = parse_event(TR_MASTER_SON_OPERATE_RET, ret);
                    break;
               }
                break;
	   case CAS_IOCMD_MAIL_HEADER_GET:
	   			p_cas_mail_headers = (cas_mail_headers_t *)param;
				OS_PRINTF("CAS_IOCMD_MAIL_HEADER_GET start : max_num[%d]\n", cas_mail_msg.cas_mail_headers.max_num);
				mtos_sem_take(&semaphore[0], 0);
				for(index=0; index<cas_mail_msg.cas_mail_headers.max_num; index++)
				{
					memcpy(p_cas_mail_headers->p_mail_head[index].subject,
						cas_mail_msg.cas_mail_headers.p_mail_head[index].subject,
						sizeof(cas_mail_msg.cas_mail_headers.p_mail_head[index].subject));
					memcpy(p_cas_mail_headers->p_mail_head[index].creat_date,
						cas_mail_msg.cas_mail_headers.p_mail_head[index].creat_date,
						sizeof(cas_mail_msg.cas_mail_headers.p_mail_head[index].creat_date));
					p_cas_mail_headers->p_mail_head[index].body_len = cas_mail_msg.cas_mail_headers.p_mail_head[index].body_len;
					p_cas_mail_headers->p_mail_head[index].priority = cas_mail_msg.cas_mail_headers.p_mail_head[index].priority;
					p_cas_mail_headers->p_mail_head[index].m_id = cas_mail_msg.cas_mail_headers.p_mail_head[index].mail_id;
                    if(cas_mail_msg.cas_mail_headers.p_mail_head[index].has_read == 0)
                      p_cas_mail_headers->p_mail_head[index].new_email = 1;
                    else
                      p_cas_mail_headers->p_mail_head[index].new_email = 0;
				}
				p_cas_mail_headers->max_num = cas_mail_msg.cas_mail_headers.max_num;
				mtos_sem_give(&semaphore[0]);
				OS_PRINTF("CAS_IOCMD_MAIL_HEADER_GET end \n");
				break;
	   case CAS_IOCMD_MAIL_BODY_GET:
	   	        p_cas_mail_body = (cas_mail_body_t *)param;
				ret = cas_tr_get_msg_body_by_index(p_cas_mail_body, p_cas_mail_body->mail_id, CAS_MSG_MAIL);
				if (ret != SUCCESS)
				{
					OS_PRINTF("%s:LINE:%d, get mail body failed!", __FUNCTION__, __LINE__);
				}
	   			break;
	   case CAS_IOCMD_ANNOUNCE_HEADER_GET:
				p_cas_announce_headers = (cas_announce_headers_t *)param;
				OS_PRINTF("CAS_IOCMD_ANNOUNCE_HEADER_GET start : max_num[%d]\n", cas_announce_msg.cas_mail_headers.max_num);
				mtos_sem_take(&semaphore[1], 0);
				for(index=0; index<cas_announce_msg.cas_mail_headers.max_num; index++)
				{
					memcpy(p_cas_announce_headers->p_mail_head[index].subject,
						cas_announce_msg.cas_mail_headers.p_mail_head[index].subject,
						sizeof(cas_announce_msg.cas_mail_headers.p_mail_head[index].subject));
					memcpy(p_cas_announce_headers->p_mail_head[index].creat_date,
						cas_announce_msg.cas_mail_headers.p_mail_head[index].creat_date,
						sizeof(cas_announce_msg.cas_mail_headers.p_mail_head[index].creat_date));
					p_cas_announce_headers->p_mail_head[index].body_len = cas_announce_msg.cas_mail_headers.p_mail_head[index].body_len;
					p_cas_announce_headers->p_mail_head[index].priority = cas_announce_msg.cas_mail_headers.p_mail_head[index].priority;
					p_cas_announce_headers->p_mail_head[index].m_id = cas_announce_msg.cas_mail_headers.p_mail_head[index].mail_id;
                    if(cas_announce_msg.cas_mail_headers.p_mail_head[index].has_read == 0)
                      p_cas_announce_headers->p_mail_head[index].new_email = 1;
                    else
                      p_cas_announce_headers->p_mail_head[index].new_email = 0;
				}
				p_cas_announce_headers->max_num = cas_announce_msg.cas_mail_headers.max_num;
				mtos_sem_give(&semaphore[1]);
				OS_PRINTF("CAS_IOCMD_ANNOUNCE_HEADER_GET end \n");
	   			break;
	   case CAS_IOCMD_ANNOUNCE_BODY_GET:
				p_cas_announce_body = (cas_announce_body_t *)param;
				ret = cas_tr_get_msg_body_by_index(p_cas_announce_body, p_cas_announce_body->mail_id, CAS_MSG_ANNOUNCE);
				if (ret != SUCCESS)
				{
				   OS_PRINTF("%s:LINE:%d, get announce body failed!", __FUNCTION__, __LINE__);
				}
	   		    break;
	   case CAS_IOCMD_MAIL_DELETE_BY_INDEX:
	   	        msg_id = *(u32 *)param;
				ret = cas_tr_delete_msg_by_index(msg_id, CAS_MSG_MAIL);
				if (ret != SUCCESS)
				{
				   OS_PRINTF("%s:LINE:%d, delete mail %d body failed!", __FUNCTION__, __LINE__, msg_id);
				}
	            break;
	   case CAS_IOCMD_MAIL_DELETE_ALL:
	   			ret = cas_tr_delete_all_msg(CAS_MSG_MAIL);
				if (ret != SUCCESS)
				{
				   OS_PRINTF("%s:LINE:%d, delete all mail body failed!", __FUNCTION__, __LINE__);
				}
	   	 		break;
	   case CAS_IOCMD_ANNOUNCE_DELETE_BY_INDEX:
				msg_id = *(u32 *)param;
				ret = cas_tr_delete_msg_by_index(msg_id, CAS_MSG_ANNOUNCE);
				if (ret != SUCCESS)
				{
				  OS_PRINTF("%s:LINE:%d, delete announce %d body failed!", __FUNCTION__, __LINE__, msg_id);
				}
	   		    break;
	   case CAS_IOCMD_ANNOUNCE_DELETE_ALL:
				ret = cas_tr_delete_all_msg(CAS_MSG_ANNOUNCE);
				if (ret != SUCCESS)
				{
				  OS_PRINTF("%s:LINE:%d, delete all announce body failed!", __FUNCTION__, __LINE__);
				}
	   			break;
	   case CAS_IOCMD_OSD_GET:
	   			p_cas_msg = (msg_info_t *)param;
				mtos_sem_take(&semaphore[2], 0);
				memcpy(p_cas_msg, &cas_notification_msg, sizeof(cas_notification_msg));
				mtos_sem_give(&semaphore[2]);
	   			break;
            default:
                ret = ERR_FAILURE;
      }

      if(ret != 0)
            return event;

       return SUCCESS;
}

static RET_CODE cas_adt_tr_deinit()
{
  	return SUCCESS;
}
extern BOOL ui_play_reset_pmt_pid(void);
static RET_CODE cas_adt_tr_card_reset(u32 slot, card_reset_info_t *p_info)
{
  s16 ret = 0;

  if (NULL == p_info)
      return ERR_FAILURE;
  OS_PRINTF("## %s %d \n", __FUNCTION__, __LINE__);
  cas_tr_priv_t *p_priv = g_cas_priv.cam_op[CAS_ID_TR].p_priv;

  scard_pro_register_op((scard_device_t *)p_info->p_smc);

  p_priv->card_status = SMC_CARD_INSERT;
  ret = SC_ATR_Notify_fun(0, SMC_CARD_INSERT, p_info->p_atr->p_buf, SMC_PROTOCOL_T0);
  ui_play_reset_pmt_pid();
  OS_PRINTF("Card inserted. \n");
  return SUCCESS;
}

static RET_CODE cas_adt_tr_card_remove(void)
{
  cas_tr_priv_t *p_priv  = (cas_tr_priv_t *)g_cas_priv.cam_op[CAS_ID_TR].p_priv;
  scard_atr_desc_t atr_desc = {0};
  s16 ret;
  OS_PRINTF("## %s %d \n", __FUNCTION__, __LINE__);
  p_priv->card_status = SMC_CARD_REMOVE;


  ret = SC_ATR_Notify_fun(0, SMC_CARD_REMOVE, atr_desc.p_buf, SMC_PROTOCOL_T0);
  OS_PRINTF("Card removed. \n");
  return SUCCESS;
}


static RET_CODE cas_adt_tr_identify(u16 ca_sys_id)
{
  //if((ca_sys_id >= 0x4a40) && (ca_sys_id <= 0x4a4f))
  {
    return SUCCESS;
  }

  return ERR_FAILURE;
}
static RET_CODE cas_adt_tr_table_process(u32 t_id, u8 *p_buf, u32 *p_result)
{
   u32 table_id = 0;
   OS_PRINTF("### Table:%s\n", t_id == CAS_TID_PMT ?"PMT":"CAT");
   MT_ASSERT( NULL != p_buf);
   switch(t_id)
   {
       case CAS_TID_PMT:
            table_id = 1;
            break;
       case CAS_TID_CAT:
            table_id = 0;
            break;
       default:
            OS_PRINTF("Topreal driver does not support specified table.\n");
            return ERR_FAILURE;
   }
   /* In topreal driver, CAT is 0, PMT is 1 */
   *p_result = MC_MNGR_PostPsiTable(table_id, p_buf);
   if(MC_OK != *p_result)
   {
        OS_PRINTF("Set PSI table failed. Error code:%d\n", *p_result);
        return ERR_FAILURE;
   }
    return SUCCESS;
}


RET_CODE cas_tr_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id)
{
  s16 ret;
  u8 uIndex = 0;
  s_msg_queue_node_t *p_msg_queue = NULL;
  u32 *p_stack = NULL;
  u32 queue_task_prio = 0;

  g_cas_priv.cam_op[CAS_ID_TR].attached = 1;
  g_cas_priv.cam_op[CAS_ID_TR].inited = 0;

  g_cas_priv.cam_op[CAS_ID_TR].func.init = NULL;
  g_cas_priv.cam_op[CAS_ID_TR].func.deinit
    = cas_adt_tr_deinit;
  g_cas_priv.cam_op[CAS_ID_TR].func.card_reset
    = cas_adt_tr_card_reset;
  g_cas_priv.cam_op[CAS_ID_TR].func.card_remove
    = cas_adt_tr_card_remove;
  g_cas_priv.cam_op[CAS_ID_TR].func.io_ctrl
    = cas_adt_tr_io_ctrl;
  g_cas_priv.cam_op[CAS_ID_TR].func.identify
    = cas_adt_tr_identify;
  g_cas_priv.cam_op[CAS_ID_TR].func.table_process
    = cas_adt_tr_table_process;

  tr_priv = mtos_malloc(sizeof(cas_tr_priv_t));
  MT_ASSERT(NULL != tr_priv);
  memset(tr_priv, 0x00, sizeof(cas_tr_priv_t));
  if(NULL != p_cfg->nvram_read)
      tr_priv->nv_read = p_cfg->nvram_read;
  if(NULL != p_cfg->nvram_write)
      tr_priv->nv_write = p_cfg->nvram_write;
  g_cas_priv.cam_op[CAS_ID_TR].p_priv = tr_priv;

  if(TR_MAX_TASK_NUM <= p_cfg->end_task_prio-p_cfg->task_prio)
  {
	OS_PRINTF("%s:LINE:%d, assin task priority id is not 10!\n", __FUNCTION__, __LINE__);
	MT_ASSERT(0);
	
  }
  tr_priv->task_pri = p_cfg->task_prio;

   tr_priv->ctrl_num= 0;
  *p_cam_id = (u32)(&g_cas_priv.cam_op[CAS_ID_TR]);
  ret = cas_adt_tr_init(p_cfg->level);
  MT_ASSERT(SUCCESS == ret);
  OS_PRINTF("Topreal CAS attached.\n");

  /* malloc msg queue 20 nodes */
  p_msg_queue = mtos_malloc(MSG_QUEUE_MAX_NUM * sizeof(s_msg_queue_node_t));
  MT_ASSERT(NULL != p_msg_queue);
  for (uIndex = 0; uIndex < MSG_QUEUE_MAX_NUM; uIndex ++)
  {
    p_msg_queue_node[uIndex] = p_msg_queue;
    memset(p_msg_queue_node[uIndex], 0, sizeof(s_msg_queue_node_t));
    p_msg_queue ++;
  }
  #ifdef INDENTIFY_CREATE_TINE
  memset(cas_message_identify_msg, 0xff, MSG_IDENTIFY_MAX_NUM * sizeof(CAS_TIMESTAMP));
  #endif
  ret = cas_tr_get_msg_from_nvram(CAS_MSG_MAIL);
  ret = cas_tr_get_msg_from_nvram(CAS_MSG_ANNOUNCE);
  ret = cas_tr_get_msg_from_nvram(CAS_MSG_INDEX);

  OS_PRINTF("%s:LINE:%d, get mail/announce from nvram success!\n", __FUNCTION__, __LINE__);
  if (0 != p_cfg->queue_task_prio)
  {
  	queue_task_prio = p_cfg->queue_task_prio;
  }
  else
  {
  	queue_task_prio = STORE_MSG_TASK_PRIO;
  }
  p_stack = (u32*)mtos_malloc(STORE_MSG_TASK_STACK_SIZE);
  MT_ASSERT(NULL != p_stack);
  ret = mtos_task_create((u8 *)"MessageStoreTask",
			cas_tr_store_message,
			(void *)0,
			queue_task_prio,
			p_stack,
			STORE_MSG_TASK_STACK_SIZE);
  if (!ret)
  {
	  OS_PRINTF("%s:LINE:%d, Create message store task failed = 0x%08x!\n", __FUNCTION__, __LINE__, ret);
	  return ERR_FAILURE;
  }

  return SUCCESS;
}

