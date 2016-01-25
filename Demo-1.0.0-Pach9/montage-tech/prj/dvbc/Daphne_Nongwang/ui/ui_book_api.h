/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_BOOK_API_H__
#define __UI_BOOK_API_H__

#define BOOK_TIME_OUT_MS  1000

//Ԥ��event��̳���ʱ��(����)
#define MINIMUM_DURATION 1

enum latest_node_state
{
  LATEST_NODE_WAIT = 0,
  LATEST_NODE_START,
  LATEST_NODE_END,
};

typedef enum 
{
  BOOK_ERR_NO = 0,
  BOOK_ERR_PLAYING,
  BOOK_ERR_OVERTIME,
  BOOK_ERR_CONFILICT,
  BOOK_ERR_DUR_TOO_SHORT,
  BOOK_ERR_SAME,
  BOOK_ERR_FULL,

  BOOK_ERR_MAX_CNT
}book_error_t;

RET_CODE ui_book_init(void);

RET_CODE ui_book_tmr_release(void);

book_error_t check_book_pg(book_pg_t *p_node);

BOOL  is_time_booked(book_pg_t *p_node, u8 except_index);

BOOL is_node_playing(book_pg_t *p_node);

void book_get_book_node(u8 index, book_pg_t *p_node);

BOOL book_add_node(book_pg_t *p_node);

void book_edit_node(u8 index, book_pg_t *p_node);

BOOL book_delete_node(u8 index);

BOOL book_delete_node_all(void);

u8 book_get_book_num(void);

u8 book_check_latest_node(u8 index);

RET_CODE book_sort_by_start_time(void);

RET_CODE book_check_is_skip(u8 index, u8 *p_skip);

void book_check_node_on_delete(void);

void book_overdue_node_handler(u8 index);

u8 book_get_match_node(book_pg_t *p_node);

void book_delete_overdue_node(void);

void book_delete_all_conflict_node(book_pg_t *p_node);

u8 book_get_latest_index(void);

void ui_set_book_flag(BOOL is_enable);

BOOL ui_get_book_flag(void);
#endif 
