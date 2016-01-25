/******************************************************************************/
/******************************************************************************/
#ifndef __DGIF_SW_H__
#define __DGIF_SW_H__

RET_CODE dgif_sw_getinf(lld_pic_t *p_lld,
                                unsigned char *data, 
                                unsigned int data_size, 
                                pic_info_t *p_info, void ** p_ptr);


RET_CODE dgif_sw_setinfo(lld_pic_t *p_lld,  pic_param_t *p_pic_param,void * ptr);

RET_CODE dgif_sw_getline(lld_pic_t *p_lld,  unsigned char *p_dstline, u32 * p_line_num,void *ptr);

RET_CODE dgif_sw_start(lld_pic_t *p_lld,void * ptr);

RET_CODE dgif_sw_stop(lld_pic_t *p_lld, void * ptr);


#endif
