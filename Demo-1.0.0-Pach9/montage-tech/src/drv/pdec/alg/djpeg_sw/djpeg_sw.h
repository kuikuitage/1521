/******************************************************************************/
/******************************************************************************/
#ifndef __DJPEG_SW_H__
#define __DJPEG_SW_H__

RET_CODE djpeg_sw_getinf(lld_pic_t *p_lld,
                                unsigned char *data, 
                                unsigned int data_size, 
                                pic_info_t *p_info);


RET_CODE djpeg_sw_setinfo(lld_pic_t *p_lld,  pic_param_t *p_pic_param);

RET_CODE djpeg_sw_getline(lld_pic_t *p_lld,  unsigned char *p_dstline,u32 *p_line_num);

RET_CODE djpeg_sw_start(lld_pic_t *p_lld);

RET_CODE djpeg_sw_stop(lld_pic_t *p_lld);

#endif
