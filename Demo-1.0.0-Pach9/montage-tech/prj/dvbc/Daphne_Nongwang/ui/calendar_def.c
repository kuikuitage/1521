/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "calendar_def.h"
#include <util/gb2312.h>
#include <mdl/data_manager.h>

static u16 data_offset[DATA_NUM];

/*static u8 *almanac_str[] = 
{
  "����", "��Ȣ", "����", "��լ", "����", "����", "����", "����", 
  "��", "����", "�ɲ�", "����", "����", "����", "����", "����",
  "����", "������", "����", "���", "����", "��ȯ", "ի��", "����",
  "����", "����", "����", "����", "��ԡ", "����", "���", "����",
  "�ɷ�", "����", "����", "����", "��", "�ɲ�", "��ж", "�β�",
  "����", "��ľ", "����", "��׽", "����", "����", "л��", "���˿�",
  "����", "��ɥ", "����е", "��", "�ܻ�", "�޷�", "����", "���",
  "ɨ��", "��Ѩ", "����", "����", "��ҽ", "����", "����", "��ԫ",
  "����", "����", "����", "ƽ�ε�Ϳ", "����", "����", "����", "�촬",
  "�ò�", "����ľ", "̽��", "��ˮ", "���²���", "��ԫ", "����", "�쳵��",
  "��ѧ", "��ţ��", "������", "����ԫǽ", "������", "����", "����",
  "ȡ��", "����", "���", "������", "�ϼ�", "����", "����", "���",
  "���", "�ն�", "�������", "ϰ��", "����", "����", "����", "����",
  "���", "����", "�־�", "����", "����", "����", "������ȡ"
};*/
/*
static u8 *solar_term_str[] = 
{
  "С��", "��", "����", "��ˮ",
  "����", "����", "����", "����",
  "����", "С��", "â��", "����",
  "С��", "����", "����", "����",
  "��¶", "���", "��¶", "˪��",
  "����", "Сѩ", "��ѩ", "����" 
};*/
/*
static u8 *animal_str[] =
{
  "��", "ţ", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��" 
};*/

/*static u8 *sky[] =
{ 
  "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", 
};*/

/*static u8 *earth[] =
{ 
  "��", "��", "��", "î", "��", "��", "��", "δ", "��", "��", "��", "��",
};*/
/*
static u8 *month_code[] =
{ 
  "һ", "��", "��", "��", "��", "��", "��", "��", "��", "ʮ", "��", "��",
};

static u8 *day_code[] = 
{ 
  "��", "ʮ", "إ", "��", 
};*/

static u16 calender_read_data(u8 id, u16 index);

static void calender_read_gbstr(u8 id, u16 index, u16 *buf);

//static void cdata_copy(u8 *target, u8 *source, u8 len);

static BOOL is_leap_year(u16 year);

static s32 calc_date_diff(u16 end_year, u8 end_month, u8 end_day,
                   u16 start_year, u8 start_month, u8 start_day);

static u8 get_solar_term(u16 year, u16 month, u16 day);

static u8 get_lunar_moondays(u8 month_p, u16 table_addr);

static u8 get_lunar_info(u32 year, u8 month, u8 day, u8 *p);


/*
 * �������Ӧ��ũ������,ÿ�����ֽ�,
 * ��ʽ��һ�ֽ�BIT7-4 λ��ʾ�����·�,ֵΪ0 Ϊ������,BIT3-0 ��Ӧũ����1-4 �µĴ�С
 * �ڶ��ֽ�BIT7-0 ��Ӧũ����5-12 �´�С,�����ֽ�BIT7 ��ʾũ����13 ���´�С
 * �·ݶ�Ӧ��λΪ1 ��ʾ��ũ���´�(30 ��),Ϊ0 ��ʾС(29 ��).
 * �����ֽ�BIT6-5 ��ʾ���ڵĹ����·�,BIT4-0 ��ʾ���ڵĹ�������
 */
/*static const u8 year_code[] =
{
  0x04, 0xAe, 0x53, //1901 0
  0x0A, 0x57, 0x48, //1902 3
  0x55, 0x26, 0xBd, //1903 6
  0x0d, 0x26, 0x50, //1904 9
  0x0d, 0x95, 0x44, //1905 12
  0x46, 0xAA, 0xB9, //1906 15
  0x05, 0x6A, 0x4d, //1907 18
  0x09, 0xAd, 0x42, //1908 21
  0x24, 0xAe, 0xB6, //1909
  0x04, 0xAe, 0x4A, //1910
  0x6A, 0x4d, 0xBe, //1911
  0x0A, 0x4d, 0x52, //1912
  0x0d, 0x25, 0x46, //1913
  0x5d, 0x52, 0xBA, //1914
  0x0B, 0x54, 0x4e, //1915
  0x0d, 0x6A, 0x43, //1916
  0x29, 0x6d, 0x37, //1917
  0x09, 0x5B, 0x4B, //1918
  0x74, 0x9B, 0xC1, //1919
  0x04, 0x97, 0x54, //1920
  0x0A, 0x4B, 0x48, //1921
  0x5B, 0x25, 0xBC, //1922
  0x06, 0xA5, 0x50, //1923
  0x06, 0xd4, 0x45, //1924
  0x4A, 0xdA, 0xB8, //1925
  0x02, 0xB6, 0x4d, //1926
  0x09, 0x57, 0x42, //1927
  0x24, 0x97, 0xB7, //1928
  0x04, 0x97, 0x4A, //1929
  0x66, 0x4B, 0x3e, //1930
  0x0d, 0x4A, 0x51, //1931
  0x0e, 0xA5, 0x46, //1932
  0x56, 0xd4, 0xBA, //1933
  0x05, 0xAd, 0x4e, //1934
  0x02, 0xB6, 0x44, //1935
  0x39, 0x37, 0x38, //1936
  0x09, 0x2e, 0x4B, //1937
  0x7C, 0x96, 0xBf, //1938
  0x0C, 0x95, 0x53, //1939
  0x0d, 0x4A, 0x48, //1940
  0x6d, 0xA5, 0x3B, //1941
  0x0B, 0x55, 0x4f, //1942
  0x05, 0x6A, 0x45, //1943
  0x4A, 0xAd, 0xB9, //1944
  0x02, 0x5d, 0x4d, //1945
  0x09, 0x2d, 0x42, //1946
  0x2C, 0x95, 0xB6, //1947
  0x0A, 0x95, 0x4A, //1948
  0x7B, 0x4A, 0xBd, //1949
  0x06, 0xCA, 0x51, //1950
  0x0B, 0x55, 0x46, //1951
  0x55, 0x5A, 0xBB, //1952
  0x04, 0xdA, 0x4e, //1953
  0x0A, 0x5B, 0x43, //1954
  0x35, 0x2B, 0xB8, //1955
  0x05, 0x2B, 0x4C, //1956
  0x8A, 0x95, 0x3f, //1957
  0x0e, 0x95, 0x52, //1958
  0x06, 0xAA, 0x48, //1959
  0x7A, 0xd5, 0x3C, //1960
  0x0A, 0xB5, 0x4f, //1961
  0x04, 0xB6, 0x45, //1962
  0x4A, 0x57, 0x39, //1963
  0x0A, 0x57, 0x4d, //1964
  0x05, 0x26, 0x42, //1965
  0x3e, 0x93, 0x35, //1966
  0x0d, 0x95, 0x49, //1967
  0x75, 0xAA, 0xBe, //1968
  0x05, 0x6A, 0x51, //1969
  0x09, 0x6d, 0x46, //1970
  0x54, 0xAe, 0xBB, //1971
  0x04, 0xAd, 0x4f, //1972
  0x0A, 0x4d, 0x43, //1973
  0x4d, 0x26, 0xB7, //1974
  0x0d, 0x25, 0x4B, //1975
  0x8d, 0x52, 0xBf, //1976
  0x0B, 0x54, 0x52, //1977
  0x0B, 0x6A, 0x47, //1978
  0x69, 0x6d, 0x3C, //1979
  0x09, 0x5B, 0x50, //1980
  0x04, 0x9B, 0x45, //1981
  0x4A, 0x4B, 0xB9, //1982
  0x0A, 0x4B, 0x4d, //1983
  0xAB, 0x25, 0xC2, //1984
  0x06, 0xA5, 0x54, //1985
  0x06, 0xd4, 0x49, //1986
  0x6A, 0xdA, 0x3d, //1987
  0x0A, 0xB6, 0x51, //1988
  0x09, 0x37, 0x46, //1989
  0x54, 0x97, 0xBB, //1990
  0x04, 0x97, 0x4f, //1991
  0x06, 0x4B, 0x44, //1992
  0x36, 0xA5, 0x37, //1993
  0x0e, 0xA5, 0x4A, //1994
  0x86, 0xB2, 0xBf, //1995
  0x05, 0xAC, 0x53, //1996
  0x0A, 0xB6, 0x47, //1997
  0x59, 0x36, 0xBC, //1998
  0x09, 0x2e, 0x50, //1999 294
  0x0C, 0x96, 0x45, //2000 297
  0x4d, 0x4A, 0xB8, //2001
  0x0d, 0x4A, 0x4C, //2002
  0x0d, 0xA5, 0x41, //2003
  0x25, 0xAA, 0xB6, //2004
  0x05, 0x6A, 0x49, //2005
  0x7A, 0xAd, 0xBd, //2006
  0x02, 0x5d, 0x52, //2007
  0x09, 0x2d, 0x47, //2008
  0x5C, 0x95, 0xBA, //2009
  0x0A, 0x95, 0x4e, //2010
  0x0B, 0x4A, 0x43, //2011
  0x4B, 0x55, 0x37, //2012
  0x0A, 0xd5, 0x4A, //2013
  0x95, 0x5A, 0xBf, //2014
  0x04, 0xBA, 0x53, //2015
  0x0A, 0x5B, 0x48, //2016
  0x65, 0x2B, 0xBC, //2017
  0x05, 0x2B, 0x50, //2018
  0x0A, 0x93, 0x45, //2019
  0x47, 0x4A, 0xB9, //2020
  0x06, 0xAA, 0x4C, //2021
  0x0A, 0xd5, 0x41, //2022
  0x24, 0xdA, 0xB6, //2023
  0x04, 0xB6, 0x4A, //2024
  0x69, 0x57, 0x3d, //2025
  0x0A, 0x4e, 0x51, //2026
  0x0d, 0x26, 0x46, //2027
  0x5e, 0x93, 0x3A, //2028
  0x0d, 0x53, 0x4d, //2029
  0x05, 0xAA, 0x43, //2030
  0x36, 0xB5, 0x37, //2031
  0x09, 0x6d, 0x4B, //2032
  0xB4, 0xAe, 0xBf, //2033
  0x04, 0xAd, 0x53, //2034
  0x0A, 0x4d, 0x48, //2035
  0x6d, 0x25, 0xBC, //2036
  0x0d, 0x25, 0x4f, //2037
  0x0d, 0x52, 0x44, //2038
  0x5d, 0xAA, 0x38, //2039
  0x0B, 0x5A, 0x4C, //2040
  0x05, 0x6d, 0x41, //2041
  0x24, 0xAd, 0xB6, //2042
  0x04, 0x9B, 0x4A, //2043
  0x7A, 0x4B, 0xBe, //2044
  0x0A, 0x4B, 0x51, //2045
  0x0A, 0xA5, 0x46, //2046
  0x5B, 0x52, 0xBA, //2047
  0x06, 0xd2, 0x4e, //2048
  0x0A, 0xdA, 0x42, //2049
  0x35, 0x5B, 0x37, //2050
  0x09, 0x37, 0x4B, //2051
  0x84, 0x97, 0xC1, //2052
  0x04, 0x97, 0x53, //2053
  0x06, 0x4B, 0x48, //2054
  0x66, 0xA5, 0x3C, //2055
  0x0e, 0xA5, 0x4f, //2056
  0x06, 0xB2, 0x44, //2057
  0x4A, 0xB6, 0x38, //2058
  0x0A, 0xAe, 0x4C, //2059
  0x09, 0x2e, 0x42, //2060
  0x3C, 0x97, 0x35, //2061
  0x0C, 0x96, 0x49, //2062
  0x7d, 0x4A, 0xBd, //2063
  0x0d, 0x4A, 0x51, //2064
  0x0d, 0xA5, 0x45, //2065
  0x55, 0xAA, 0xBA, //2066
  0x05, 0x6A, 0x4e, //2067
  0x0A, 0x6d, 0x43, //2068
  0x45, 0x2e, 0xB7, //2069
  0x05, 0x2d, 0x4B, //2070
  0x8A, 0x95, 0xBf, //2071
  0x0A, 0x95, 0x53, //2072
  0x0B, 0x4A, 0x47, //2073
  0x6B, 0x55, 0x3B, //2074
  0x0A, 0xd5, 0x4f, //2075
  0x05, 0x5A, 0x45, //2076
  0x4A, 0x5d, 0x38, //2077
  0x0A, 0x5B, 0x4C, //2078
  0x05, 0x2B, 0x42, //2079
  0x3A, 0x93, 0xB6, //2080
  0x06, 0x93, 0x49, //2081
  0x77, 0x29, 0xBd, //2082
  0x06, 0xAA, 0x51, //2083
  0x0A, 0xd5, 0x46, //2084
  0x54, 0xdA, 0xBA, //2085
  0x04, 0xB6, 0x4e, //2086
  0x0A, 0x57, 0x43, //2087
  0x45, 0x27, 0x38, //2088
  0x0d, 0x26, 0x4A, //2089
  0x8e, 0x93, 0x3e, //2090
  0x0d, 0x52, 0x52, //2091
  0x0d, 0xAA, 0x47, //2092
  0x66, 0xB5, 0x3B, //2093
  0x05, 0x6d, 0x4f, //2094
  0x04, 0xAe, 0x45, //2095
  0x4A, 0x4e, 0xB9, //2096
  0x0A, 0x4d, 0x4C, //2097
  0x0d, 0x15, 0x41, //2098
  0x2d, 0x92, 0xB5, //2099
};*/

/*
 * �·����ݱ�
 */
static const u8 day_code1[9] =
{ 0x0, 0x1f, 0x3b, 0x5a, 0x78, 0x97, 0xb5, 0xd4, 0xf3 };
static const u16 day_code2[3] =
{ 0x111, 0x130, 0x14e };


static void calendar_read_data(u32 offset, u8 *p_buffer, u32 lens)
{
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  dm_read(p_dm_handle, CALENDAR_BLOCK_ID, 0, (u16)offset, lens, p_buffer);
}


/****************************************************************************
* �Ӻ���,���ڶ�ȡ���ݱ���ũ���µĴ��»�С��,�������Ϊ�󷵻�30,ΪС����29
****************************************************************************/
static u8 get_lunar_moondays(u8 month_p, u16 table_addr)
{
  u8 mask = 0x1;
  u8 year_code = 0;

  if (month_p == 0)
  {
    return 0;
  }

  if (month_p > 4)
  {
    table_addr++;
    
    if (month_p < 9)
    {
      mask = 0x10;
    }
  }

  if (month_p > 12)
  {
    table_addr++;
  }

  mask <<= 4 - (((month_p - 1) % 4) + 1);

//  if ((year_code[table_addr] & mask) == 0)
  year_code = (u8)calender_read_data(DATA_YEAR_CODE, table_addr);
  if((year_code & mask) == 0)
  {
    return 29; //С��29��
  }
  else
  {
    return 30; //����30��
  }
}

/*********************************************************************************************************
 * ��������:get_lunar_info
 * ��������:��ũ��ת��(ֻ����1901-2099��)
 * �䡡��:  year        ������
 *          month       ������
 *          day         ������
 *          p           ����ũ�����ڵ�ַ
 * �䡡��:  1           �ɹ�
 *          0           ʧ��
 ********************************************************************************************************/
static u8 get_lunar_info(
  u32 year,
  u8 month,
  u8 day,
  u8 *p)
{
  u8 temp1, temp2, temp3, month_p, year_h, year_l;
  u8 flag_y;
  u16 temp4, table_addr;

  year_h = (u8)(year / 100); year_l = (u8)(year % 100);
  if ((year_h != 19) && (year_h != 20))
  {
    return(0);
  }

  /* ��λ���ݱ��ַ */
  if (year_h == 20)
  {
    table_addr = (year_l + 100 - 1) * 3;
  }
  else
  {
    table_addr = (year_l - 1) * 3;
  }

  /* ȡ���괺�����ڵĹ����·� */
//  temp1 = year_code[table_addr + 2] & 0x60;
  temp1 = (u8)calender_read_data(DATA_YEAR_CODE, table_addr + 2);
  temp1 &= 0x60;
  temp1 >>= 5;

  /* ȡ���괺�����ڵĹ����� */
//  temp2 = year_code[table_addr + 2] & 31;
  temp2 = (u8)calender_read_data(DATA_YEAR_CODE, table_addr + 2);
  temp2 &= 31;
  /* ���㵱�괺���뵱��Ԫ��������,����ֻ���ڹ���1�»�2�� */
  if (temp1 == 1)
  {
    temp3 = temp2 - 1;
  }
  else
  {
    temp3 = temp2 + 31 - 1;
  }

  /* ���㹫�����뵱��Ԫ�������� */
  if (month < 10)
  {
    temp4 = day_code1[month - 1] + day - 1;
  }
  else
  {
    temp4 = day_code2[month - 10] + day - 1;
  }
  /* ��������´���2�²��Ҹ����2��Ϊ����,������1 */
  if ((month > 2) && (year_l % 4 == 0))
  {
    temp4++;
  }

  /* �жϹ������ڴ���ǰ���Ǵ��ں� */
  if (temp4 >= temp3)
  {
    temp4 -= temp3;
    month = 1;
    month_p = 1;

    flag_y = 0;
    temp1 = get_lunar_moondays(month_p, table_addr);

    /* �����ݱ���ȡ����������·�,��Ϊ0����������� */
//    temp2 = year_code[table_addr] / 16;
    temp2 = (u8)calender_read_data(DATA_YEAR_CODE, table_addr);
    temp2 /= 16;
    
    while (temp4 >= temp1)
    {
      temp4 -= temp1;
      month_p++;
      if (month == temp2)
      {
        flag_y = ~flag_y;
        if (flag_y == 0)
        {
          month++;
        }
      }
      else
      {
        month++;
      }
      temp1 = get_lunar_moondays(month_p, table_addr);
    }
    day = temp4 + 1;
  }
  /* �������ڴ���ǰʹ���������������� */
  else
  {
    temp3 -= temp4;
    if (year_l == 0)
    {
      year_l = 100 - 1;
      year_h = 19;
    }
    else
    {
      year_l--;
    }
    table_addr -= 3;
    month = 12;
//    temp2 = year_code[table_addr] / 16;
    temp2 = (u8)calender_read_data(DATA_YEAR_CODE, table_addr);
    temp2 /= 16;
    
    if (temp2 == 0)
    {
      month_p = 12;
    }
    else
    {
      month_p = 13;
    }

    flag_y = 0;
    temp1 = get_lunar_moondays(month_p, table_addr);

    while (temp3 > temp1)
    {
      temp3 -= temp1;
      month_p--;
      if (flag_y == 0)
      {
        month--;
      }
      if (month == temp2)
      {
        flag_y = ~flag_y;
      }
      temp1 = get_lunar_moondays(month_p, table_addr);
    }
    day = temp1 - temp3 + 1;
  }

  *p++ = year_h;
  *p++ = year_l;
  *p++ = month;
  *p = day;
  return(1);
}


static u8 const table_week[12] =
{ 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 };                              //���������ݱ�
/*********************************************************************************************************
 * ��������:GetWeek
 * ��������:���빫�����ڵõ�����(ֻ����1901-2099��)
 * �䡡��:  year        ������
 *          month       ������
 *          day	        ������
 *          p           �������ڵ�ַ
 * �䡡��:  ��
 ********************************************************************************************************/
u8 get_weekday(
  u16 year,
  u8 month,
  u8 day)
{
  u32 temp2;
  u8 year_h, year_l;

  year_h = year / 100; year_l = year % 100;

  /* ���Ϊ21����,�������100 */
  if (year_h > 19)
  {
    year_l += 100;
  }
  /* ����������ֻ��1900��֮��� */
  temp2 = year_l + year_l / 4;
  temp2 = temp2 % 7;
  temp2 = temp2 + day + table_week[month - 1];
  if (year_l % 4 == 0 && month < 3)
  {
    temp2--;
  }

  return (u8)(temp2 % 7);
}


/*********************************************************************************************************
 * ��������:get_chinese_era
 * ��������:���빫�����ڵõ�һ��������(ֻ����1901-2099��)
 * �䡡��:  year        ������
 *          p           �������ڵ�ַ
 * �䡡��:  ��
 ********************************************************************************************************/
static void get_chinese_era(u32 year, u8 *p)
{
  u8 x;

  if (year >= 1984)
  {
    year = year - 1984;
    x = year % 60;
  }
  else
  {
    year = 1984 - year;
    x = 60 - year % 60;
  }
  *p = x;
}


/*********************************************************************************************************
 * ��������:convert_lunar_str
 * ��������:���빫�����ڵõ�ũ���ַ���
 *          ��:convert_lunar_str(2007,02,06,str) ����str="����������ʮ��"
 * �䡡��:  year        ������
 *          month       ������
 *          day         ������
 *          str         ����ũ�������ַ�����ַ   15Byte
 * �䡡��:  ��
 ********************************************************************************************************/
void convert_lunar_str(
  u32 year,
  u8 month,
  u8 day,
//  char *str)
  u16 *str)
{
  u8 lunar_info[4];
  u8 era_year;
  u16 tmp[2];

//  cdata_copy(&str[0], (u8 *)"���������³�һ", 15);
  if (get_lunar_info(year, month, day, (u8 *)lunar_info) == 0)
  {
    return;
  }
  get_chinese_era(lunar_info[0] * 100 + lunar_info[1], &era_year);
//  cdata_copy(&str[0], (u8 *)sky[era_year % 10], 2);   //  ��
//  cdata_copy(&str[2], (u8 *)earth[era_year % 12], 2); //  ��
  calender_read_gbstr(DATA_SKY, (era_year % 10), str);
  calender_read_gbstr(DATA_EARTH, (era_year % 12), tmp);
  *(str + 1) = tmp[0];
  *(str + 2) = 0x5e74; //��
  
/*  if (lunar_info[2] == 1)
  {
    cdata_copy(&str[6], (u8 *)"��", 2);
  }
  else
  {
    cdata_copy(&str[6], (u8 *)month_code[lunar_info[2] - 1], 2);
  }*/
  if(lunar_info[2] == 1)
  {
    *(str + 3) = 0x6b63;//��
  }
  else
  {
    calender_read_gbstr(DATA_MONTH_CODE, (lunar_info[2] - 1), tmp);
    *(str + 3) = tmp[0];
  }
  *(str + 4) = 0x6708;//��
/*  if (lunar_info[3] > 10)
  {
    cdata_copy(&str[10], (u8 *)day_code[lunar_info[3] / 10], 2);
  }
  else
  {
    cdata_copy(&str[10], (u8 *)"��", 2);
  }*/
  if(lunar_info[3] == 10)
  {
  *(str + 5) = 0x521d;//��  
  }
  else
  {
    calender_read_gbstr(DATA_DAY_CODE, lunar_info[3]/10, tmp);
    *(str + 5) = tmp[0];
  }

  
    
//  cdata_copy(&str[12], (u8 *)month_code[(lunar_info[3] - 1) % 10], 2);
  calender_read_gbstr(DATA_MONTH_CODE, (lunar_info[3] - 1) % 10, tmp);
  *(str + 6) = tmp[0];
  *(str + 7) = 0;
}

/*
static void cdata_copy(u8 *target, u8 *source, u8 len)
{
  while(len--)
  {
    *target++ = *source++;
  }
}
*/

static BOOL is_leap_year(u16 year)
{
  return !((year % 4) && (year % 100)) || !(year % 400);
}


static s32 calc_date_diff(u16 end_year, u8 end_month, u8 end_day,
                   u16 start_year, u8 start_month, u8 start_day)
{
  u16 monthday[] =
  { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

//�����������1��1��֮����������
  s32 diff_days = (end_year - start_year) * 365;
  diff_days += (end_year - 1) / 4 - (start_year - 1) / 4;
  diff_days -= ((end_year - 1) / 100 - (start_year - 1) / 100);
  diff_days += (end_year - 1) / 400 - (start_year - 1) / 400;

  //����end_year��1��1�յ�end_month��end_day��֮�������
  diff_days += monthday[end_month - 1] +
               (is_leap_year(end_year) && end_month > 2 ? 1 : 0);
  diff_days += end_day;

//��ȥstart_year��1��1�յ�start_month��start_day��֮�������
  diff_days -= (monthday[start_month - 1] +
                (is_leap_year(start_year) && start_month > 2 ? 1 : 0));
  diff_days -= start_day;
  
  return diff_days;
}


//���ݽ������ݴ洢��ʽ,��������year��month��day�ն�Ӧ�Ľ���,
static u8 get_solar_term(u16 year, u16 month, u16 day)
{
  u16 nday;
  u8 flag =
    (u8)calender_read_data(DATA_LHOLDAY,
                      (u16)((year - CALENDAR_START_YEAR) * 12 + month - 1));
  
  if (day < 15)
  {
    nday = 15 - ((flag >> 4) & 0x0f);
  }
  else
  {
    nday = ((flag) & 0x0f) + 15;
  }
  if (nday == day)
  {
    return (month - 1) * 2 + (day > 15 ? 1 : 0) + 1;
  }
  else
  {
    return 0;
  }
}


u8 get_lunar_date(u16 year, u8 month, u8 day,
                   u16 *ilunaryear, u8 *ilunarmonth, u8 *ilunarday)
{
  u8 lunar_data[4];

  get_lunar_info(year, month, day, lunar_data);

  *ilunaryear = lunar_data[0] * 100 + lunar_data[1];
  *ilunarmonth = lunar_data[2];
  *ilunarday = lunar_data[3];

  return get_solar_term(year, month, day);
}

void get_lunar_str(u16 year, u8 month, u8 day, u16 *unistr, u16 max_len)
{
//  u8 buf[16];

//  convert_lunar_str(year, month, day, buf);
//  convert_gb2312_to_unicode(buf, 15, unistr, max_len);
  convert_lunar_str(year, month, day, unistr);

}


u8 get_monthdays(u16 year, u8 month)
{
  switch (month)
  {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
      return 31;

      break;
    case 4: case 6: case 9: case 11:
      return 30;

      break;
    case 2:
      //���������
      if (is_leap_year(year))
      {
        return 29;
      }
      else
      {
        return 28;
      }
      break;
  }
  return 0;
}


void format_animal(u16 year, u16 *unistr, u16 max_len)
{
//  u8 buf[] = { "����" };

//  cdata_copy(buf, (u8 *)animal_str[(year + 8) % 12], 2);
//  convert_gb2312_to_unicode(buf, 4, unistr, max_len);
//  u16 uni_year = 0x5e74;//��
  
  calender_read_gbstr(DATA_ANIMAL, (year + 8) % 12, unistr);      
//  uni_strcat(unistr, &uni_year, 80);
  *(unistr + 1) = 0x5e74;
  *(unistr + 2) = 0;
}


void format_chinese_era(u16 year, u16 *unistr, u16 max_len)
{
//  u8 buf [] = { "������" };
  u8 era_year;
  u16 tmp = 0;
  
  get_chinese_era(year, &era_year);
  
//  cdata_copy(&buf[0], (u8 *)sky[era_year % 10], 2);   //  ��
//  cdata_copy(&buf[2], (u8 *)earth[era_year % 12], 2); //  ��

//  convert_gb2312_to_unicode(buf, 6, unistr, max_len);

  calender_read_gbstr(DATA_SKY, (era_year % 10), unistr);
  calender_read_gbstr(DATA_EARTH, (era_year % 12), &tmp);

  *(unistr + 1) = tmp;
  *(unistr + 2) = 0x5e74;//��
  *(unistr + 3) = 0;
}


void format_lunar_month(u8 month, u16 * unistr, u16 max_len)
{
//  u8 buf [] = { "����" };

  if (month != 1)
  {
//    cdata_copy(&buf[0], (u8 *)month_code[month - 1], 2);
    calender_read_gbstr(DATA_MONTH_CODE, month - 1, unistr);
  }
  else
  {
    *unistr = 0x6b63;//��
  }
  *(unistr + 1) = 0x6708;//�� 
  *(unistr + 2) = 0;
//  convert_gb2312_to_unicode(buf, 4, unistr, max_len);
}


void format_lunar_day(u8 day, u16 *unistr, u16 max_len)
{
  u16 tmp[2];
//  u8 buf [] = { "��һ" };

/*  if (day > 10)
  {
    cdata_copy(&buf[0], (u8 *)day_code[day / 10], 2);
  }
  else
  {
    cdata_copy(&buf[0], (u8 *)"��", 2);
  }*/
  if(day == 10)
  {
    *unistr = 0x521d;//"��"
  }
  else
  {
    calender_read_gbstr(DATA_DAY_CODE, day/10, unistr);
  }
//  cdata_copy(&buf[2], (u8 *)month_code[(day - 1) % 10], 2);
  calender_read_gbstr(DATA_MONTH_CODE, (day - 1)%10, tmp);
  *(unistr + 1) = tmp[0];
  *(unistr + 2) = 0;

//  convert_gb2312_to_unicode(buf, 4, unistr, max_len);
}


/* ��gCal��ȡ��ĳ�������չ����pattern���30�ֽڵ������� */
void get_pattern(u16 year, u8 month, u8 day, u8 * pattern)
{
  s32 days;
  u16 index_0, index_1;
  u8 cal;
  u16 i = 0;
  u16 j = 0;
  u16 counter;

//  if (year < 2005 || year > 2007)
//  {
//    return;
//  }
//  days = calc_date_diff(year, month, day, 2005, 1, 1);
  days = calc_date_diff(year, month, day, 1901, 1, 1)%1095;
  index_0 = calender_read_data(DATA_INDEX, (u16)days);
  index_1 = calender_read_data(DATA_INDEX, (u16)(days + 1));
  for (i = index_0; i < index_1;)
  {
    cal = (u8)calender_read_data(DATA_CAL, i);
    if (cal != 0xcc)
    {
      pattern[j++] = cal;
      i++;
    }
    else
    {
      i++;
      counter = (u8)calender_read_data(DATA_CAL, i);
      while (counter--)
      {
        pattern[j++] = 0;
      }
      i++;
    }
  }
}


/* ����pattern��ÿ���pattern����ȡÿ��Ĵʻ㣬��: yi_unistr����: ji_unistr��count�ֱ��Ǵ�ĸ��?*/
void format_almanac_info(u8 *pattern, 
                         u16 *yi_unistr, u8 *yi_cnt, u16 yi_max_len, 
                         u16 *ji_unistr, u8 *ji_cnt, u16 ji_max_len)
{
  s8 i, j;
  u16 buf[16];
  u32 len;

  *yi_cnt = 0;
  *ji_cnt = 0;

  for (i = 0; i < 15; i++)
  {
    for (j = 7; j >= 0; j--)
    {
      if (pattern[i] & (1 << j))
      {
//        convert_gb2312_to_unicode(almanac_str[i * 8 + 7 - j], 16, buf, 16);
        calender_read_gbstr(DATA_ALMANAC, i * 8 + 7 - j, buf);
        uni_strcat(yi_unistr, buf, yi_max_len);
        str_asc2uni((u8 *)",", buf);
        uni_strcat(yi_unistr, buf, yi_max_len);
        (*yi_cnt)++;
      }
    }
  }
  
  for (i = 29; i >= 15; i--)
  {
    for (j = 0; j <= 7; j++)
    {
      if (pattern[i] & (1 << j))
      {
//        convert_gb2312_to_unicode(almanac_str[(29 - i) * 8 + j], 16, buf, 16);
        calender_read_gbstr(DATA_ALMANAC, (29 - i) * 8 + j, buf);
        uni_strcat(ji_unistr, buf, ji_max_len);
        str_asc2uni((u8 *)",", buf);
        uni_strcat(ji_unistr, buf, ji_max_len);
        (*ji_cnt)++;
      }
    }
  }

  len = uni_strlen(yi_unistr);
  if (len > 0)
    yi_unistr[len-1] = '\0';

  len = uni_strlen(ji_unistr);
  if (len > 0)
    ji_unistr[len-1] = '\0';
}


void format_solar_term(u8 solar_term, u16 *unistr, u16 max_len)
{
  if (solar_term)
  {
//    convert_gb2312_to_unicode(solar_term_str[solar_term - 1], 4, unistr, max_len);
    calender_read_gbstr(DATA_SOLAR_TERM, (solar_term - 1), unistr);
  }
}
void calendar_init(void)
{
  u16 hdr_addr = 0;
  struct data_head head;
  u8 i;


  for (i = 0; i < DATA_NUM; i++)
  {
    hdr_addr = sizeof(struct calendar_head);

    hdr_addr += i * sizeof(struct data_head);
    calendar_read_data((hdr_addr), (u8 *)(&head),
                   sizeof(struct data_head));
    data_offset[i] = head.data_offset;
  }

  return;
}


static u16 calender_read_data(u8 id, u16 index)
{
  u16 offset = 0;
  u8 buf8 = 0;
  u16 buf16 = 0;

  offset = data_offset[id];
  switch (id)
  {
    case DATA_LMONTHDAY:
    case DATA_INDEX:
      offset += index * sizeof(u16);
      calendar_read_data((offset), (u8 *)(&buf16), sizeof(u16));
      return buf16;

    case DATA_LMONTH:
    case DATA_CAL:
    case DATA_LHOLDAY:
    case DATA_YEAR_CODE:  
      offset += index * sizeof(u8);
      calendar_read_data((offset), (u8 *)(&buf8), sizeof(u8));
      return buf8;
    default:
      MT_ASSERT(0);
      return 0;
  }
}

static void calender_read_gbstr(u8 id, u16 index, u16 *buf)
{
  u16 offset = 0;
//  u16 tmp = 0;

  offset = data_offset[id];
  switch(id)
  {
    case DATA_ALMANAC:
      offset += index * 4 * sizeof(u16);// read 4 words.
      calendar_read_data(offset, (u8 *)(buf), 4 * sizeof(u16));
      *(buf + 4) = 0;
      break;
    case DATA_ANIMAL:
    case DATA_MONTH_CODE:
    case DATA_DAY_CODE:      
    case DATA_SKY:
    case DATA_EARTH:      
      offset += index * sizeof(u16);//read one word.
      calendar_read_data(offset, (u8 *)(buf), sizeof(u16));
      *(buf + 1) = 0;
      break;
    case DATA_SOLAR_TERM:
      offset += index * 2 * sizeof(u16);//read two words.
      calendar_read_data(offset, (u8 *)(buf), 2 * sizeof(u16));
      *(buf + 2) = 0;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  return;
}


