/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef _CALENDAR_DEF_H_
#define _CALENDAR_DEF_H_

#include "ui_common.h"

#define CALENDAR_START_YEAR   1901
#define CALENDAR_END_YEAR     2099

/*!
  Calendar block
  */
#define CALENDAR_BLOCK_ID           0x90

enum data_id
{
		DATA_LMONTHDAY = 0,
		DATA_LMONTH,
		DATA_CAL,
		DATA_INDEX,
		DATA_LHOLDAY,
		DATA_YEAR_CODE,
		DATA_SOLAR_TERM,
		DATA_ANIMAL,
		DATA_DAY_CODE,
		DATA_EARTH,
		DATA_MONTH_CODE,
		DATA_SKY,
		DATA_ALMANAC,
		DATA_NUM,
};

struct data_head
{
	u16 data_offset;
	u16 length;
	u8 data_id;
};

struct calendar_head
{
	u8 total_num;
};

/*
 * ���빫�����ڵõ�����(ֻ����1901-2099��)
 */
u8 get_weekday(u16 year, u8 month, u8 day);

/*
 * ���빫�����µõ���������(ֻ����1901-2099��)
 */
u8 get_monthdays(u16 year, u8 month);

/*
 * ���빫�����ڵõ���Ӧ��ũ������(ֻ����1901-2099��)�������ض�Ӧ�Ľ�������(1-24, 0��ʾ�޶�Ӧ����)
 */
u8 get_lunar_date(u16 year, u8 month, u8 day,
                  u16 *lunar_year, u8 *lunar_month, u8 *lunar_day);

/*
 * ���빫�����ڵõ���Ӧ��ũ�������ַ��� "���������³�һ" (ֻ����1901-2099��)
 */
void get_lunar_str(u16 year, u8 month, u8 day, u16 *unistr, u16 max_len);

/*
 * ��ũ����ݸ�ʽ��Ϊ��Ф���ַ���
 */
void format_animal(u16 year, u16 *unistr, u16 max_len);

/*
 * ��ũ����ݸ�ʽ��Ϊ��ɵ�֧���ַ���
 */
void format_chinese_era(u16 year, u16 *unistr, u16 max_len);

/*
 * ���·ݸ�ʽ��Ϊ�����ַ���
 */
void format_lunar_month(u8 month, u16 *unistr, u16 max_len);

/*
 * ��ũ�����ڸ�ʽ��Ϊ�����ַ���
 */
void format_lunar_day(u8 day, u16 *unistr, u16 max_len);

/*
 * �ѽ��������ʽ��Ϊ�����ַ���
 */
void format_solar_term(u8 solar_term, u16 *unistr, u16 max_len);

/*
 * ���빫�����ڵõ���Ӧ�Ļ�������(ֻ����2005-2007��)
 */
void get_pattern(u16 year, u8 month, u8 day, u8 *pattern);

/*
 * ���ݻ������ݣ���ȡÿ���˼���Ϣ
 */
void format_almanac_info(u8 *pattern, 
                         u16 *yi_unistr, u8 *yi_cnt, u16 yi_max_len, 
                         u16 *ji_unistr, u8 *ji_cnt, u16 ji_max_len);

/*
 * ��ʼ�����ݿ���FLASH�ϵ�ƫ��
 */
void calendar_init(void);

#endif

