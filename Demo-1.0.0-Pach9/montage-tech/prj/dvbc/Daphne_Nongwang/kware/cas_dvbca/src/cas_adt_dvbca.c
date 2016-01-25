/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

#include "dvbca_interface.h"

#include <string.h>

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
#include "dmx.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "dvbca_cas_adapter.h"
#include "dvbca_types.h"

#include "mtos_misc.h"

#include "sys_dbg.h"
//#include "ui_ca_character_code.h"
#define SN_RELEASE_TIME	      __TIME__
#define STB_SERIAL_LEN				(10)
#define CAS_EMAIL_STORE_MAX_NUM		(8)
#define DVBCA_MAXLEN_MSG_TEXT		(4096)	

ST_DVBCA_MSG cas_osd_msg_t;

extern cas_adapter_priv_t g_cas_priv;
u8 watch_level = 0;


extern RET_CODE dvbca_stb_drv_adt_init(void);
extern void dvbcas_stb_drv_careqlock(void);
extern void dvbcas_stb_drv_carequnlock(void);

typedef enum
{
/*!
    ABC
  */
  SC_REMOVE_START,
  /*!
    ABC
  */
  SC_REMOVE_OVER,
  /*!
    ABC
  */
  SC_INSERT_START,
    /*!
    ABC
  */
  SC_INSERT_OVER,
  /*!
    ABC
  */
  SC_STATUS_END
}dvbcas_smartcard_status_t;

dvbcas_smartcard_status_t dvbcas_uScStatus = SC_STATUS_END;

static s32 dvbca_ca_desc_parse(u8 *pBuf, s32 nDescLen, dvbca_ca_desc_t *pDesc)
{
	u8 *p_data = pBuf;

	DEBUG(DVBCAS_PORTING,INFO, "call in!\n");

	if((NULL != pBuf) && (NULL != pDesc))
	{
		pDesc->ca_system_id = (p_data[0] << 8) | p_data[1];
		pDesc->ca_pid = ((p_data[2]&0x1F) << 8) | p_data[3];
		DEBUG(DVBCAS_PORTING,INFO, "ca_system_id = 0x%x ca_pid=%d\n", pDesc->ca_system_id, pDesc->ca_pid);

		return SUCCESS;
	}
	return ERR_FAILURE;
}/*dvbca_ca_desc_parse*/

static void dvbca_pmt_parse_descriptor(u8 *buf, s16 length,u16 program_number)
{
	u8 *p_data = buf;
	u8 descriptor_tag = 0;
	u8 descriptor_length = 0;

	dvbca_ca_desc_t ca_desc = {0};
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;

	while (length > 0)
	{
		descriptor_tag = p_data[0];
		descriptor_length = p_data[1];
		DEBUG(DVBCAS_PORTING,INFO, "descriptor_tag=0x%x\n", descriptor_tag);

		if(0 == descriptor_length)
		{
			break;
		}

		length -= 2;
		p_data += 2;
		switch (descriptor_tag)
		{
			case 0x09:	/* DVB_DESC_CA */
			{
				dvbca_ca_desc_parse(p_data, descriptor_length, &ca_desc);

				DEBUG(DVBCAS_PORTING,INFO, "ecm pid = 0x%x, system_id=0x%x\n", ca_desc.ca_pid, ca_desc.ca_system_id);
				if(DVBCA_SUCESS == DVBCASTB_IsDVBCA(ca_desc.ca_system_id))
				{
					if(priv->cur_ecm_pid == ca_desc.ca_pid)
					{
						return;
					}
					priv->cur_ecm_pid = ca_desc.ca_pid;

					DEBUG(DVBCAS_PORTING,INFO, "DVBCASTB_SetEcmPid: 0x%x!\n", priv->cur_ecm_pid);
					DVBCASTB_SetEcmPid(&(priv->cur_ecm_pid), 1);
				}
				else
				{
					DEBUG(DVBCAS_PORTING,INFO, "not DVBCA!\n");
				}
			}
			break;
			default:
			break;
		}

		length -= descriptor_length;
		p_data = p_data + descriptor_length;
	}
}/*dvbca_pmt_parse_descriptor*/

static void dvbca_cas_parse_pmt(u8 *p_buf)
{
	u8 table_id = 0;
	s16 section_length = 0;
	u16 program_number = 0;
	u8 version_number = 0;
	u16 pcr_pid = 0;
	u16 program_info_length = 0;
	u8 stream_type = 0;
	u16 elementary_pid = 0;
	u8 es_info_length = 0;
	u8 *p_data = NULL;
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	priv->cur_ecm_pid = 0;

	DEBUG(DVBCAS_PORTING,INFO,"call in!\n");

	MT_ASSERT(p_buf != NULL);

	p_data = p_buf;

	if(NULL != p_data)
	{
		table_id = p_data[0];
		section_length = (((u16)p_data[1] << 8) | p_data[2]) & 0x0FFF;
		if(section_length > 0)
		{
			program_number = (u16)p_data[3] << 8 | p_data[4];
			version_number = (p_data[5] & 0x3E) >> 1;
			pcr_pid = ((u16)p_data[8] << 8 | p_data[9]) & 0x1FFF;

			program_info_length = ((u16)p_data[10] << 8 | p_data[11]) & 0x0FFF;
			p_data += 12;

			dvbca_pmt_parse_descriptor(p_data, program_info_length,program_number);
			p_data += program_info_length;
			section_length -= (13 + program_info_length);//head and crc
			DEBUG(DVBCAS_PORTING,INFO, "[DVBCA]stream_type=0x%x \n",p_data[0]);

			while(section_length > 0)
			{
				stream_type = p_data[0];
				elementary_pid = ((u16)p_data[1] << 8 | p_data[2]) & 0x1FFF;
				DEBUG(DVBCAS_PORTING,INFO, "stream_type=0x%x, elementary_PID=0x%x\n", stream_type, elementary_pid);
				es_info_length = ((u16)p_data[3] << 8 | p_data[4]) & 0x0FFF;
				p_data += 5;
				dvbca_pmt_parse_descriptor(p_data, es_info_length,program_number);
				p_data += es_info_length;
				section_length -= (es_info_length + 5);
			}
		}
	}

}/*dvbca_cas_parse_pmt*/

static void dvbca_cat_parse_descriptor(u8 *buf, s16 length)
{
	u8 *p_data = buf;
	u8	descriptor_tag = 0;
	u8	descriptor_length = 0;
	u16 ca_system_id = 0;
	u16 emm_pid = 0;

	DEBUG(DVBCAS_PORTING,INFO, "call in!\n");
	while (length > 0)
	{
		descriptor_tag = p_data[0];
		descriptor_length = p_data[1];
		p_data += 2;
		length -= 2;
		if (descriptor_length == 0)
		{
			continue;
		}
		switch (descriptor_tag)
		{
			case 0x09:	/* DVB_DESC_CA */
			{
				ca_system_id = MAKE_WORD(p_data[1], p_data[0]);
				emm_pid = MAKE_WORD(p_data[3],p_data[2]&0x1F);
				if (DVBCA_SUCESS == DVBCASTB_IsDVBCA(ca_system_id))
				{
					DEBUG(DVBCAS_PORTING,INFO, "emm pid =0x%x,ca_system_id =0x%x\n", emm_pid, ca_system_id);
					DVBCASTB_SetEmmPid(emm_pid);
				}
				else
				{
					DEBUG(DVBCAS_PORTING,INFO, "NOT DVBCA!\n");
				}
			}
			break;
			default:
			break;
		}
		length -= descriptor_length;
		p_data = p_data + descriptor_length;
	}

}/*dvbca_cat_parse_descriptor*/

static void dvbca_cas_parse_cat(u8 *p_buf)
{
	u8 *p_data = p_buf;
	u8 version_number = 0;
	u16 length = 0;

	DEBUG(DVBCAS_PORTING,INFO, "call in!\n");
	version_number = (u8)((p_data[5] & 0x3E) >> 1);
	length = (u16)(((p_data[1] & 0x0f) << 8) | p_data[2]);
	DEBUG(DVBCAS_PORTING,INFO, "length=%d,version_number=%d\n", length, version_number);

	p_data += 8;
	dvbca_cat_parse_descriptor(p_data, length - 9);

	DEBUG(DVBCAS_PORTING,INFO, "parsing cat over.\n");

}/*dvbca_cas_parse_cat*/


static RET_CODE cas_adt_dvbca_init()
{
	static BOOL init_flag = FALSE;
	int ret = 0;
	u8 task_prio = 35;
	u8 temp_buffer[CAS_CARD_SN_MAX_LEN + 1] = "80000031";
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	DEBUG(DVBCAS_PORTING,INFO,"call in!\n");
	priv->used_chV = 0xffff;
	priv->used_chA = 0xffff;
	priv->reset_flag = 0;
	priv->card_status = SMC_CARD_REMOVE;

	mtos_sem_create(&(priv->dvbcas_lock),TRUE);
	if (init_flag)
	{
		return SUCCESS;
	}
	if (0 != priv->task_priv.task_prio)
    {
		task_prio = priv->task_priv.task_prio;
    }
    else
    {
		task_prio = DVBCA_TASK_PRIORITY_0;
    }
	if(!init_flag)
    {
    	ret = dvbca_stb_drv_adt_init();
		if (SUCCESS != ret)
		{
			DEBUG(DVBCAS_PORTING, INFO, "stb drv init return error! ret=%d\n",ret);
			return DVBCA_FAILED;
		}
    	OS_PRINTF("[dvbcas] %s %d ! \n",__func__,__LINE__);
		cas_get_stb_serial(CAS_ID_DVBCA, temp_buffer);
		OS_PRINTF("[dvbcas] %s %d ! stb_serial=%s\n",__func__,__LINE__,temp_buffer);
		ret = DVBCASTB_Init(task_prio, temp_buffer);
		OS_PRINTF("[dvbcas] %s %d ! \n",__func__,__LINE__);
		if (ret != DVBCA_SUCESS)
		{
			DEBUG(DVBCAS_PORTING, INFO, "DVBCASTB_Init return error! ret=%d\n", ret);
			return DVBCA_FAILED;
		}
		mtos_task_sleep(300);
		init_flag = TRUE;
    }
	DEBUG(DVBCAS_PORTING,INFO,"dvbca init success!\n");
	return SUCCESS;
}/*cas_adt_dvbca_init*/

static RET_CODE cas_adt_dvbca_deinit()
{
	DEBUG(DVBCAS_PORTING,INFO,"call in!\n");
  return SUCCESS;
}/*cas_adt_dvbca_deinit*/

static void dvbcas_set_sc_insert_status(u8 uStatus)
{
	dvbcas_uScStatus = uStatus;
}

static RET_CODE cas_adt_dvbca_card_reset(u32 slot, card_reset_info_t *p_info)
{
	int ret = 0;
	if (NULL == p_info)
    {
        DEBUG(DVBCAS_PORTING,INFO,"p_info is NULL!\n");
        return ERR_FAILURE;
    }
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	DEBUG(DVBCAS_PORTING,INFO,"call in!\n");
	priv->p_smc_dev = (scard_device_t *)p_info->p_smc;
	scard_pro_register_op((scard_device_t *)p_info->p_smc);
	priv->slot = slot;

	priv->card_status = SMC_CARD_INSERT;
	dvbcas_set_sc_insert_status(SC_INSERT_START);
	priv->reset_flag = 1;
	ret = DVBCASTB_SCInsert();
	if (ret != DVBCA_SUCESS)
	{
		DEBUG(DVBCAS_PORTING, INFO, "DVBCASTB_SCInsert failed! ret=%d\n", ret);
		return DVBCA_FAILED;
	}
	dvbcas_set_sc_insert_status(SC_INSERT_OVER);
	DEBUG(DVBCAS_PORTING,INFO,"card reset over! \n");
	return SUCCESS;
}/*cas_adt_dvbca_card_reset*/

static RET_CODE cas_adt_dvbca_card_remove()
{
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	priv->card_status = SMC_CARD_REMOVE;
	cas_send_event(priv->slot, CAS_ID_DVBCA, CAS_S_CARD_REMOVE, 0);
	dvbcas_set_sc_insert_status(SC_REMOVE_START);
	DVBCASTB_SCRemove();
	priv->reset_flag = 0;
	dvbcas_set_sc_insert_status(SC_REMOVE_OVER);
	return SUCCESS;
}/*cas_adt_dvbca_card_remove*/

static RET_CODE cas_adt_dvbca_identify(u16 ca_sys_id)
{
	RET_CODE ret = 0;
	DEBUG(DVBCAS_PORTING,INFO,"call in!\n");
	ret = DVBCASTB_IsDVBCA(ca_sys_id);
	if(ret != TRUE)
	{
		return DVBCA_FAILED;
	}
	DEBUG(DVBCAS_PORTING,INFO,"dvbca identify over!\n");
	return DVBCA_SUCESS;
}/*cas_adt_dvbca_identify*/

static RET_CODE cas_adt_dvbca_table_process(u32 t_id, u8 *p_buf, u32 *p_result)
{

	s_cas_dvbca_priv_t *p_priv = NULL;
	u16 section_len = 0;
    DEBUG(DVBCAS_PORTING,INFO,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>cas_adt_dvbca_table_process\n");
	DEBUG(DVBCAS_PORTING,INFO,"call in!\n");
	section_len = MAKE_WORD(p_buf[2], p_buf[1] & 0xF) + 3;

	p_priv = (s_cas_dvbca_priv_t *)g_cas_priv.cam_op[CAS_ID_DS].p_priv;

	if (CAS_TID_PMT == t_id)
	{
		OS_PRINTF("[DVBCA] pmt process\n");
		memcpy(g_cas_priv.pmt_data, p_buf, section_len);
		g_cas_priv.pmt_length = section_len;
		dvbca_cas_parse_pmt(g_cas_priv.pmt_data);
	}
	else if (CAS_TID_CAT == t_id)
	{
		OS_PRINTF("[DVBCA] cat process\n");
		memcpy(g_cas_priv.cat_data, p_buf, section_len);
		g_cas_priv.cat_length = section_len;
		dvbca_cas_parse_cat(g_cas_priv.cat_data);
	}
	else if (CAS_TID_NIT == t_id)
	{
		OS_PRINTF("[DVBCA] nit process\n");
		memcpy(g_cas_priv.nit_data, p_buf, section_len);
		g_cas_priv.nit_length = section_len;
		p_priv->private_data_specifier = (u32)((((u16)(g_cas_priv.nit_data[3])) << 8)
		| (g_cas_priv.nit_data[4]));
		DVBCASTB_NitDataGot(g_cas_priv.nit_data, section_len);
	}
  DEBUG(DVBCAS_PORTING,INFO,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>cas_adt_dvbca_table_process\n");
  return SUCCESS;
}/*cas_adt_dvbca_table_process*/

const char * _cas_ioctl2str(cas_ioctrl_cmd_t ioctl)
{
	switch(ioctl)
	{
		case CAS_IOCMD_HOTPLUG_INFO_GET :
			return "CAS_IOCMD_HOTPLUG_INFO_GET";
		case CAS_IOCMD_GET_MAIL_MAX:
			return "CAS_IOCMD_GET_MAIL_MAX";
		case CAS_IOCMD_GET_MAIL_POLICY:
			return "CAS_IOCMD_GET_MAIL_POLICY";
		case CAS_IOCMD_GET_CURRENT_MAIL:
			return "CAS_IOCMD_GET_CURRENT_MAIL";
		case CAS_IOCMD_GET_SERIAL_DATA:
			return "CAS_IOCMD_GET_SERIAL_DATA";
		case CAS_IOCMD_CA_SYS_ID_GET:
			return "CAS_IOCMD_CA_SYS_ID_GET";
		case  CAS_IOCMD_OPERATOR_INFO_GET:
			return "CAS_IOCMD_OPERATOR_INFO_GET";
		case  CAS_IOCMD_ZONE_CHECK:
			return "CAS_IOCMD_ZONE_CHECK";
		case  CAS_IOCMD_SET_ZONE_CODE:
			return "CAS_IOCMD_SET_ZONE_CODE";
		case  CAS_IOCMD_CARD_BALANCE_GET:
			return "CAS_IOCMD_CARD_BALANCE_GET";
		case  CAS_IOCMD_FINGER_MSG_GET:
			return "CAS_IOCMD_FINGER_MSG_GET";
		case  CAS_IOCMD_ECM_FINGER_MSG_GET:
			return "CAS_IOCMD_ECM_FINGER_MSG_GET";
		case  CAS_IOCMD_IPP_BUY_INFO_GET:
			return "CAS_IOCMD_IPP_BUY_INFO_GET";
		case  CAS_IOCMD_STOP:
			return "CAS_IOCMD_STOP";
		case  CAS_IOCMD_STOP_CAS:
			return "CAS_IOCMD_STOP_CAS";
		case  CAS_IOCMD_SET_ECMEMM:
			return "CAS_IOCMD_SET_ECMEMM";
		case  CAS_IOCMD_VIDEO_PID_SET:
			return "CAS_IOCMD_VIDEO_PID_SET";
		case  CAS_IOCMD_AUDIO_PID_SET:
			return "CAS_IOCMD_AUDIO_PID_SET";
		case  CAS_IOCMD_CARD_OVERDRAFT_GET:
			return "CAS_IOCMD_CARD_OVERDRAFT_GET";
		case  CAS_IOCMD_OSD_MSG_GET:
			return "CAS_IOCMD_OSD_MSG_GET";
		case  CAS_IOCMD_OSD_MSG_SHOW_END:
			return "CAS_IOCMD_OSD_MSG_SHOW_END";
		case  CAS_IOCMD_RATING_GET:
			return "CAS_IOCMD_RATING_GET";
		case  CAS_IOCMD_RATING_SET:
			return "CAS_IOCMD_RATING_SET";
		case  CAS_IOCMD_WORK_TIME_SET:
			return "CAS_IOCMD_WORK_TIME_SET";
		case  CAS_IOCMD_WORK_TIME_GET:
			return "CAS_IOCMD_WORK_TIME_GET";
		case  CAS_IOCMD_PIN_SET:
			return "CAS_IOCMD_PIN_SET";
		case  CAS_IOCMD_PIN_STATE_GET:
			return "CAS_IOCMD_PIN_STATE_GET";
		case  CAS_IOCMD_PIN_ENABLE:
			return "CAS_IOCMD_PIN_ENABLE";
		case  CAS_IOCMD_PIN_VERIFY:
			return "CAS_IOCMD_PIN_VERIFY";
		case  CAS_IOCMD_MON_CHILD_STATUS_GET:
			return "CAS_IOCMD_MON_CHILD_STATUS_GET";
		case  CAS_IOCMD_MON_CHILD_STATUS_SET:
			return "CAS_IOCMD_MON_CHILD_STATUS_SET";
		case  CAS_IOCMD_MON_CHILD_IDENTIFY:
			return "CAS_IOCMD_MON_CHILD_IDENTIFY";
		case  CAS_IOCMD_READ_FEED_DATA:
			return "CAS_IOCMD_READ_FEED_DATA";
		case  CAS_IOCMD_ENTITLE_INFO_GET:
			return "CAS_IOCMD_ENTITLE_INFO_GET";
		case  CAS_IOCMD_ONE_ENTITLE_INFO_GET:
			return "CAS_IOCMD_ONE_ENTITLE_INFO_GET";
		case  CAS_IOCMD_IPPV_INFO_GET:
				return "CAS_IOCMD_IPPV_INFO_GET";
		case  CAS_IOCMD_IPP_EXCH_RECORD_GET:
			return "CAS_IOCMD_IPP_EXCH_RECORD_GET";
		case  CAS_IOCMD_ONE_EXCH_RECORD_GET:
			return "CAS_IOCMD_ONE_EXCH_RECORD_GET";
		case  CAS_IOCMD_BURSE_INFO_GET:
			return "CAS_IOCMD_BURSE_INFO_GET";
		case  CAS_IOCMD_DETITLE_INFO_GET:
			return "CAS_IOCMD_DETITLE_INFO_GET";
		case  CAS_IOCMD_DETITLE_INFO_DEL:
			return "CAS_IOCMD_DETITLE_INFO_DEL";
		case  CAS_IOCMD_PATERNER_STA_GET:
			return "CAS_IOCMD_PATERNER_STA_GET";
		case  CAS_IOCMD_PIN_ERR_CNT_GET:
			return "CAS_IOCMD_PIN_ERR_CNT_GET";
		case  CAS_IOCMD_MSG_GET:
			return "CAS_IOCMD_MSG_GET";
		case  CAS_IOCMD_OSD_GET:
			return "CAS_IOCMD_OSD_GET";
		case  CAS_IOCMD_URGENT_GET:
			return "CAS_IOCMD_URGENT_GET";
		case  CAS_IOCMD_MAIL_HEADER_GET:
			return "CAS_IOCMD_MAIL_HEADER_GET";
		case  CAS_IOCMD_MAIL_BODY_GET:
			return "CAS_IOCMD_MAIL_BODY_GET";
		case  CAS_IOCMD_MAIL_CHANGE_STATUS:
			return "CAS_IOCMD_MAIL_CHANGE_STATUS";
		case  CAS_IOCMD_MAIL_SPACE_STATUS:
			return "CAS_IOCMD_MAIL_SPACE_STATUS";
		case  CAS_IOCMD_ANNOUNCE_HEADER_GET:
			return "CAS_IOCMD_ANNOUNCE_HEADER_GET";
		case  CAS_IOCMD_ANNOUNCE_BODY_GET:
			return "CAS_IOCMD_ANNOUNCE_BODY_GET";
		case  CAS_IOCMD_MAIL_DELETE_BY_INDEX:
			return "CAS_IOCMD_MAIL_DELETE_BY_INDEX";
		case  CAS_IOCMD_MAIL_DELETE_ALL:
			return "CAS_IOCMD_MAIL_DELETE_ALL";
		case  CAS_IOCMD_FACTORY_SET:
			return "CAS_IOCMD_FACTORY_SET";
		case  CAS_IOCMD_ANNOUNCE_DELETE_BY_INDEX:
			return "CAS_IOCMD_ANNOUNCE_DELETE_BY_INDEX";
		case  CAS_IOCMD_ANNOUNCE_DELETE_ALL:
			return "CAS_IOCMD_ANNOUNCE_DELETE_ALL";
		case  CAS_IOCMD_PPV_CHARGE_GET:
			return "CAS_IOCMD_PPV_CHARGE_GET";
		case  CAS_IOCMD_STB_SERIAL_GET:
			return "CAS_IOCMD_STB_SERIAL_GET";
		case  CAS_IOCMD_SERVICE_ID_GET:
			return "CAS_IOCMD_SERVICE_ID_GET";
		case  CAS_IOCMD_CARD_INFO_GET:
			return "CAS_IOCMD_CARD_INFO_GET";
		case  CAS_IOCMD_CARD_LOCK_STA_GET:
			return "CAS_IOCMD_CARD_LOCK_STA_GET";
		case  CAS_IOCMD_GROUP_ID_GET:
			return "CAS_IOCMD_GROUP_ID_GET";
		case  CAS_IOCMD_CUR_PRG_NUM_GET:
			return "CAS_IOCMD_CUR_PRG_NUM_GET";
		case  CAS_IOCMD_CUR_PROG_INFO_GET:
			return "CAS_IOCMD_CUR_PROG_INFO_GET";
		case  CAS_IOCMD_PPV_TAG_GET:
			return "CAS_IOCMD_PPV_TAG_GET";
		case  CAS_IOCMD_LOADER_INFO_GET:
			return "CAS_IOCMD_LOADER_INFO_GET";
		case  CAS_IOCMD_CAS_SOFT_VER_GET:
			return "CAS_IOCMD_CAS_SOFT_VER_GET";
		case  CAS_IOCMD_ZONE_CODE_GET:
			return "CAS_IOCMD_ZONE_CODE_GET";
		case  CAS_IOCMD_ZONE_CODE_SET:
			return "CAS_IOCMD_ZONE_CODE_SET";
		case  CAS_IOCMD_EIGEN_VALUE_GET:
			return "CAS_IOCMD_EIGEN_VALUE_GET";
		case  CAS_IOCMD_PPV_TAG_SET:
			return "CAS_IOCMD_PPV_TAG_SET";
		case  CAS_IOCMD_LAST_PRG_INFO_CLR:
			return "CAS_IOCMD_LAST_PRG_INFO_CLR";
		case  CAS_IOCMD_PSI_TABLE_SET:
			return "CAS_IOCMD_PSI_TABLE_SET";
		case  CAS_IOCMD_CHANNEL_STATE_SET:
			return "CAS_IOCMD_CHANNEL_STATE_SET";
		case  CAS_IOCMD_FORCE_CHANNEL_INFO_GET:
			return "CAS_IOCMD_FORCE_CHANNEL_INFO_GET";
		case  CAS_IOCMD_AUTHEN_DAY_GET:
			return "CAS_IOCMD_AUTHEN_DAY_GET";
		case  CAS_IOCMD_IPP_NOTIFY_INFO_GET:
			return "CAS_IOCMD_IPP_NOTIFY_INFO_GET";
		case  CAS_IOCMD_SHORT_MSG_GET:
			return "CAS_IOCMD_SHORT_MSG_GET";
		case  CAS_IOCMD_PURCHASE_SET:
			return "CAS_IOCMD_PURCHASE_SET";
		case  CAS_IOCMD_PARENT_UNLOCK_SET:
			return "CAS_IOCMD_PARENT_UNLOCK_SET";
		case  CAS_IOCMD_PRODUCT_TYPE_GET:
			return "CAS_IOCMD_PRODUCT_TYPE_GET";
		case  CAS_IOCMD_GET_IMPORTANT_MAIL:
			return "CAS_IOCMD_GET_IMPORTANT_MAIL";
		case  CAS_IOCMD_SMC_STATUS_GET:
			return "CAS_IOCMD_SMC_STATUS_GET";
		case  CAS_IOCMD_SMC_OPEN_DEBUG:
			return "CAS_IOCMD_SMC_OPEN_DEBUG";	
		case  CAS_IOCMD_CTRL_CODE_GET:
			return "CAS_IOCMD_CTRL_CODE_GET";	
		case  CAS_IOCMD_GET_CERTIFICATE_SERVICE:
			return "CAS_IOCMD_GET_CERTIFICATE_SERVICE";
		case  CAS_IOCMD_SET_OVERDUE_INFO_SERVICE:
			return "CAS_IOCMD_SET_OVERDUE_INFO_SERVICE";
		case  CAS_IOCMD_GET_OVERDUE_INFO_SERVICE:
			return "CAS_IOCMD_GET_OVERDUE_INFO_SERVICE";
		case  CAS_IOCMD_LANGUAGE_SET:
			return "CAS_IOCMD_LANGUAGE_SET";
		case  CAS_IOCMD_SHOW_TYPE_GET:
			return "CAS_IOCMD_SHOW_TYPE_GET";
		case  CAS_IOCMD_IPP_BUY_SET:
			return "CAS_IOCMD_IPP_BUY_SET";
		case  CAS_IOCMD_SAVE_FILE_DATA:
			return "CAS_IOCMD_SAVE_FILE_DATA";
		case  CAS_IOCMD_GET_FILM_RATING:
			return "CAS_IOCMD_GET_FILM_RATING";
		case  CAS_IOCMD_REQUEST_MASK_BUFFER:
			return "CAS_IOCMD_REQUEST_MASK_BUFFER";
		case  CAS_IOCMD_REQUEST_UPDATE_BUFFER:
			return "CAS_IOCMD_REQUEST_UPDATE_BUFFER";
		case  CAS_IOCMD_ACLIST_GET:
			return "CAS_IOCMD_ACLIST_GET";
		case  CAS_IOCMD_WATCH_MR_PROGRAM:
			return "CAS_IOCMD_WATCH_MR_PROGRAM";	
		case  CAS_IOCMD_NO_WATCH_PROGRAM:
			return "CAS_IOCMD_NO_WATCH_PROGRAM";
		case  CAS_IOCMD_OTA_INFO_GET:
			return "CAS_IOCMD_OTA_INFO_GET";
		case  CAS_IOCMD_SYSTEM_TIME_GET:
			return "CAS_IOCMD_SYSTEM_TIME_GET";
		case  CAS_IOCMD_REC_MSG_FORM_OTHER_STB:
			return "CAS_IOCMD_REC_MSG_FORM_OTHER_STB";	
		case  CAS_IOCMD_REC_MSG_FORM_OTHER_STB_TIMEOUT:
			return "CAS_IOCMD_REC_MSG_FORM_OTHER_STB_TIMEOUT";
		case  CAS_IOCOM_MOBILE_UA_GET:
			return "CAS_IOCOM_MOBILE_UA_GET";
		case  CAS_IOCOM_STB_EXTENDED_INFO_GET:
			return "CAS_IOCOM_STB_EXTENDED_INFO_GET";
		case  CAS_IOCOM_GET_ECMEMM_PID:
			return "CAS_IOCOM_GET_ECMEMM_PID";
		case  CAS_IOCOM_UA_GET:
			return "CAS_IOCOM_UA_GET";
		case  CAS_IOCOM_AS_STBID_GET:
			return "CAS_IOCOM_AS_STBID_GET";
		case  CAS_IOCOM_IPPV_BUY_FROM_EPG:
			return "CAS_IOCOM_IPPV_BUY_FROM_EPG";
		case  CAS_IOCOM_NEW_MAIL_CHECK:
			return "CAS_IOCOM_NEW_MAIL_CHECK";
		case  CAS_IOCMD_CA_OVERDUE:
			return "CAS_IOCMD_CA_OVERDUE";
		case  CAS_IOCMD_DVB_OSD_GET:
			return "CAS_IOCMD_DVB_OSD_GET";
			
	}

	return "NO MATCH IOCMD";
}

RET_CODE ret2ret_code(DVBCA_ERROR_CODE ret)
{
	switch(ret)
	{
		case DVBCA_SUCESS: 
			return CAS_SUCESS;   
			
		case DVBCA_FAILED:  
			return CAS_FAILED; 
			
		case DVBCA_INSERTCARD:
			return CAS_ERR_NOCARD;         
			
		case DVBCA_CARD_INVALID:
			return CAS_ERR_INVALID_CARD; 	
			
		case DVBCA_ECM_PID_ERROR:
			return CAS_ERR_INVALID_ECMPID;    
			
		case DVBCA_EMM_PID_ERROR:
			return CAS_ERR_INVALID_EMMPID;  
			
		case DVBCA_NOENTITLE:
			return CAS_ERR_NOENTITLE;   	
			
		case DVBCA_PIN_ERROR:	
			return CAS_ERR_PIN_ERROR;   	
			
		case DVBCA_STB_PAIR_FAILED:
			return CAS_ERR_STB_PAIR_FAILED;  
			
		case DVBCA_INIT_ERROR:
			return CAS_ERR_INIT_ERROR;	  
			
		case DVBCA_CARD_VER_ERROR:
			return CAS_ERR_CARD_VER_ERROR;  
	}           
		return 0;       
}

const char * _cas_ioctl_err_2str(DVBCA_ERROR_CODE ret)
{
	switch(ret)
	{
		case DVBCA_SUCESS:
			return "CAS_SUCESS";
			
		case DVBCA_FAILED:
			return "CAS_FAILED";
			
		case DVBCA_INSERTCARD:
			return "CAS_ERR_NOCARD";
			
		case DVBCA_CARD_INVALID:
			return "CAS_ERR_INVALID_CARD";
			
		case DVBCA_ECM_PID_ERROR:
			return "DVBCA_ECM_PID_ERROR";
			
		case DVBCA_EMM_PID_ERROR:
			return "DVBCA_EMM_PID_ERROR";
			
		case DVBCA_NOENTITLE:
			return "CAS_ERR_NOENTITLE";
			
		case DVBCA_PIN_ERROR:
			return "CAS_ERR_PIN_ERROR";
			
		case DVBCA_STB_PAIR_FAILED:
			return "CAS_ERR_STB_PAIR_FAILED";
			
		case DVBCA_INIT_ERROR:
			return "CAS_ERR_INIT_ERROR";
			
		case DVBCA_CARD_VER_ERROR:
			return "CAS_ERR_CARD_VER_ERROR";
	}
	return "NO MATCH ERR RETURN";
}

extern u16 my_sp_id;
extern u16 mail_index;
int get_DVBCA_rating(int get_app_level)
{
		switch(get_app_level)
		{
		case 7: get_app_level = 4; break;
		case 6: get_app_level = 6;break;
		case 5: get_app_level = 8; break;
		case 4: get_app_level = 12;break;
		case 3: get_app_level = 14;break;
		case 2: get_app_level = 16;break;
		case 1: 
		case 0: 
		get_app_level = 18;
		break;			
		default:get_app_level = 18;break;
		}
		return get_app_level;
}

int get_app_rating(int dvbca_level)
{
		switch(dvbca_level)
	{
		case 7: dvbca_level = 4; break;
		case 6: dvbca_level = 6;break;
		case 5: dvbca_level = 8; break;
		case 4: dvbca_level = 12;break;
		case 3: dvbca_level = 14;break;
		case 2: dvbca_level = 16;break;
		case 1: 
		case 0: 
		dvbca_level = 18;
		break;	 		
		default:dvbca_level = 18;break;
	}
		return dvbca_level;
}
int set_app_rating(int set_app_level)
{
		switch(set_app_level)
		{
		  case 4: set_app_level = 7;break;
		  case 5:
		  case 6:
			set_app_level = 6;
			break;
		  case 7:
		  case 8:
			set_app_level = 5;
			break;
		  case 9:
		  case 10:
		  case 11:
		  case 12:
			set_app_level = 4;
			break;
		  case 13:
		  case 14:
			set_app_level = 3;
			break;
		 case 15:
		 case 16:
			set_app_level = 2;
			break;
		  case 17:
		  case 18:
			set_app_level = 1;
			break;		
		  default:
			set_app_level = 0;
			break;	 
		}
		return set_app_level;
}



static RET_CODE cas_adt_dvbca_io_ctrl(u32 cmd, void *param)
{
	RET_CODE res;
	cas_card_info_t *card_info = NULL;	
	cas_adt_dvbca_priv_t *priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv;
	int ret=0;
	DEBUG(DVBCAS_PORTING,INFO,"[dvbcas]>>>>>>>>>>>>>> ! \n");
	DEBUG(DVBCAS_PORTING,INFO,"ioclt[%s]\n",_cas_ioctl2str(cmd));

	switch(cmd)
	{
		case CAS_IOCMD_FINGER_MSG_GET:
		{
			finger_msg_t *p_finger_msg = NULL;
			DEBUG(DVBCAS_PORTING,INFO,"[DBCAS] CAS_IOCMD_FINGER_MSG_GET \n");
	        p_finger_msg = (finger_msg_t *)param;
	        dvbcas_stb_drv_careqlock();
	        memcpy(p_finger_msg, &(priv->dvbca_msgFinger), sizeof(finger_msg_t));
	        dvbcas_stb_drv_carequnlock();
	        res = SUCCESS;
		}
			break;
		case CAS_IOCMD_OSD_GET:
		{
			msg_info_t *p_osd_msg = NULL;
			DEBUG(DVBCAS_PORTING,INFO,"[DVBCAS] CAS_IOCMD_OSD_MSG_GET \n");
	        p_osd_msg = (msg_info_t *)param;
	        dvbcas_stb_drv_careqlock();
	        memcpy(p_osd_msg, &(priv->dvbca_OsdMsg), sizeof(msg_info_t));
			DEBUG(DVBCAS_PORTING,INFO,"&&&&&&&&&&&&&&&&&&&&p_osd_msg -> text = %s&&&&&&&&&&&&&&&&&&&&&&&&&\n", p_osd_msg->data);
	        dvbcas_stb_drv_carequnlock();
	        res = SUCCESS;
		}
			break;
		case CAS_IOCMD_DVB_OSD_GET:
		{
			msg_info_t *p_osd_msg = NULL;
			DEBUG(DVBCAS_PORTING,INFO,"[DVBCAS] CAS_IOCMD_DVB_OSD_GET \n");
			p_osd_msg = (msg_info_t *)param;
			dvbcas_stb_drv_careqlock();
			memcpy(p_osd_msg, &(priv->dvbca_OsdMsg), sizeof(msg_info_t));
			DEBUG(DVBCAS_PORTING,INFO,"&&&&&&&&&&&&&&&&&&&&p_osd_msg -> text = %s&&&&&&&&&&&&&&&&&&&&&&&&&\n", p_osd_msg->data);
			dvbcas_stb_drv_carequnlock();
			res = SUCCESS;
		}
			break;

		case CAS_IOCMD_URGENT_GET:
		{
			dvbcas_urgent *urgent_msg = NULL;
			DEBUG(DVBCAS_PORTING,INFO,"[DVBCAS] CAS_IOCMD_URGENT_GET\n");
			urgent_msg = (dvbcas_urgent *)param;
			dvbcas_stb_drv_careqlock();
	        memcpy(urgent_msg, &(priv->urgent), sizeof(dvbcas_urgent));
	        dvbcas_stb_drv_carequnlock();
	        res = SUCCESS;
		}
			break;
			
	    case CAS_IOCMD_GET_MAIL_MAX:
			*(u8 *)param = priv->mail_cfg.max_mail_num;
			DEBUG(DVBCAS_PORTING,INFO,"******************max_mail = %d**************\n",priv->mail_cfg.max_mail_num);
			break;
		case CAS_IOCMD_GET_MAIL_POLICY:
			*(u8 *)param = priv->mail_cfg.mail_policy;
			DEBUG(DVBCAS_PORTING,INFO,"******************policy = %d**************\n",priv->mail_cfg.mail_policy);
			break;
			
		case CAS_IOCMD_GET_CURRENT_MAIL:
			break;

		case CAS_IOCMD_GET_SERIAL_DATA:
			memcpy((u8 *)param, priv->stb_serial, STB_SERIAL_LEN);
			break;

		case CAS_IOCMD_CA_SYS_ID_GET:
			DEBUG(DVBCAS_PORTING,INFO," CAS_RCV_IOCMD---> CAS_IOCMD_CA_SYS_ID_GET\n");
			break;

		case CAS_IOCMD_OPERATOR_INFO_GET:
		{
			cas_operators_info_t *opInfo = NULL;
			DVBCA_UINT32 puiOperatorID[5], puiOperatorNum;
			ST_DVBCA_OPERATORINFO info;
			int i;
			DEBUG(DVBCAS_PORTING,INFO,"operator worktimr....\n");
			opInfo = (cas_operators_info_t *)param;
			MT_ASSERT(opInfo != NULL);
			memset((void*)opInfo, 0, sizeof(cas_operators_info_t));
			ret = DVBCASTB_GetOperatorID(puiOperatorID,&puiOperatorNum);
			for(i = 0;i < puiOperatorNum && i<5;i++)
			{
				opInfo->p_operator_info[i].operator_id = (u16)puiOperatorID[i];
			}
			DEBUG(DVBCAS_PORTING,INFO,"_*_*_*sp_id = %d-%d-%d-%d-%d puiOperatorNum = %d\n",puiOperatorID[0],puiOperatorID[1],
				puiOperatorID[2],puiOperatorID[3],puiOperatorID[4],puiOperatorNum);
			CHECK_RET(DVBCAS_PORTING,ret);	
			opInfo->max_num = puiOperatorNum;
			for(i=0;i<opInfo->max_num && i<5;i++)
			{
				DVBCASTB_GetOperatorInfo(puiOperatorID[i],&info);			
				opInfo->p_operator_info[i].operator_id = puiOperatorID[i];
				DEBUG(DVBCAS_PORTING,INFO,"operator_id = %d****\n",opInfo->p_operator_info[0].operator_id);
				strncpy((char *)opInfo->p_operator_info[i].operator_name,
					(char *)info.aucOperatorName, CAS_OPERATOR_NAME_MAX_LEN);
				sprintf((char *)opInfo->p_operator_info[i].expired_time,
					"%04d-%02d-%02d %02d:%02d:%02d",
					info.stExpiredTime.ucYear + 2000,
					info.stExpiredTime.ucMonth,
					info.stExpiredTime.ucDay,
					info.stExpiredTime.ucHour,
					info.stExpiredTime.ucMinute,
					info.stExpiredTime.ucSecond);
				opInfo->p_operator_info[i].net_workID = info.usNetworkID;
				DEBUG(DVBCAS_PORTING,INFO,"xXXXX%s\n",opInfo->p_operator_info[0].expired_time);
			}
			DEBUG(DVBCAS_PORTING,INFO,"worktime:%04d-%02d-%02d %02d:%02d:%02d",
					info.stExpiredTime.ucYear,
					info.stExpiredTime.ucMonth,
					info.stExpiredTime.ucDay,
					info.stExpiredTime.ucHour,
					info.stExpiredTime.ucMinute,
					info.stExpiredTime.ucSecond);
			DEBUG(DVBCAS_PORTING,INFO,"id is %d\n",opInfo->p_operator_info[0].operator_id);
			DEBUG(DVBCAS_PORTING,INFO,"net_work_id = %d",opInfo->p_operator_info[0].net_workID);
		}
		break;

		case CAS_IOCMD_ZONE_CHECK:
		case CAS_IOCMD_SET_ZONE_CODE:
		case CAS_IOCMD_CARD_BALANCE_GET:
		case CAS_IOCMD_ECM_FINGER_MSG_GET:
		case CAS_IOCMD_IPP_BUY_INFO_GET:
			break;
			
		case CAS_IOCMD_STOP:
			break;

		case CAS_IOCMD_STOP_CAS:
			break;
		
		case CAS_IOCMD_SET_ECMEMM:
	        break;
			
		case CAS_IOCMD_VIDEO_PID_SET:
			priv->cur_v_pid = *((u16 *)param);
			DEBUG(DVBCAS_PORTING,INFO," CAS_RCV_IOCMD---> CAS_IOCMD_VIDEO_PID_SET\n");
			ret = SUCCESS;
			break;

		case CAS_IOCMD_AUDIO_PID_SET:
			priv->cur_a_pid = *((u16 *)param);
			DEBUG(DVBCAS_PORTING,INFO," CAS_RCV_IOCMD---> CAS_IOCMD_VIDEO_PID_SET\n");
			ret = SUCCESS;
			break;
			
		case CAS_IOCMD_CARD_OVERDRAFT_GET:
		case CAS_IOCMD_OSD_MSG_GET:
		case CAS_IOCMD_OSD_MSG_SHOW_END:
			break;

		case CAS_IOCMD_RATING_GET:
			param = &watch_level;
			DEBUG(DVBCAS_PORTING,INFO,"***** watch_level:%d",watch_level);			
			watch_level = get_app_rating(watch_level);
			ret = DVBCASTB_GetRating(&watch_level);		
			CHECK_RET(DVBCAS_PORTING,ret);		
	        break;

		case CAS_IOCMD_RATING_SET:
		{
			cas_rating_set_t *rating_set= NULL;
			rating_set = (cas_rating_set_t *)param;
			MT_ASSERT(rating_set != NULL);
			rating_set->rate = set_app_rating(rating_set->rate);	

			ret = DVBCASTB_SetRating(rating_set->pin,rating_set->rate);
			DEBUG(DVBCAS_PORTING, INFO, "rating_set->pin:%s, rating_set->rate:%d",
												rating_set->pin, rating_set->rate);

			CHECK_RET(DVBCAS_PORTING,ret);	
		}
	        break;

		case CAS_IOCMD_WORK_TIME_SET:
		{
			cas_card_work_time_t *p_work_time = NULL;
			ST_DVBCAWORKTIME start_time,end_time;
			DEBUG(DVBCAS_PORTING,INFO,"[DVBCA] CAS_IOCMD_WORK_TIME_SET \n");
			p_work_time = (cas_card_work_time_t *)param;
			DEBUG(DVBCAS_PORTING,INFO,"[DVBCA]pin : %x-%x-%x ! \n", p_work_time->pin[0], p_work_time->pin[1], p_work_time->pin[2]);
			DEBUG(DVBCAS_PORTING,INFO,"[DVBCA]sapare num %d \n", p_work_time->pin_spare_num);
			DEBUG(DVBCAS_PORTING,INFO,"[DVBCA]set card WORKTIME START : %d : %d  \n", start_time.ucHour, start_time.ucMinute);
			DEBUG(DVBCAS_PORTING,INFO,"[DVBCA]set card WORKTIME END	 : %d : %d  \n", end_time.ucHour, end_time.ucMinute);
			
			start_time.ucHour = p_work_time->start_hour;
			start_time.ucMinute = p_work_time->start_minute;
			
			end_time.ucHour = p_work_time->end_hour;
			end_time.ucMinute = p_work_time->end_minute;
				
			ret = DVBCASTB_SetWorkTime((DVBCA_UINT8*) p_work_time->pin, &start_time,&end_time);
			CHECK_RET(DVBCAS_PORTING,ret);
		}	
            break;
		
		case CAS_IOCMD_WORK_TIME_GET:
		{
			ST_DVBCAWORKTIME start_time,end_time;
			card_info->work_time= *(cas_card_work_time_t *)param;
			ret = DVBCASTB_GetWorkTime(&start_time, &end_time);
			CHECK_RET(DVBCAS_PORTING,ret);
			card_info->work_time.start_hour= start_time.ucHour;
			card_info->work_time.start_minute= start_time.ucMinute;
			
			card_info->end_time.end_hour= end_time.ucHour;
			card_info->end_time.end_minute= end_time.ucMinute;
 		}
            break;

		case CAS_IOCMD_PIN_SET:
		{
			int i;
			cas_pin_modify_t *p_pin_modify = NULL;
			p_pin_modify = (cas_pin_modify_t *)param;
			MT_ASSERT(p_pin_modify != NULL);
			DEBUG(DVBCAS_PORTING,INFO,"change pin");
			
			OS_PRINTF("\n old:");
			for (i=0;i<6;i++)
				OS_PRINTF("%d",p_pin_modify->old_pin[i]);
			OS_PRINTF("\n new:");
			for (i=0;i<6;i++)
				OS_PRINTF("%d",p_pin_modify->new_pin[i]);
			OS_PRINTF("\n");			

			
			ret = DVBCASTB_ChangePin(p_pin_modify->old_pin,p_pin_modify->new_pin);
			CHECK_RET(DVBCAS_PORTING,ret);
			DEBUG(DVBCAS_PORTING,INFO,"CHANGE PIN = %d ************&&&&&\n",ret);
		}
	        break;

		case CAS_IOCMD_PIN_STATE_GET:
            break;
		case CAS_IOCMD_PIN_ENABLE:
			break;
			
		case CAS_IOCMD_PIN_VERIFY:
		{
			cas_pin_modify_t *p_pin_mod = NULL;
			p_pin_mod = (cas_pin_modify_t *)param;
			MT_ASSERT(p_pin_mod != NULL);
			ret = DVBCASTB_CheckPin(p_pin_mod->match_pin);
			CHECK_RET(DVBCAS_PORTING,ret);
			DEBUG(DVBCAS_PORTING,INFO,"check PIN = %d************&&&&&\n",ret);

		}
            break;
			
		case CAS_IOCMD_MON_CHILD_STATUS_GET:
		case CAS_IOCMD_MON_CHILD_STATUS_SET:
		case CAS_IOCMD_MON_CHILD_IDENTIFY:
		case CAS_IOCMD_READ_FEED_DATA:
			break;

		case CAS_IOCMD_ENTITLE_INFO_GET:
		{ 
			ST_DVBCA_ENTITLES  *p_pstProductEntitles = NULL;
			int i;
			product_entitles_info_t *p_entitles_info = (product_entitles_info_t *)param;
			p_entitles_info->operator_id = my_sp_id;  // for test
			DEBUG(DVBCAS_PORTING,INFO,"^^^^^^get id = %d\n",p_entitles_info->operator_id);
			// get num
			p_pstProductEntitles = (ST_DVBCA_ENTITLES  *)mtos_malloc(sizeof(ST_DVBCA_ENTITLES));
			ret = DVBCASTB_GetServiceEntitles(p_entitles_info->operator_id, p_pstProductEntitles);
			DEBUG(DVBCAS_PORTING,INFO,"ret = %d\n",ret);
			CHECK_RET(DVBCAS_PORTING,ret);
			

			p_entitles_info->max_num = p_pstProductEntitles->usProductCount;
			DEBUG(DVBCAS_PORTING,INFO,"p_entitles_info->max_num = %d\n",p_entitles_info->max_num);
			for(i=0; i<p_entitles_info->max_num; i++)
			{
				p_entitles_info->p_entitle_info[i].product_id = (u32)p_pstProductEntitles->astEntitles[i].usProductID;
				p_entitles_info->p_entitle_info[i].start_time[0]= p_pstProductEntitles->astEntitles[i].ucStartYear;
				p_entitles_info->p_entitle_info[i].start_time[1]= p_pstProductEntitles->astEntitles[i].ucStartMonth;
				p_entitles_info->p_entitle_info[i].start_time[2]= p_pstProductEntitles->astEntitles[i].ucStartDay;

				p_entitles_info->p_entitle_info[i].expired_time[0]= p_pstProductEntitles->astEntitles[i].ucExpiredYear;
				p_entitles_info->p_entitle_info[i].expired_time[1]= p_pstProductEntitles->astEntitles[i].ucExpiredMonth;
				p_entitles_info->p_entitle_info[i].expired_time[2]= p_pstProductEntitles->astEntitles[i].ucExpiredDay; 	
			}
			mtos_free(p_pstProductEntitles);
		}	
        break;

		case CAS_IOCMD_ONE_ENTITLE_INFO_GET:
		case CAS_IOCMD_IPPV_INFO_GET:
		case CAS_IOCMD_IPP_EXCH_RECORD_GET:
		case CAS_IOCMD_ONE_EXCH_RECORD_GET:
		case CAS_IOCMD_BURSE_INFO_GET:
		case CAS_IOCMD_DETITLE_INFO_GET:
		case CAS_IOCMD_DETITLE_INFO_DEL:
		case CAS_IOCMD_PIN_ERR_CNT_GET:
			break;
			
		case CAS_IOCMD_PATERNER_STA_GET:
		{
			cas_rating_set_t *pairing_set= NULL;
			pairing_set = (cas_rating_set_t *)param;
			MT_ASSERT(pairing_set != NULL);
			ret = DVBCASTB_SetStbPair(pairing_set->pin);
			CHECK_RET(DVBCAS_PORTING,ret);	
		}
	        break;
			
		case CAS_IOCMD_MSG_GET:
		{
			msg_info_t *p_msg_info =NULL;
			p_msg_info = (msg_info_t *)param;
			MT_ASSERT(p_msg_info != NULL);
			p_msg_info->type = 1;
			p_msg_info->osd_display.location = 0;
			p_msg_info->osd_display.roll_value = 1;
			p_msg_info->osd_display.roll_mode = 1;
			p_msg_info->data_len = DVBCA_MAXLEN_MSG_TEXT;
			p_msg_info->display_time = cas_osd_msg_t.ucIntervalTime;
			memcpy(p_msg_info->data, &cas_osd_msg_t.aucText, DVBCA_MAXLEN_MSG_TEXT);
			DEBUG(DVBCAS_PORTING, INFO,"CAS_RCV_IOCMD---> CAS_IOCMD_MSG_GET\n");
		}
        break;
	  
		case CAS_IOCMD_MAIL_HEADER_GET:		  
		{
			int ret, i=0;
			//DVBCA_UINT16 total_num =0;
			ST_DVBCAEmail *p_mail;
			cas_mail_headers_t *p_mail_headers= NULL;

			p_mail_headers = (cas_mail_headers_t *)param;
			ASSERT(p_mail_headers != NULL);

			p_mail_headers->mail_read = DVBCASTB_GetMailNum(CA_DB_MAIL_READ_NUM);
			p_mail_headers->max_display_email_num = DVBCASTB_GetMailNum(CA_DB_MAIL_TOTAL_NUM);
			p_mail_headers->max_num =  DVBCASTB_GetMailNum(CA_DB_MAIL_TOTAL_NUM);
			p_mail_headers->new_mail_num = DVBCASTB_GetMailNum(CA_DB_MAIL_NOT_READ_NUM);
			p_mail_headers->total_email_num = DVBCASTB_GetMailNum(CA_DB_MAIL_TOTAL_NUM);

			p_mail = (ST_DVBCAEmail*)mtos_malloc(sizeof(ST_DVBCAEmail) * p_mail_headers->total_email_num);
			if (p_mail == NULL)
			{
				DEBUG(DVBCAS_PORTING,ERR,"*********************malloc failure!!\n");
				return CAS_FAILED;
			}
			ret = DVBCASTB_GetMail(p_mail,0,p_mail_headers->total_email_num);
			CHECK_RET(DVBCAS_PORTING,ret);	  

			for(i=0; i<p_mail_headers->total_email_num; i++)
			{
				p_mail_headers->p_mail_head[i].body_len = strlen((char*)p_mail[i].aucContext);
				p_mail_headers->p_mail_head[i].creat_date[0] = '\0';
				p_mail_headers->p_mail_head[i].display_times = 1;
				p_mail_headers->p_mail_head[i].index = 0;
				p_mail_headers->p_mail_head[i].m_id = p_mail[i].usEmailID;
				p_mail_headers->p_mail_head[i].new_email = p_mail[i].ucNewEmail;
				p_mail_headers->p_mail_head[i].priority = 0;

				strncpy((char *)p_mail_headers->p_mail_head[i].sender,
				(char *)p_mail[i].aucTerm,MIN(DVBCA_MAIL_TERM_SIZE,CAS_MAIL_SENDER_MAX_LEN));
				//p_mail_headers->p_mail_head[i].sender[CAS_MAIL_SENDER_MAX_LEN - 1] ="SYSTEM" ;
				strncpy((char *)p_mail_headers->p_mail_head[i].sender,
					(char *)"SYSTEM",MIN(DVBCA_MAIL_TERM_SIZE,CAS_MAIL_SENDER_MAX_LEN));
				p_mail_headers->p_mail_head[i].sender[CAS_MAIL_SENDER_MAX_LEN - 1] = '\0';

				p_mail_headers->p_mail_head[i].send_date[0] = '\0';
				p_mail_headers->p_mail_head[i].special_id = 0;

				strncpy((char *)p_mail_headers->p_mail_head[i].subject,
				(char *)p_mail[i].aucTitle,MIN(DVBCA_MAIL_TITLE_SIZE,CAS_MAIL_SUBJECT_MAX_LEN));
				p_mail_headers->p_mail_head[i].subject[CAS_MAIL_SUBJECT_MAX_LEN - 1] = '\0';

			}
			mtos_free(p_mail);
		}
		break;

		case CAS_IOCMD_MAIL_BODY_GET:
		{
			ST_DVBCAEmail *p_mail;

			cas_mail_body_t *p_mail_body = NULL;
			p_mail_body = (cas_mail_body_t *)param;
			ASSERT(p_mail_body != NULL);	
			p_mail = (ST_DVBCAEmail*)mtos_malloc(sizeof(ST_DVBCAEmail));
			
			if (p_mail == NULL){
				DEBUG(DVBCAS_PORTING,ERR,"*********************malloc failure!!\n");
				return CAS_FAILED;
			}
			DEBUG(DVBCAS_PORTING,INFO,"mail_index = %d\n",mail_index);
			ret = DVBCASTB_GetMail(p_mail,mail_index,1);
			CHECK_RET(DVBCAS_PORTING,ret);
			
			strncpy((char *)p_mail_body->data,
					(char *)p_mail->aucContext,MIN(CAS_MAIL_BODY_MAX_LEN,DVBCA_MAIL_CONTEXT_SIZE));
			p_mail_body->data[CAS_MAIL_BODY_MAX_LEN - 1] = '\0';
			DEBUG(DVBCAS_PORTING, INFO,"[DVBCA]p_mail_body->data: %s! \n",p_mail_body->data);

			mtos_free(p_mail);
		}		
		break;
		
		case CAS_IOCMD_MAIL_CHANGE_STATUS:
		{
			int ret;
			u32 *p_mail_id = NULL;
			p_mail_id = (u32 *)param;
			ret = DVBCASTB_SetMailRead((DVBCA_UINT16)*p_mail_id);
			if(ret < 0)
			{
				DEBUG(DVBCAS_PORTING,INFO,"****************CHANGE MAIL FAILED****************\n");	
				//return DVBCA_FAILED;
			}
			DEBUG(DVBCAS_PORTING,INFO,"*****************p_mail_id= %d***************\n",*p_mail_id);
		}
	    break;

		case CAS_IOCMD_MAIL_SPACE_STATUS:
	        break;

		case CAS_IOCMD_ANNOUNCE_HEADER_GET:
	        break;

		case CAS_IOCMD_ANNOUNCE_BODY_GET:
	        break;

		case CAS_IOCMD_MAIL_DELETE_BY_INDEX:
		{
			int ret;
			u32 *p_mail_id = NULL;
			p_mail_id = (u32 *)param;
			ret = DVBCASTB_DelMail((DVBCA_UINT16)*p_mail_id);
			if(ret < 0)
			{
		//		DEBUG(DVBCAS_PORTING,INFO,"****************DELETE MAIL FAILED****************\n");	
				return DVBCA_FAILED;
			}
		//	DEBUG(DVBCAS_PORTING,INFO,"**************delete mail***p_mail_id= %d***************\n",*p_mail_id);
			
			break;
		}

		case CAS_IOCMD_MAIL_DELETE_ALL:
		//	DEBUG(DVBCAS_PORTING,INFO,"##########################delete all mail########################\n");
			break;
		case CAS_IOCMD_FACTORY_SET:
		case CAS_IOCMD_ANNOUNCE_DELETE_BY_INDEX:
		case CAS_IOCMD_ANNOUNCE_DELETE_ALL:
		case CAS_IOCMD_PPV_CHARGE_GET:
			break;	
		
		case CAS_IOCMD_STB_SERIAL_GET:
			memcpy((u8 *)param, priv->stb_serial, STB_SERIAL_LEN);
            break;

		case CAS_IOCMD_SERVICE_ID_GET:
			break;
			
		case CAS_IOCMD_CARD_INFO_GET:
		{
			ST_DVBCAWORKTIME begintime, endtime;
			DVBCA_UINT32 puiStbID[20];
			DVBCA_UINT8 pucStbNum;
			card_info = (cas_card_info_t *)param;
			MT_ASSERT(card_info != NULL);
			memset((void*)card_info, 0, sizeof(cas_card_info_t));

			ret = DVBCASTB_GetCardID(card_info->card_id);
			DEBUG(DVBCAS_PORTING,INFO,"****card_id = %s",card_info->card_id);
			card_info->card_id_len = DVBSTBCA_Strlen((char *)card_info->card_id);
			MT_ASSERT(card_info->card_id_len < CAS_CARD_ID_MAX_LEN);
			CHECK_RET(DVBCAS_PORTING,ret);

			ret = DVBCASTB_GetCaVer(card_info->cas_ver);
			DEBUG(DVBCAS_PORTING,INFO,"****cas_ver = %s",card_info->cas_ver);
			MT_ASSERT(DVBSTBCA_Strlen((char *)card_info->cas_ver) <= CAS_VERSION_MAX_LEN);
			CHECK_RET(DVBCAS_PORTING,ret);

			ret = DVBCASTB_GetCardVer(card_info->card_version);
			DEBUG(DVBCAS_PORTING,INFO,"****card_version = %s",card_info->card_version);
			MT_ASSERT((u8)DVBSTBCA_Strlen((char *)card_info->card_version) <= CAS_CARD_VER_MAX_LEN);
			CHECK_RET(DVBCAS_PORTING,ret);

			ret = DVBCASTB_GetRating((unsigned char *)&card_info->card_work_level);
			DEBUG(DVBCAS_PORTING,INFO,"*******card_work_level_get = %d",card_info->card_work_level);
			card_info->card_work_level = get_app_rating(card_info->card_work_level);
			DEBUG(DVBCAS_PORTING,INFO,"ret = %d\n",ret);
			DEBUG(DVBCAS_PORTING,INFO,"****card_work_level = %d",card_info->card_work_level);
			CHECK_RET(DVBCAS_PORTING,ret);

			ret = DVBCASTB_GetWorkTime(&begintime,&endtime);
			CHECK_RET(DVBCAS_PORTING,ret);
			card_info->work_time.start_hour = begintime.ucHour;
			card_info->work_time.start_minute = begintime.ucMinute;
			card_info->end_time.end_hour = endtime.ucHour;
			card_info->end_time.end_minute= endtime.ucMinute;
			DEBUG(DVBCAS_PORTING,INFO,"****%2d:%2d-%2d:%2d\n",card_info->work_time.start_hour,card_info->work_time.start_minute,
				card_info->end_time.end_hour,card_info->end_time.end_minute);

			ret = DVBCASTB_GetStbPair(puiStbID,&pucStbNum);
			strncpy((char*)card_info->stb_pair_id ,(char *)puiStbID, 20);
			CHECK_RET(DVBCAS_PORTING,ret);	
		}
			break;
			
		case CAS_IOCMD_CUR_PRG_NUM_GET:
	        break;

		case CAS_IOCMD_CARD_LOCK_STA_GET:
		case CAS_IOCMD_GROUP_ID_GET:
		case CAS_IOCMD_CUR_PROG_INFO_GET:
		case CAS_IOCMD_PPV_TAG_GET:
		case CAS_IOCMD_LOADER_INFO_GET:
		case CAS_IOCMD_CAS_SOFT_VER_GET:
		case CAS_IOCMD_ZONE_CODE_GET:
		case CAS_IOCMD_ZONE_CODE_SET:
		case CAS_IOCMD_EIGEN_VALUE_GET:
		case CAS_IOCMD_LAST_PRG_INFO_CLR:
		case CAS_IOCMD_CHANNEL_STATE_SET:
		case CAS_IOCMD_FORCE_CHANNEL_INFO_GET:
		case CAS_IOCMD_AUTHEN_DAY_GET:			
		case CAS_IOCMD_IPP_NOTIFY_INFO_GET:
		case CAS_IOCMD_SHORT_MSG_GET:
		case CAS_IOCMD_PURCHASE_SET:
		case CAS_IOCMD_PARENT_UNLOCK_SET:
		case CAS_IOCMD_PRODUCT_TYPE_GET:
		case CAS_IOCMD_GET_IMPORTANT_MAIL:
		case CAS_IOCMD_SMC_STATUS_GET:
		case CAS_IOCMD_SMC_OPEN_DEBUG:
		case CAS_IOCMD_CTRL_CODE_GET:
		case CAS_IOCMD_GET_CERTIFICATE_SERVICE:
		case CAS_IOCMD_SET_OVERDUE_INFO_SERVICE:
		case CAS_IOCMD_GET_OVERDUE_INFO_SERVICE:
		case CAS_IOCMD_LANGUAGE_SET:
		case CAS_IOCMD_SHOW_TYPE_GET:
		case CAS_IOCMD_IPP_BUY_SET:
		case CAS_IOCMD_SAVE_FILE_DATA:
		case CAS_IOCMD_GET_FILM_RATING:
		case CAS_IOCMD_REQUEST_MASK_BUFFER:
		case CAS_IOCMD_REQUEST_UPDATE_BUFFER:
		case CAS_IOCMD_ACLIST_GET:
		case CAS_IOCMD_WATCH_MR_PROGRAM:
		case CAS_IOCMD_NO_WATCH_PROGRAM:
		case CAS_IOCMD_OTA_INFO_GET:
		case CAS_IOCMD_SYSTEM_TIME_GET:
		case CAS_IOCMD_REC_MSG_FORM_OTHER_STB:
		case CAS_IOCMD_REC_MSG_FORM_OTHER_STB_TIMEOUT:
		case CAS_IOCOM_MOBILE_UA_GET:
		case CAS_IOCOM_STB_EXTENDED_INFO_GET:
		case CAS_IOCOM_GET_ECMEMM_PID:
		case CAS_IOCOM_UA_GET:
		case CAS_IOCOM_AS_STBID_GET:
		case CAS_IOCOM_IPPV_BUY_FROM_EPG:
		case CAS_IOCOM_NEW_MAIL_CHECK:
		case CAS_IOCMD_CA_OVERDUE:	
		break;
			
		default:
			break;	
	}
	DEBUG(DVBCAS_PORTING,INFO,"[dvbcas]<<<<<<<<<<<<<<<<<<<<!\n");
	DEBUG(DVBCAS_PORTING,INFO,"ioclt[%s]\n",_cas_ioctl_err_2str(ret));
	return ret2ret_code(ret);
}/*cas_adt_dvbca_io_ctrl*/

RET_CODE cas_adt_dvbca_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id)
{
  cas_adt_dvbca_priv_t *p_priv = NULL;
  DEBUG_ENABLE_MODE(DVBCAS_PORTING,INFO);
  DEBUG(DVBCAS_PORTING,INFO,"call in!\n");
  g_cas_priv.cam_op[CAS_ID_DVBCA].attached = 1;
  g_cas_priv.cam_op[CAS_ID_DVBCA].inited = 0;

  g_cas_priv.cam_op[CAS_ID_DVBCA].func.init
    = cas_adt_dvbca_init;
  g_cas_priv.cam_op[CAS_ID_DVBCA].func.deinit
    = cas_adt_dvbca_deinit;
  g_cas_priv.cam_op[CAS_ID_DVBCA].func.card_reset
    = cas_adt_dvbca_card_reset;
  g_cas_priv.cam_op[CAS_ID_DVBCA].func.card_remove
    = cas_adt_dvbca_card_remove;
  g_cas_priv.cam_op[CAS_ID_DVBCA].func.identify
    = cas_adt_dvbca_identify;
  g_cas_priv.cam_op[CAS_ID_DVBCA].func.table_process
    = cas_adt_dvbca_table_process;
  g_cas_priv.cam_op[CAS_ID_DVBCA].func.io_ctrl
    = cas_adt_dvbca_io_ctrl;

  p_priv = g_cas_priv.cam_op[CAS_ID_DVBCA].p_priv
    = mtos_malloc(sizeof(cas_adt_dvbca_priv_t));
  MT_ASSERT(NULL != p_priv);
  memset(p_priv, 0x00, sizeof(cas_adt_dvbca_priv_t));

  p_priv->p_dmx_dev = p_cfg->p_dmx_dev;
  p_priv->mail_cfg.max_mail_num = p_cfg->max_mail_num;
  p_priv->mail_cfg.mail_policy = p_cfg->mail_policy;
  p_priv->filter_mode = p_cfg->filter_mode;

  p_priv->task_priv.flash_start_add = p_cfg->flash_start_adr;
  p_priv->task_priv.flash_size = p_cfg->flash_size;
  p_priv->task_priv.task_prio = p_cfg->task_prio;
  p_priv->monitor_priv.task_prio_monitor = p_cfg->end_task_prio;
  *p_cam_id = (u32)(&g_cas_priv.cam_op[CAS_ID_DVBCA]);
  DEBUG(DVBCAS_PORTING,INFO,"dvbca attach over!\n");
  
  return SUCCESS;
}

