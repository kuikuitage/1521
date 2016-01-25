#ifndef __CUSTOMER_DEF_H__
#define __CUSTOMER_DEF_H__

#define FRONT_BOARD_DEMO                              1
#define FRONT_BOARD_XIONGMAO                    2
#define FRONT_BOARD_TONGGUANG                 3
#define FRONT_BOARD_CHANGHONG                 4
#define FRONT_BOARD_JINGHUICHENG              5
#define FRONT_BOARD_JINYA                             6
#define FRONT_BOARD_PANLONGHU                  7
#define FRONT_BOARD_BOYUAN                         8
#define FRONT_BOARD_TAIHUI                           9
#define FRONT_BOARD_YINGJI                             10
#define FRONT_BOARD_XINSHIDA                        11
#define FRONT_BOARD_HAIHUIDA                        12
#define FRONT_BOARD_JINGGONG                        13
#define FRONT_BOARD_BOYUAN_ON_BROAD      14
#define FRONT_BOARD_YINHE                               15
#define FRONT_BOARD_YINHE_NANMEI               16
#define FRONT_BOARD_SHIDA                              17
#define FRONT_BOARD_PANLONGHU_TR             18
#define FRONT_BOARD_FANTONG                        19
#define FRONT_BOARD_TEFA                               20
#define FRONT_BOARD_CHANGJIANG                  21
#define FRONT_BOARD_JIUZHOU                 22

#define CONFIG_CAS_ID_ADT_MG      1
#define CONFIG_CAS_ID_TR                2
#define CONFIG_CAS_ID_KV                3
#define CONFIG_CAS_ID_TF                4
#define CONFIG_CAS_ID_SV                5
#define CONFIG_CAS_ID_DVN             6
#define CONFIG_CAS_ID_ST                7
#define CONFIG_CAS_ID_BY                8
#define CONFIG_CAS_ID_CONAXK      9
#define CONFIG_CAS_ID_GS               10 /***quanzhi****/
#define CONFIG_CAS_ID_DS               11
#define CONFIG_CAS_ID_GT               12
#define CONFIG_CAS_ID_WF              13
#define CONFIG_CAS_ID_SHIDA        14
#define CONFIG_CAS_ID_P4V            15
#define CONFIG_CAS_ID_YJ               16
#define CONFIG_CAS_ID_IRDETO       17
#define CONFIG_CAS_ID_YXSB           18
#define CONFIG_CAS_ID_XSM           19
#define CONFIG_CAS_ID_MAX_NUM      20
#define CONFIG_CAS_UNKNOWN            21
#define CONFIG_CAS_ID_DVBCA			22
#define CONFIG_CAS_ID_QL			23



#define CONFIG_ADS_ID_NOADS            1  /***no ads function****/
#define CONFIG_ADS_ID_BOYUAN          2
#define CONFIG_ADS_ID_TR                    3    /***tengrui****/
#define CONFIG_ADS_ID_DESAI              4
#define CONFIG_ADS_ID_GS                    5   /***quanzhi****/
#define CONFIG_ADS_ID_SZXC              6    /***szxc 2.0.2ads***/
#define CONFIG_ADS_ID_XSM				7	/***XSM2.0***/
#define CONFIG_ADS_ID_UNKNOWN      8




typedef enum
{
  CUSTOMER_DEFAULT,
  CUSTOMER_DEMO,
  CUSTOMER_BOYUAN,
  CUSTOMER_WANGFA,
  CUSTOMER_JINGHUICHENG_QZ,
  CUSTOMER_JINGHUICHENG_WF,
  CUSTOMER_JINGHUICHENG_TH,
  CUSTOMER_PANLONGHU,
  CUSTOMER_YINHE,
  CUSTOMER_TAIHUI,
  CUSTOMER_YINGJI,
  CUSTOMER_XINSHIDA,
  CUSTOMER_JINGGONG,
  CUSTOMER_ZHONGDA,
  CUSTOMER_HAIHUIDA,
  CUSTOMER_TONGGUANG,
  CUSTOMER_CHANGHONG,
  CUSTOMER_JINGHUICHENG,
  CUSTOMER_JINYA,
  CUSTOMER_SHIDA,
  CUSTOMER_XIONGMAO,
  CUSTOMER_TONGGUANG_CG,
  CUSTOMER_TONGGUANG_QY,
  CUSTOMER_PANLONGHU_TR,
  CUSTOMER_CHANGJIANG,
  CUSTOMER_CHANGJIANG_LQ,
  CUSTOMER_CHANGJIANG_NY,
  CUSTOMER_CHANGJIANG_QY,
  CUSTOMER_CHANGJIANG_JS,
  CUSTOMER_FANTONG,
  CUSTOMER_FANTONG_KF,
  CUSTOMER_FANTONG_KFAJX,
  CUSTOMER_FANTONG_KF_SZXC312,
  CUSTOMER_FANTONG_BYAJX,
  CUSTOMER_FANTONG_XSMAJX,
  CUSTOMER_DESAI,
  CUSTOMER_XINSIDA_LQ,
  CUSTOMER_TDX_PAKISTAN,
  CUSTOMER_TEFA,
  CUSTOMER_JIUZHOU,
  CUSTOMER_ZHILING,
  CUSTOMER_ZHILING_LQ,
  CUSTOMER_ZHILING_KF,
  CUSTOMER_XINSIDA_SPAISH,
  CUSTOMER_HEBI,
  CUSTOMER_SHENZHOU_QN,
  CUSTOMER_TEFA_DS5,
  CUSTOMER_JIZHONG_SHANXI,
  CUSTOMER_CHANGJIANG_SHANGJIE,
  CUSTOMER_YINHE_GUYE,
  CUSTOMER_YINHE_XIANGCHENG,
  CUSTOMER_CHANGJIANG_ZHECHENG,
  CUSTOMER_YINHE_CANGZHOU,
  CUSTOMER_YINHE_LUANXIAN,
  CUSTOMER_AOJIEXUN,
}CUSTOMER_ID;
typedef enum
{
  UIO_LED_TYPE_4D = 0,
  UIO_LED_TYPE_3D,  
} CUSTOMER_UIO_LED_TYPE;

typedef struct customer_cfg
{
  u8 customer;
}customer_cfg_t;

void set_customer_config(void);
void get_customer_config(customer_cfg_t *p);
u8 get_uio_led_number_type(void);
#endif //__SYS_DEVS_H__

