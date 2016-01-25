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
 * 输入公历日期得到星期(只允许1901-2099年)
 */
u8 get_weekday(u16 year, u8 month, u8 day);

/*
 * 输入公历年月得到该月天数(只允许1901-2099年)
 */
u8 get_monthdays(u16 year, u8 month);

/*
 * 输入公历日期得到对应的农历日期(只允许1901-2099年)，并返回对应的节气编码(1-24, 0表示无对应节气)
 */
u8 get_lunar_date(u16 year, u8 month, u8 day,
                  u16 *lunar_year, u8 *lunar_month, u8 *lunar_day);

/*
 * 输入公历日期得到对应的农历日期字符串 "甲子年正月初一" (只允许1901-2099年)
 */
void get_lunar_str(u16 year, u8 month, u8 day, u16 *unistr, u16 max_len);

/*
 * 把农历年份格式化为生肖年字符串
 */
void format_animal(u16 year, u16 *unistr, u16 max_len);

/*
 * 把农历年份格式化为天干地支纪字符串
 */
void format_chinese_era(u16 year, u16 *unistr, u16 max_len);

/*
 * 把月份格式化为中文字符串
 */
void format_lunar_month(u8 month, u16 *unistr, u16 max_len);

/*
 * 把农历日期格式化为中文字符串
 */
void format_lunar_day(u8 day, u16 *unistr, u16 max_len);

/*
 * 把节气编码格式化为中文字符串
 */
void format_solar_term(u8 solar_term, u16 *unistr, u16 max_len);

/*
 * 输入公历日期得到对应的黄历数据(只允许2005-2007年)
 */
void get_pattern(u16 year, u8 month, u8 day, u8 *pattern);

/*
 * 根据黄历数据，提取每日宜忌信息
 */
void format_almanac_info(u8 *pattern, 
                         u16 *yi_unistr, u8 *yi_cnt, u16 yi_max_len, 
                         u16 *ji_unistr, u8 *ji_cnt, u16 ji_max_len);

/*
 * 初始化数据块在FLASH上的偏移
 */
void calendar_init(void);

#endif

